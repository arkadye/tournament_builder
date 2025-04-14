#include "tournament_builder/descriptors/descriptor_base.hpp"

namespace tournament_builder::descriptor
{
    DescriptorHandle parse_descriptor(const nlohmann::json& input)
    {
        return internal_descriptor::DescriptorBaseImpl::parse_descriptor(input);
    }
}
