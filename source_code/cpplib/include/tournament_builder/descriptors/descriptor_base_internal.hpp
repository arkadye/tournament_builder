#pragma once

#include "tournament_builder/name.hpp"
#include "tournament_builder/reference.hpp"
#include "tournament_builder/itaggable.hpp"
#include "tournament_builder/competition.hpp"
#include "tournament_builder/descriptors/descriptor_handle.hpp"
#include "tournament_builder/exceptions.hpp"

#include  "nlohmann/json_fwd.hpp"

#include <format>
#include <string_view>
#include <vector>
#include <optional>

namespace tournament_builder
{
	class World;
	class RealCompetition;
	namespace descriptor
	{
		template <typename T> class DescriptorBase;
	}
}

namespace tournament_builder::descriptor::internal_descriptor
{
	class DescriptorBaseImpl : public NamedElement , public TaggableMixin
	{
	public:
		using NamedElement::NamedElement;
		virtual ~DescriptorBaseImpl() = default;
		std::optional<RealCompetition> generate_wrapper() const;
		static DescriptorHandle parse_descriptor(const nlohmann::json& input);

		virtual Name get_descriptor_uid() const = 0;
		virtual void write_to_json(nlohmann::json& out) const; // Done this way to avoid having to include the full JSON header.
	protected:
		friend class DescriptorRegister;

		virtual std::optional<RealCompetition> generate() const = 0;

		// Checks the UID matches the input.
		void verify_input(const nlohmann::json& input) const;

		// This is a good place for common functions for manipluating these things.
		template <typename T>
		static void add_outer_to_references(std::vector<Reference<T>>& list)
		{
			for (Reference<T>& ref : list)
			{
				if (const SoftReference* sf = std::get_if<SoftReference>(&ref.data()))
				{
					std::string new_ref_str = std::format("@OUTER.{}", sf->to_string());
					SoftReference new_sf{ new_ref_str };
					Reference<T> new_reference{ std::move(new_sf)};
					ref = std::move(new_reference);
				}
			}
		}

		void add_name_and_descriptor_type_to_json(nlohmann::json& target) const;

		DescriptorHandle parse_wrapper(const nlohmann::json& input) const;
		virtual DescriptorHandle parse(const nlohmann::json& input) const = 0;
	};

	template <typename T>
	class DescriptorRegistrationObject
	{
	public:
		DescriptorRegistrationObject();
	};

	class DescriptorRegister
	{
	private:
		static void register_impl(DescriptorHandle new_element, Name uid);
	public:
		template <typename T>
		static void register_descriptor();
		static DescriptorHandle parse_descriptor(const nlohmann::json& input);
	};

	template<typename T>
	inline void DescriptorRegister::register_descriptor()
	{
		DescriptorHandle descriptor{ new T{ std::format("descriptor_examplar") } };
		Name uid = descriptor->get_descriptor_uid();
		register_impl(std::move(descriptor), uid);
	}

	template <typename T>
	DescriptorRegistrationObject<T>::DescriptorRegistrationObject()
	{
		internal_descriptor::DescriptorRegister::register_descriptor<T>();
	}
}

namespace tournament_builder::exception
{
	class WriteToJSONNotImplemented : public TournamentBuilderException
	{
	public:
		WriteToJSONNotImplemented(const tournament_builder::descriptor::internal_descriptor::DescriptorBaseImpl& target);
	};
}