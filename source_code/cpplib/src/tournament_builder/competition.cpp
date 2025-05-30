#include "tournament_builder/competition.hpp"
#include "tournament_builder/descriptors/descriptor_base.hpp"
#include "tournament_builder/competitor.hpp"

#include "tournament_builder/json/json_helpers.hpp"
#include "tournament_builder/tag_and_reference_helpers.hpp"

#include "nlohmann/json.hpp"

#include <algorithm>

namespace tournament_builder
{
	using nlohmann::json;
	bool RealCompetition::has_resolved_all_references() const
	{
		const bool contents_result = std::ranges::all_of(phases, [](const Competition& inner) {return inner.has_resolved_all_references(); });
		return contents_result && has_finalized_entry_list();
	}

	bool RealCompetition::has_finalized_entry_list() const
	{
		return std::ranges::all_of(entry_list, &Reference<Competitor>::is_resolved);
	}

	bool RealCompetition::resolve_all_references_impl(World& context, std::vector<Name>& location)
	{
		// In general, working outside-in will work better.
		try
		{
#ifndef NDEBUG
			auto location_copy = location;
#endif
			location.push_back(name);
			bool result = false;
			for (std::size_t i = 0u; i < entry_list.size(); ++i)
			{
				Reference<Competitor>& competitor_ref = entry_list[i];
				if (competitor_ref.is_reference())
				{
					result = competitor_ref.resolve(context, location) || result; // We DO NOT want to short circuit this.
					if (competitor_ref.is_resolved())
					{
						Competitor& competitor = competitor_ref.get();
						competitor.add_tag(Tag{ std::format("$ENTRY:{}", i + 1), false });
					}
				}
			}

			for (Competition& phase : phases)
			{
				result = phase.resolve_all_references_impl(context, location) || result;
			}

			location.pop_back();
#ifndef NDEBUG
			assert(location_copy == location);
#endif

			return result;
		}
		catch (tournament_builder::exception::TournamentBuilderException& ex)
		{
			ex.add_context(*this);
			throw ex;
		}
	}

	bool RealCompetition::resolve_all_references(World& context, std::vector<Name>& location)
	{
		// Keep trying until we fail to make a change
		while (resolve_all_references_impl(context, location));
		return has_resolved_all_references();
	}

	RealCompetition RealCompetition::parse(const json& input)
	{
		using namespace descriptor;
		json_helper::validate_type(input, json::value_t::object);

		// It's not a descriptor, so parse it properly.
		const Name name = Name::parse(input);
		RealCompetition result{ name };
		try
		{
			if (input.contains("phases"))
			{
				try
				{
					result.phases = json_helper::get_array(input["phases"], &Competition::parse);
				}
				catch (tournament_builder::exception::TournamentBuilderException& ex)
				{
					ex.add_context("While parsing 'phases'");
					throw ex;
				}
			}
			if (input.contains("entry_list"))
			{
				result.entry_list = Competitor::parse_entry_list(input, "entry_list");
			}
			result.add_tags_from_json(input);
			return result;
		}
		catch (tournament_builder::exception::TournamentBuilderException& ex)
		{
			ex.add_context(result);
			throw ex;
		}
	}

	std::shared_ptr<IReferencable> RealCompetition::copy_ref(const ReferenceCopyOptions&) const
	{
		Competition* copy = new Competition{ *this };
		IReferencable* icopy = copy;
		return std::shared_ptr<IReferencable>(icopy);
	}

	std::vector<IReferencable*> RealCompetition::get_all_next_locations()
	{
		std::vector<IReferencable*> result;
		result.reserve(entry_list.size() + phases.size());

		std::ranges::transform(entry_list | std::views::filter(&Reference<Competitor>::is_resolved),
			std::back_inserter(result),
			[](Reference<Competitor>& ref_comp) -> IReferencable*
			{
				return &ref_comp.get();
			});
		std::ranges::transform(phases, std::back_inserter(result), [](Competition& comp) {return &comp; });
		return result;
	}

	std::vector<IReferencable*> RealCompetition::get_next_locations_entry_tag(const Token& tag)
	{
		std::vector<IReferencable*> result;

		// If we still have unresolved references, skip this.
		if (std::ranges::any_of(entry_list, [](const Reference<Competitor>& rc) { return rc.is_reference(); }))
		{
			return result;
		}

		helpers::GetSpecialTagArgsResult arg_result = helpers::get_special_tag_args(tag, helpers::SpecialTagType::entry, ':');
		const auto entry_list_size = std::ssize(entry_list);

		// If neither max is below the range or min is above it, we find no results.
		if (arg_result.max < -entry_list_size) return result;
		if (arg_result.min > entry_list_size) return result;
		arg_result.max = std::min(arg_result.max, entry_list_size);
		arg_result.min = std::max(arg_result.min, -entry_list_size);
		assert(arg_result.min <= arg_result.max);
		const auto num_elements = 1 + arg_result.max - arg_result.min;
		result.reserve(num_elements);

		auto get_entry_positive_idx_only = [&entries = entry_list, entry_list_size](int64_t idx) -> IReferencable*
			{
				assert(idx > 0);
				assert(idx <= entry_list_size);
				Reference<Competitor>& result_ref = entries[idx - 1];
				assert(result_ref.is_resolved());
				Competitor& result = result_ref.get();
				return &result;
			};

		auto get_entry = [entry_list_size, &get_entry_positive_idx_only](int64_t idx)
			{
				assert(idx != 0);
				if (idx < 0)
				{
					const int64_t new_idx = 1 + entry_list_size + idx;
					return get_entry_positive_idx_only(new_idx);
				}
				// else
				return get_entry_positive_idx_only(idx);
			};

		for (int64_t i = arg_result.min; i <= arg_result.max; ++i)
		{
			if (i == 0) continue;
			IReferencable* entry = get_entry(i);
			result.push_back(entry);
		}
		return result;
	}

	std::vector<IReferencable*> RealCompetition::get_next_locations_pos_tags(const Token& tag)
	{
		std::vector<IReferencable*> result;
		// If we still have unresolved references, skip this.
		if (std::ranges::any_of(entry_list, [](const Reference<Competitor>& rc) { return rc.is_reference(); }))
		{
			return result;
		}

		helpers::GetSpecialTagArgsResult arg_result = helpers::get_special_tag_args(tag, helpers::SpecialTagType::entry, ':');
		const auto entry_list_size = std::ssize(entry_list);

		// If neither max is below the range or min is above it, we find no results.
		if (arg_result.max < -entry_list_size) return result;
		if (arg_result.min > entry_list_size) return result;
		arg_result.max = std::min(arg_result.max, entry_list_size);
		arg_result.min = std::max(arg_result.min, -entry_list_size);
		assert(arg_result.min <= arg_result.max);

		for (Reference<Competitor>& entry : entry_list)
		{
			assert(entry.is_resolved());
			Competitor& competitor = entry.get();
			const auto finishing_positions = competitor.get_finishing_positions();

			if (!finishing_positions.has_value()) continue;
			const auto [pos_low, pos_high] = finishing_positions.value();
			
			const int neg_low = pos_low - entry_list_size;
			const int neg_high = pos_high - entry_list_size;

			auto ranges_overlap = [&arg_result](int low, int high)
				{
					if (arg_result.max < low) return false;
					if (arg_result.min > high) return false;
					return true;
				};

			if (ranges_overlap(pos_low, pos_high) || ranges_overlap(neg_low, neg_high))
			{
				result.push_back(&competitor);
			}
		}
		std::ranges::sort(result, {},
			[](IReferencable* ir) {
				Competitor* comp = dynamic_cast<Competitor*>(ir);
				assert(comp != nullptr);
				const auto pos = comp->get_finishing_positions();
				assert(pos.has_value());
				return pos.value();
			});
		return result;
	}

	std::vector<IReferencable*> RealCompetition::get_next_locations(const Token& token)
	{
		{
			const Tag tag{ token };
			switch (tag.get_type())
			{
			case Tag::TagType::entry:
				return get_next_locations_entry_tag(token);
			case Tag::TagType::pos:
				return get_next_locations_pos_tags(token);
			default:
				break;
			}
		}

		std::vector<IReferencable*> result;
		result = get_all_next_locations();
		auto filter = [token](IReferencable* iref)
			{
				return !iref->matches_token(token);
			};
		auto remove_result = std::ranges::remove_if(result, filter);
		result.erase(begin(remove_result), end(remove_result));
		result.shrink_to_fit();
		return result;

	}

	bool Competition::has_resolved_all_references() const
	{
		if (const auto* rc = std::get_if<RealCompetition>(&m_data))
		{
			return rc->has_resolved_all_references();
		}
		return false;
	}

	bool Competition::has_finalized_entry_list() const
	{
		if (const auto* rc = std::get_if<RealCompetition>(&m_data))
		{
			return rc->has_finalized_entry_list();
		}
		return false;
	}

	bool Competition::resolve_all_references(World& context, std::vector<Name>& location)
	{
		while (resolve_all_references_impl(context, location));
		return has_resolved_all_references();
	}

	bool Competition::resolve_all_references_impl(World& context, std::vector<Name>& location)
	{
		using descriptor::DescriptorHandle;
		bool result = false;
		if (auto* p_desc = std::get_if<DescriptorHandle>(&m_data))
		{
			DescriptorHandle& desc = *p_desc;
			desc->resolve_contained_references(context, location);
			if (auto real_comp_opt = desc->generate_wrapper())
			{
				// Warning! This will invalidate p_desc and desc!!!
				// DO NOT access those variables after this move.
				m_data = std::move(*real_comp_opt);
				result = true;
			}
		}

		if (auto* p_real_comp = std::get_if<RealCompetition>(&m_data))
		{
			result = p_real_comp->resolve_all_references_impl(context, location) || result;
		}
		return result;
	}

	Competition Competition::parse(const nlohmann::json& input)
	{
		if (descriptor::DescriptorHandle descriptor_result = descriptor::parse_descriptor(input))
		{
			return Competition(std::move(descriptor_result));
		}
		RealCompetition rc = RealCompetition::parse(input);
		return Competition{ std::move(rc) };
	}

	Name Competition::get_reference_key() const
	{
		struct Impl
		{
			Name operator()(const descriptor::DescriptorHandle& rh) const { return rh->name; }
			Name operator()(const RealCompetition& rc) const { return rc.name; }
		};
		return std::visit(Impl{}, m_data);
	}

	std::shared_ptr<IReferencable> Competition::copy_ref(const ReferenceCopyOptions& reference_copy_options) const
	{
		struct Impl
		{
			const ReferenceCopyOptions& rco;
			std::shared_ptr<IReferencable> operator()(const descriptor::DescriptorHandle& dh) const
			{
				return std::make_shared<Competition>(dh);
			}
			std::shared_ptr<IReferencable> operator()(RealCompetition rc) const
			{
				rc.take_tags_via_reference(rc, rco);
				return std::make_shared<Competition>(rc);
			}
		};
		return std::visit(Impl{ reference_copy_options }, m_data);
	}

	std::vector<IReferencable*> Competition::get_all_next_locations()
	{
		if (auto* prc = std::get_if<RealCompetition>(&m_data))
		{
			return prc->get_all_next_locations();
		}
		return {};
	}

	std::vector<IReferencable*> Competition::get_next_locations(const Token& token)
	{
		if (auto* prc = std::get_if<RealCompetition>(&m_data))
		{
			return prc->get_next_locations(token);
		}
		return {};
	}

	const std::vector<Tag>& Competition::get_tags() const
	{
		struct Impl
		{
			const auto& impl(const TaggableMixin& tm) const { return tm.get_tags(); }
			const auto& operator()(const descriptor::DescriptorHandle& dh) const { return impl(*dh); }
			const auto& operator()(const RealCompetition& rc) const { return impl(rc); }
		};
		return std::visit(Impl{}, m_data);
	}

	void Competition::add_tag(Tag tag)
	{
		struct Impl
		{
			Tag t;
			void impl(TaggableMixin& tm) const { return tm.add_tag(std::move(t)); }
			void operator()(descriptor::DescriptorHandle& dh) const { impl(*dh); }
			void operator()(RealCompetition& rc) const { impl(rc); }
		};
		std::visit(Impl{std::move(tag)}, m_data);
	}

	void Competition::take_tags_via_reference(const ITaggable& other, const ReferenceCopyOptions& reference_copy_options)
	{
		struct Impl
		{
			const ITaggable& source;
			const ReferenceCopyOptions& rco;
			void impl(TaggableMixin& tm) const { return tm.take_tags_via_reference(source, rco); }
			void operator()(descriptor::DescriptorHandle& dh) const { impl(*dh); }
			void operator()(RealCompetition& rc) const { impl(rc); }
		};
		std::visit(Impl{ other, reference_copy_options }, m_data);
	}

	RealCompetition* Competition::get_real_competition()
	{
		return std::get_if<RealCompetition>(&m_data);
	}

	const RealCompetition* Competition::get_real_competition() const
	{
		return std::get_if<RealCompetition>(&m_data);
	}
}