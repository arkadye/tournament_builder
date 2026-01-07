#include "tournament_builder/descriptors/descriptor_base.hpp"

namespace tournament_builder::descriptor
{
	// This is for testing purposes only. It will *never* resolve to an actual competition.
	class DEBUG_NeverResolves : public DescriptorBase<DEBUG_NeverResolves>
	{
	public:
		std::vector<Reference<Competitor>> entry_list;

		using DescriptorBase::DescriptorBase;
		Name get_descriptor_uid() const override { return Name{ "DEBUG_never_resolves" }; }
		void write_to_json(nlohmann::json& target) const override;
	private:
		std::optional<RealCompetition> generate() const override { return std::nullopt; }
		DescriptorHandle parse(std::shared_ptr<DEBUG_NeverResolves> prototype, const nlohmann::json& input) const override;
	};
}