#include "tournament_builder/descriptors/descriptor_base.hpp"

namespace tournament_builder::descriptor
{
    DescriptorHandle parse_descriptor(const nlohmann::json& input, World& context)
    {
        return internal_descriptor::DescriptorBaseImpl::parse_descriptor(input, context);
    }
}
