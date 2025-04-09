#pragma once

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
	class Competitor;
	class Competition;

	// Because this is intended as library code I do not want to have to pull headers in throug this one unless absolutely necessary.
	// So I do some funky things with variants and meta-programming to pass highly constrained templates into the implementation file
	// (where I can #include as much as I need) and put the bulk of the code in there.

	namespace internal_reference
	{
		// Credit to https://stackoverflow.com/posts/45892305/revisions for this bit.
		template<typename T, typename VARIANT_T>
		struct IsVariantMember;

		template<typename T, typename... ALL_T>
		struct IsVariantMember<T, std::variant<ALL_T...>>
			: public std::disjunction<std::is_same<T, ALL_T>...> {
		};

		template <typename VARIANT_T>
		struct VariantOfVectors;

		template <typename... ALL_T>
		struct VariantOfVectors <std::variant<ALL_T...>>
		{
			using Type = std::variant<std::vector<ALL_T>...>;
		};

		template <typename VARIANT_T>
		using VariantOfVectorsType = VariantOfVectors<VARIANT_T>::Type;

		using ReferencableTypePtrsVariant = std::variant<Competitor*, Competition*>;

		template <typename T>
		struct IsReferencableType
			: public IsVariantMember<T*, ReferencableTypePtrsVariant> {
		};

		template <typename T>
		constexpr bool IsReferencableType_Value = IsReferencableType<T>::value;
	}

	template <typename T>
	concept Referencable = requires { internal_reference::IsReferencableType_Value<T>; };

	using Location = std::vector<Name>;

	template <Referencable T>
	class ReferenceResult
	{
	public:
		T* result = nullptr;
		Location location;
		operator bool() const noexcept { return result != nullptr; }
		auto operator<=>(const ReferenceResult& other) const noexcept
		{
			return this->result <=> other.result;
		}
		bool operator==(const ReferenceResult& other) const noexcept
		{
			return this->result == other.result;
		}
		std::string to_string() const;
	};

	namespace internal_reference
	{
		using VectorOfReferenceResultsVariant = std::variant<std::vector<ReferenceResult<Competition>>, std::vector<ReferenceResult<Competitor>>>;
	}

	class SoftReference
	{
		template <Referencable T> friend class Reference;
		std::vector<Token> m_elements;
	public:
		SoftReference() : SoftReference{ std::string_view{""} } {}
		explicit SoftReference(std::string_view input);
		SoftReference(const SoftReference&) = default;
		SoftReference(SoftReference&&) noexcept = default;
		SoftReference& operator=(const SoftReference&) = default;
		SoftReference& operator=(SoftReference&&) noexcept = default;
		std::string to_string() const;
		static SoftReference parse(const nlohmann::json& input);
	private:
		// This should only be accessed via a Reference<T>
		template <Referencable T> std::vector<ReferenceResult<T>> dereference_to(World& context, const Location& location) const;
		internal_reference::VectorOfReferenceResultsVariant dereference_to(World& context, const Location& location, internal_reference::ReferencableTypePtrsVariant impl_chooser) const;
		static std::optional<SoftReference> try_parse(const nlohmann::json& input);
		[[noreturn]] void throw_invalid_dereference() const;
	};

	template <Referencable T>
	class Reference
	{
		mutable std::variant<SoftReference, T> m_data;
	public:
		explicit Reference(SoftReference input) : m_data{std::move(input)} {}
		explicit Reference(T input) : m_data{ std::move(input) } {}
		explicit Reference(std::string_view input) : Reference{ SoftReference{input} } {}

		Reference(const Reference&) = default;
		Reference(Reference&&) noexcept = default;
		Reference& operator=(const Reference&) = default;
		Reference& operator=(Reference&&) noexcept = default;

		std::vector<ReferenceResult<T>> dereference_multi(World& context, const Location& location) const;
		ReferenceResult<T> dereference_single(World& context, const Location& location) const;
		T& get() const;
		bool is_reference() const noexcept { return std::holds_alternative<SoftReference>(m_data); }
		bool is_resolved() const noexcept { return std::holds_alternative<T>(m_data); }

		// Replace a soft reference, if there is one, with the actual T it points to.
		// Returns true if a change was made. (So false if this was already resolved, or if it could not be resolved to a single item).
		bool resolve(World& context, const Location& location);

		std::string to_string() const;

		static Reference parse(const nlohmann::json& input);
		const std::variant<SoftReference, T>& data() const noexcept { return m_data; }
	private:
		[[noreturn]] void throw_invalid_dereference() const { assert(is_reference()); std::get<SoftReference>(m_data).throw_invalid_dereference(); }
	};
	template<Referencable T>
	inline std::string ReferenceResult<T>::to_string() const
	{
		std::ostringstream oss;
		for (Name t : location)
		{
			oss << t << '.';
		}
		oss << (result != nullptr ? result->to_string() : "[nullptr]");
		return oss.str();
	}
}

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

template <tournament_builder::Referencable T>
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

template <tournament_builder::Referencable T>
struct std::formatter<tournament_builder::ReferenceResult<T>>
{
	template <class PrsContext>
	constexpr auto parse(const PrsContext& prsContext)
	{
		return prsContext.begin();
	}

	auto format(tournament_builder::ReferenceResult<T> ref_result, std::format_context& ctx) const
	{
		if (ref_result.result == nullptr)
		{
			return std::format_to(ctx.out(), {}, "[Not found]");
		}
		std::ostringstream oss;
		oss << '[';
		bool first = true;
		for (tournament_builder::Name n : ref_result.location)
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
		oss << "]." << ref_result.result->name.to_string();
		return std::format_to(ctx.out(), "{}", oss.str());
	}
};

namespace tournament_builder
{
	// Implementations
	template<Referencable T>
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
			std::vector<ReferenceResult<T>> operator()(T& data)
			{
				return { { &data , loc } };
			}
		};
		return std::visit(Impl{ context, location }, m_data);
	}

	template<Referencable T>
	inline bool Reference<T>::resolve(World& context, const Location& location)
	{
		if (is_reference())
		{
			ReferenceResult<T> candidate = dereference_single(context, location);
			T* result = candidate.result;
			if(result != nullptr)
			{
				m_data = result->copy_ref();
				return true;
			}
		}
		return false;
	}

	template<Referencable T>
	inline ReferenceResult<T> Reference<T>::dereference_single(World& context, const Location& location) const
	{
		struct Impl
		{
			const Reference<T>& outer;
			World& con;
			const Location& loc;
			ReferenceResult<T> operator()(const SoftReference& sf)
			{
				const std::vector<ReferenceResult<T>> result = outer.dereference_multi(con, loc);
				return result.size() == 1u ? std::move(result.front()) : ReferenceResult<T>{};
			}
			ReferenceResult<T> operator()(T& data)
			{
				return { &data , loc };
			}
		};
		return std::visit(Impl{ *this, context, location }, m_data);
	}

	template<Referencable T>
	inline T& Reference<T>::get() const
	{
		if (is_reference())
		{
			throw_invalid_dereference();
		}
		assert(std::holds_alternative<T>(m_data));
		return std::get<T>(m_data);
	}

	template<Referencable T>
	inline std::string Reference<T>::to_string() const
	{
		struct Impl
		{
			std::string operator()(const T& t) const { return std::format("name='{}'", t.to_string());	}
			std::string operator()(const SoftReference& sr) const { return std::format("{}", sr); }
		};
		return std::visit(Impl{}, m_data);
	}

	template<Referencable T>
	inline Reference<T> Reference<T>::parse(const nlohmann::json& input)
	{
		if (std::optional<SoftReference> sf = SoftReference::try_parse(input))
		{
			return Reference<T>{ std::move(sf.value()) };
		}
		return Reference<T>{T::parse(input)};
	}

	template<Referencable T>
	inline std::vector<ReferenceResult<T>> SoftReference::dereference_to(World& context, const Location& location) const
	{
		using namespace internal_reference;
		using RefPtr = T*;
		using ResultType = std::vector<ReferenceResult<T>>;
		auto result = dereference_to(context, location, ReferencableTypePtrsVariant{ RefPtr{nullptr} });
		assert(std::holds_alternative<ResultType>(result));
		return std::move(std::get<ResultType>(result));
	}
}