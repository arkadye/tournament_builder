#include "tournament_builder/descriptors/descriptor_base_internal.hpp"
#include "tournament_builder/json/json_helpers.hpp"


#include <cassert>
#include <vector>

namespace tournament_builder::descriptor::internal_descriptor
{
	inline std::vector<std::pair<Name, DescriptorHandle>> m_derived_descriptors_register;

	auto find_descriptor(Name uid)
	{
		return std::ranges::find(m_derived_descriptors_register, uid, &std::pair<Name, DescriptorHandle>::first);
	}

	bool contains_descriptor(Name uid)
	{
		return find_descriptor(uid) == end(m_derived_descriptors_register);
	}

	void DescriptorBaseImpl::verify_input(const nlohmann::json& input) const
	{
		const std::string type = json_helper::get_string(input, "descriptor_type");
		assert(std::string_view{ type } == get_descriptor_uid());
	}

	DescriptorHandle DescriptorBaseImpl::parse_wrapper(const nlohmann::json& input) const
	{
		DescriptorHandle result = parse(input);
		result->add_tags_from_json(input);
		return result;
	}

	void DescriptorRegister::register_impl(DescriptorHandle new_element, Name uid)
	{
		assert(new_element != nullptr);
		const Name descriptor_uid = new_element->get_descriptor_uid();
		assert(descriptor_uid == uid);
		assert(contains_descriptor(uid) && "Duplicate UID found");
		m_derived_descriptors_register.emplace_back(uid, std::move(new_element));
	}

	DescriptorHandle DescriptorRegister::parse_descriptor(const nlohmann::json& input)
	{
		const std::optional<std::string> descriptor_type = json_helper::get_optional_string(input, "descriptor_type");
		if (!descriptor_type.has_value())
		{
			return nullptr;
		}
		const auto find_it = find_descriptor(Name{ *descriptor_type });
		if (find_it == end(m_derived_descriptors_register))
		{
			throw exception::JsonInvalidDescriptor{ input, *descriptor_type };
		}

		const auto& exemplar = find_it->second;
		return exemplar->parse_wrapper(input);
	}

	std::optional<RealCompetition> DescriptorBaseImpl::generate_wrapper() const
	{
		std::optional<RealCompetition> result = generate();
		if(result.has_value())
		{
			static_cast<TaggableMixin&>(*result) = *this;
		}
		return result;
	}

	DescriptorHandle DescriptorBaseImpl::parse_descriptor(const nlohmann::json& input)
	{
		return DescriptorRegister::parse_descriptor(input);
	}
}