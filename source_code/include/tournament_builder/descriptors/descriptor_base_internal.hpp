#pragma once

#include "tournament_builder/name.hpp"
#include "tournament_builder/competition.hpp"
#include "tournament_builder/reference.hpp"

#include  "nlohmann/json_fwd.hpp"

#include <memory>
#include <format>
#include <string_view>
#include <vector>

namespace tournament_builder
{
	class World;
}

namespace tournament_builder::descriptor::internal_descriptor
{
	class DescriptorBaseImpl;

	using DescriptorHandle = std::unique_ptr<DescriptorBaseImpl>;

	class DescriptorBaseImpl : public NamedElement
	{
	public:
		using NamedElement::NamedElement;
		virtual Competition generate() const = 0;
		virtual Name get_descriptor_uid() const = 0;
		virtual DescriptorHandle parse(const nlohmann::json& input) const = 0;
		static DescriptorHandle parse_descriptor(const nlohmann::json& input);

	protected:
		// Checks the UID matches the input.
		void verify_input(const nlohmann::json& input) const;

		// This is a good place for common functions for manipluating these things.
		template <Referencable T>
		static void add_outer_to_references(std::vector<Reference<T>>& list)
		{
			for (Reference<T>& ref : list)
			{
				if (const SoftReference* sf = std::get_if<SoftReference>(&ref.data()))
				{
					ref = Reference<T>{ std::format("@OUTER.{}", sf->to_string()) };
				}
			}
		}
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
		static std::unique_ptr<DescriptorBaseImpl> parse_descriptor(const nlohmann::json& input);
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