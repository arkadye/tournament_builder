#pragma once

#include "tournament_builder/ireferencable.hpp"
#include "tournament_builder/token.hpp"
#include "tournament_builder/name.hpp"

#include "nlohmann/json_fwd.hpp"

#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <cassert>
#include <sstream>
#include <algorithm>

namespace tournament_builder
{
	class World;
	using Location = ::std::vector<Name>;
	namespace internal_reference
	{
		class ReferenceBase;

		class ReferenceResultBase
		{
			Location location;
		protected:
			IReferencable* result = nullptr;
		public:
			ReferenceResultBase() = default;
			ReferenceResultBase(IReferencable* a_result, Location a_location)
				: location(std::move(a_location)), result{ a_result }
			{
			}

			const Location& get_location() const { return location; }
			Location& get_location() { return location; }

			operator bool() const noexcept { return result != nullptr; }

			auto operator<=>(const ReferenceResultBase& other) const noexcept
			{
				return this->result <=> other.result;
			}
			bool operator==(const ReferenceResultBase& other) const noexcept
			{
				return this->result == other.result;
			}
			std::string to_string() const;
		};
	}

	class LocationPusher
	{
	private:
		Location& location;
#ifndef NDEBUG
		Location original_location;
#endif
	public:
		[[nodiscard]] LocationPusher(Location& loc, const Name& name);
		~LocationPusher();
	};

	template <typename T>
	class ReferenceResult : public internal_reference::ReferenceResultBase
	{
		static_assert(std::is_base_of_v<IReferencable, T>);
	public:
		using internal_reference::ReferenceResultBase::ReferenceResultBase;
		ReferenceResult(const ReferenceResultBase& other) : ReferenceResultBase{ other } {}
		T* get() const
		{
			if (result == nullptr) return nullptr;
			return result->get_as<T>();
		}

		bool is_correct_type() const
		{
			if (result != nullptr)
			{
				return result->is_a<T>();
			}
			return false;
		}
	};

	class SoftReference
	{
		using Replacement = std::pair<std::string, std::string>;
		template <typename T> friend class Reference;
		friend class internal_reference::ReferenceBase;
		std::vector<Token> m_elements;
		std::vector<Replacement> m_replacements;
		int32_t m_min_results = 1;
		int32_t m_max_results = 1;
		ReferenceCopyOptions m_copy_opts;
	public:
		SoftReference() : SoftReference{ std::string_view{""} } {}
		explicit SoftReference(std::string_view input);
		SoftReference(const SoftReference&) = default;
		SoftReference(SoftReference&&) noexcept = default;
		SoftReference& operator=(const SoftReference&) = default;
		SoftReference& operator=(SoftReference&&) noexcept = default;
		std::string to_string() const;
		const ReferenceCopyOptions& get_copy_opts() const { return m_copy_opts; }
		static SoftReference parse(const nlohmann::json& input);
		int32_t get_min_results() const noexcept { return m_min_results; }
		int32_t get_max_results() const noexcept { return m_max_results; }
		bool has_fixed_size() const noexcept { return get_min_results() == get_max_results(); }
		bool has_single_item() const noexcept { return has_fixed_size() && get_min_results() == 1; }
	private:
		// This should only be accessed via a Reference
		template <typename T>
		std::vector<ReferenceResult<T>> dereference_to(World& context, const Location& location) const;
		std::vector<internal_reference::ReferenceResultBase> dereference_to_impl(World& context, const Location& location) const;
		static std::optional<SoftReference> try_parse(const nlohmann::json& input);
		[[noreturn]] void throw_invalid_dereference() const;
	};

	namespace internal_reference
	{
		class ReferenceBase
		{
		protected:
			mutable std::variant<SoftReference, std::shared_ptr<IReferencable>> m_data;
		public:
			explicit ReferenceBase(SoftReference input) : m_data{ std::move(input) } {}
			explicit ReferenceBase(std::shared_ptr<IReferencable> input) : m_data{ std::move(input) } {}
			explicit ReferenceBase(std::string_view input) : ReferenceBase{ SoftReference{input} } {}

			ReferenceBase(const ReferenceBase&) = default;
			ReferenceBase(ReferenceBase&&) noexcept = default;
			ReferenceBase& operator=(const ReferenceBase&) = default;
			ReferenceBase& operator=(ReferenceBase&&) noexcept = default;

			bool is_reference() const noexcept { return std::holds_alternative<SoftReference>(m_data); }
			bool is_resolved() const noexcept { return std::holds_alternative<std::shared_ptr<IReferencable>>(m_data); }
			int32_t get_min_results() const noexcept;
			int32_t get_max_results() const noexcept;
			bool has_fixed_size() const noexcept;
			bool has_single_item() const noexcept;

			std::string to_string() const;

			ReferenceCopyOptions get_copy_opts() const;

			const std::variant<SoftReference, std::shared_ptr<IReferencable>>& data() const noexcept { return m_data; }
		protected:
			static std::optional<SoftReference> try_parse_soft_reference(const nlohmann::json& input) { return SoftReference::try_parse(input); }
			[[noreturn]] void throw_invalid_dereference() const;
		};
	}

	template <typename T>
	class Reference : public internal_reference::ReferenceBase
	{
		static_assert(std::is_base_of_v<IReferencable, T>);
	public:
		using internal_reference::ReferenceBase::ReferenceBase;
		Reference(T input);
		std::vector<ReferenceResult<T>> dereference_multi(World& context, const Location& location) const;
		ReferenceResult<T> dereference_single(World& context, const Location& location) const;
		T& get() const;

		static Reference parse(const nlohmann::json& input);

	};
	
}

namespace tournament_builder
{
	// Implementations
	template <typename T>
	inline std::vector<ReferenceResult<T>> Reference<T>::dereference_multi(World& context, const Location& location) const
	{
		struct Impl
		{
			World& con;
			const Location& loc;
			std::vector<ReferenceResult<T>> operator()(const SoftReference& sf)
			{
				return sf.dereference_to<T>(con, loc);
			}
			std::vector<ReferenceResult<T>> operator()(const std::shared_ptr<IReferencable>& data)
			{
				return { { data.get(), loc } };
			}
		};
		return std::visit(Impl{ context, location }, m_data);
	}

	template <typename T>
	inline ReferenceResult<T> Reference<T>::dereference_single(World& context, const Location& location) const
	{
		struct Impl
		{
			const Reference& outer;
			World& con;
			const Location& loc;
			ReferenceResult<T> operator()(const SoftReference& sf)
			{
				std::vector<ReferenceResult<T>> result = outer.dereference_multi(con, loc);
				return result.size() == 1u ? std::move(result.front()) : ReferenceResult<T>{};
			}
			ReferenceResult<T> operator()(const std::shared_ptr<IReferencable>& data)
			{
				return { data.get() , loc};
			}
		};
		return std::visit(Impl{ *this, context, location }, m_data);
	}

	template <typename T>
	inline T& Reference<T>::get() const
	{
		if (is_reference())
		{
			throw_invalid_dereference();
		}
		assert(std::holds_alternative<std::shared_ptr<IReferencable>>(m_data));
		const auto& up_result = std::get<std::shared_ptr<IReferencable>>(m_data);
		auto* ptr_result = up_result.get();
		T* result = dynamic_cast<T*>(ptr_result);
		if (result == nullptr)
		{
			throw_invalid_dereference();
		}
		return *result;
	}

	template<typename T>
	inline Reference<T> Reference<T>::parse(const nlohmann::json& input)
	{
		if (std::optional<SoftReference> sf = try_parse_soft_reference(input))
		{
			return Reference{ std::move(sf.value()) };
		}
		return Reference{T::parse(input)};
	}

	template <typename T>
	inline Reference<T>::Reference(T input) : internal_reference::ReferenceBase{ std::shared_ptr<IReferencable>{new T{std::move(input)}} }
	{}

	template<typename T>
	inline std::vector<ReferenceResult<T>> SoftReference::dereference_to(World& context, const Location& location) const
	{
		const std::vector<internal_reference::ReferenceResultBase> base_result = dereference_to_impl(context, location);
		std::vector<ReferenceResult<T>> result;
		result.reserve(base_result.size());
		std::ranges::copy(base_result, std::back_inserter(result));
		return result;
	}
}

// FORMATTERS
template <>
struct std::formatter<tournament_builder::SoftReference>
{
	template <class PrsContext>
	constexpr auto parse(const PrsContext& prsContext)
	{
		return prsContext.begin();
	}

	auto format(tournament_builder::SoftReference ref, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "{}", ref.to_string());
	}
};

template <typename T>
struct std::formatter<tournament_builder::Reference<T>>
{
	template <class PrsContext>
	constexpr auto parse(const PrsContext& prsContext)
	{
		return prsContext.begin();
	}

	auto format(tournament_builder::Reference<T> ref, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "{}", ref.to_string());
	}
};

template <typename T>
struct std::formatter<tournament_builder::ReferenceResult<T>>
{
	template <class PrsContext>
	constexpr auto parse(const PrsContext& prsContext)
	{
		return prsContext.begin();
	}

	auto format(const tournament_builder::ReferenceResult<T>& ref_result, std::format_context& ctx) const
	{
		if (!ref_result)
		{
			return std::format_to(ctx.out(), {}, "[Not found]");
		}
		std::ostringstream oss;
		oss << '[';
		bool first = true;
		for (const tournament_builder::Name& n : ref_result.get_location())
		{
			if (first)
			{
				first = false;
			}
			else
			{
				oss << '.';
			}
			oss << n.to_string();
		}
		const auto* as_named = ref_result.template get_as<tournament_builder::NamedElement>();
		oss << "]." << as_named->name.to_string();
		return std::format_to(ctx.out(), "{}", oss.str());
	}
};