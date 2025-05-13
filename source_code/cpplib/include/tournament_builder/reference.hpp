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
		template <typename T> friend class Reference;
		friend class internal_reference::ReferenceBase;
		std::vector<Token> m_elements;
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

		// Replace a soft reference, if there is one, with the actual T it points to.
		// Returns true if a change was made. (So false if this was already resolved, or if it could not be resolved to a single item).
		bool resolve(World& context, const Location& location);
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
	inline bool Reference<T>::resolve(World& context, const Location& location)
	{
		if (is_reference())
		{
			ReferenceResult<T> candidate = dereference_single(context, location);
			if (candidate.is_correct_type())
			{
				IReferencable* result = candidate.get();
				if (result != nullptr)
				{
					m_data = result->copy_ref(get_copy_opts());
					return true;
				}
			}
		}
		return false;
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
				const std::vector<ReferenceResult<T>> result = outer.dereference_multi(con, loc);
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

	inline std::string internal_reference::ReferenceBase::to_string() const
	{
		struct Impl
		{
			std::string operator()(const std::shared_ptr<IReferencable>& ir) const
			{
				return std::format("name='{}'", ir->get_reference_key());

			}
			std::string operator()(const SoftReference& sr) const { return std::format("{}", sr.to_string()); }
		};
		return std::visit(Impl{}, m_data);
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