#include "NoScopeSequences.h"

#include "Generic/GenericStringPropertySequence.h"
#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

using namespace menu;

namespace menu::no_scope_sequences
{
    class SequenceOpenGlobalScope final : public MenuFileParser::sequence_t
    {
    public:
        SequenceOpenGlobalScope()
        {
            const MenuMatcherFactory create(this);

            AddMatchers({
                create.Char('{')
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            state->m_in_global_scope = true;
        }
    };
}

using namespace no_scope_sequences;

NoScopeSequences::NoScopeSequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences)
    : AbstractScopeSequenceHolder(allSequences, scopeSequences)
{
}

void NoScopeSequences::AddSequences(FeatureLevel featureLevel)
{
    AddSequence(std::make_unique<SequenceOpenGlobalScope>());
}
