#pragma once

#include "BaseTemplate.h"
#include "Domain/Evaluation/OperandDynamic.h"
#include "Domain/Evaluation/Operation.h"
#include "Generating/PerAssetRenderingContext.h"

#include <format>

class BasePerVariantTemplate : public BaseTemplate
{
protected:
    BasePerVariantTemplate(std::ostream& stream, const PerVariantRenderingContext& context);

    [[nodiscard]] std::string MakeAllocAlignment(const StructureInformation& info) const;

    [[nodiscard]] size_t SizeForDeclModifierLevel(const MemberInformation& memberInfo, size_t level) const;
    [[nodiscard]] size_t
        OffsetForMemberModifier(const MemberInformation& memberInfo, const DeclarationModifierComputations& modifier, size_t nestedBaseOffset) const;

    /**
     * Evaluates whether the struct/union has matching memory layouts between the generated code and game.
     */
    [[nodiscard]] bool MemoryLayoutMatches(const StructureInformation& structureInfo) const;

    const PerVariantRenderingContext& m_env;
};
