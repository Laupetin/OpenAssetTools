#include "TechniqueNoScopeSequences.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

#include <cassert>

using namespace techset;

namespace techset
{
    class SequencePass final : public TechniqueParser::sequence_t
    {
    public:
        SequencePass()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({create.Char('{')});
        }

    protected:
        void ProcessMatch(TechniqueParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_in_pass == false);
            state->m_in_pass = true;

            state->m_acceptor->AcceptNextPass();
        }
    };
} // namespace techset

const std::vector<TechniqueParser::sequence_t*>& TechniqueNoScopeSequences::GetSequences()
{
    static std::vector<TechniqueParser::sequence_t*> tests({new SequencePass()});

    return tests;
}
