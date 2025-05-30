#pragma once

#include "tournament_builder/descriptors/descriptor_base_internal.hpp"
#include "tournament_builder/generic_utils.hpp"

namespace tournament_builder::descriptor
{
	DescriptorHandle parse_descriptor(const nlohmann::json& input);

	template <typename T>
	class DescriptorBase : public internal_descriptor::DescriptorBaseImpl
	{
	public:
		explicit DescriptorBase(std::string_view name);

		// Return a UNIQUE Name used in the parser to identify which type to parse.
		// This will match up with the "descriptor_type" field in the JSON.
		virtual Name get_descriptor_uid() const = 0;

		// Parse the JSON object here.
		virtual DescriptorHandle parse(const nlohmann::json& input) const = 0;

		// Overwrite this if generate() can ever fail. This is used to serialize the descriptor if it isn't resolved.
		void write_to_json(nlohmann::json& target) const override { internal_descriptor::DescriptorBaseImpl::write_to_json(target); }

		// If this is ever copied via reference, this is used. By default it uses the copy constructor.
		std::shared_ptr<IReferencable> copy_ref(const ReferenceCopyOptions&) const;

		// Override this to allow parts of the descriptor to be found via a reference.
		std::vector<IReferencable*> get_next_locations() override { return {}; }

		// If there are overridden references that you may want to resolve before resolving
		// this descriptor, resolve them in this function.
		void resolve_contained_references(World& world, std::vector<Name>& location) override { internal_descriptor::DescriptorBaseImpl::resolve_contained_references(world, location); }

	protected:
		// Implement this function to generate the actual competition structure we want.
		std::optional<RealCompetition> generate() const override = 0;

		// When generating, if you move a something into an internal structure
		// you need to adjust the references to have an extra @OUTER on the front. This will
		// do that for you.
		using internal_descriptor::DescriptorBaseImpl::add_outer_to_references;

		// This runs some asserts on the input. It should never fail, regardless of
		// how the class is customised.

		// void verify_input(const nlohmann::json& input) const;
		using internal_descriptor::DescriptorBaseImpl::verify_input;

	private:
		inline static internal_descriptor::DescriptorRegistrationObject<T> m_register_object;
	};

	template <typename T>
	inline DescriptorHandle DescriptorBase<T>::parse(const nlohmann::json& input) const
	{
		return internal_descriptor::DescriptorBaseImpl::parse(input);
	}

	template<typename T>
	inline std::shared_ptr<IReferencable> DescriptorBase<T>::copy_ref(const ReferenceCopyOptions& rco) const
	{
		const T* this_as_actual_type = dynamic_cast<const T*>(this);
		auto result = std::make_shared<T>(*this_as_actual_type);
		result->take_tags_via_reference(*this, rco);
		return result;
	}

	template<typename T>
	inline DescriptorBase<T>::DescriptorBase(std::string_view name)
		: internal_descriptor::DescriptorBaseImpl{ name }
	{
		auto make_sure_this_has_an_address = &m_register_object;
	}
}