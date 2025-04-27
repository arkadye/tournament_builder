#pragma once

#include "tournament_builder/descriptors/descriptor_base_internal.hpp"
#include "tournament_builder/generic_utils.hpp"

namespace tournament_builder::descriptor
{
	using DescriptorHandle = internal_descriptor::DescriptorHandle;

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
	inline DescriptorBase<T>::DescriptorBase(std::string_view name)
		: internal_descriptor::DescriptorBaseImpl{ name }
	{
		auto make_sure_this_has_an_address = &m_register_object;
	}
}