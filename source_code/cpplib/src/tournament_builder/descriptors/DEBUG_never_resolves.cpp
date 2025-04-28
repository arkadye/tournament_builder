#include "tournament_builder/descriptors/DEBUG_never_resolves.hpp"

namespace tournament_builder::descriptor
{
    void DEBUG_NeverResolves::write_to_json(nlohmann::json& target) const
    {
        add_name_and_descriptor_type_to_json(target);
        target["comment"] = "This descriptor will *never* resolve. The output should be the same as the input (more or less).";
    }

    DescriptorHandle DEBUG_NeverResolves::parse(const nlohmann::json& input) const
    {
        Name name = Name::parse(input);
        return std::make_shared<DEBUG_NeverResolves>(name);
    }
}