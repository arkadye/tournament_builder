#pragma once

#include <memory>

namespace tournament_builder::descriptor
{
	namespace internal_descriptor
	{
		class DescriptorBaseImpl;
	}
	using DescriptorHandle = std::shared_ptr<internal_descriptor::DescriptorBaseImpl>;
}