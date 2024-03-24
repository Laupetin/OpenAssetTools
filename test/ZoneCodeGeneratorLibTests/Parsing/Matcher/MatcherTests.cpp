#include "Parsing/Header/Impl/HeaderParserValue.h"
#include "Parsing/Header/Matcher/HeaderMatcherFactory.h"
#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Parsing/Mock/MockLexer.h"
#include "Parsing/Mock/MockSequence.h"
#include "Utils/ClassUtils.h"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <sstream>

namespace test::parsing::matcher
{
    // This class uses the header implementation of IParserValues
    // This should not make a difference in behaviour and only affects test data

    typedef MockLexer<HeaderParserValue> lexer_t;
    typedef MockSequence<HeaderParserValue> sequence_t;
    typedef sequence_t::parent_t::matcher_t matcher_t;
    typedef SequenceResult<HeaderParserValue> sequence_result_t;
    typedef HeaderMatcherFactory factory_t;

    class MatchersTestsHelper
    {
        std::unique_ptr<MockSequenceState> m_mock_state;
        std::unique_ptr<lexer_t> m_lexer;
        std::unique_ptr<sequence_t> m_sequence;

        unsigned m_consumed_token_count;

    public:
        MatchersTestsHelper()
            : m_mock_state(std::make_unique<MockSequenceState>()),
              m_sequence(std::make_unique<sequence_t>()),
              m_consumed_token_count(0)
        {
        }

        void Tokens(std::initializer_list<Movable<HeaderParserValue>> tokens)
        {
            m_lexer = std::make_unique<lexer_t>(tokens, HeaderParserValue::EndOfFile(TokenPos()));
        }

        void Matchers(const std::initializer_list<Movable<std::unique_ptr<matcher_t>>> matchers) const
        {
            m_sequence->AddMockMatchers(matchers);
        }

        void LabeledMatchers(const std::initializer_list<Movable<std::unique_ptr<matcher_t>>> matchers, const int label) const
        {
            m_sequence->AddMockLabeledMatchers(matchers, label);
        }

        _NODISCARD factory_t Factory() const
        {
            return HeaderMatcherFactory(m_sequence->GetLabelSupplier());
        }

        void MatchCallback(std::function<void(sequence_result_t& result)> cb) const
        {
            m_sequence->Handle(std::move(cb));
        }

        bool PerformTest()
        {
            // Tokens must be set first
            REQUIRE(m_lexer.get() != nullptr);
            return m_sequence->MatchSequence(m_lexer.get(), m_mock_state.get(), m_consumed_token_count);
        }

        _NODISCARD unsigned GetConsumedTokenCount() const
        {
            return m_consumed_token_count;
        }
    };

    TEST_CASE("Matcher: Ensure can match simple AND token sequence", "[parsing][matcher]")
    {
        MatchersTestsHelper test;
        const TokenPos pos;
        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Identifier(pos, new std::string("test_namespace")),
            HeaderParserValue::Character(pos, '{'),
            HeaderParserValue::Invalid(pos),
        });
        const auto create = test.Factory();
        test.Matchers({
            create.Type(HeaderParserValueType::NAMESPACE),
            create.Identifier(),
            create.Char('{'),
        });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 3);
    }

    TEST_CASE("Matcher: Ensure AND matcher can fail", "[parsing][matcher]")
    {
        MatchersTestsHelper test;
        const TokenPos pos;
        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Identifier(pos, new std::string("test_namespace")),
            HeaderParserValue::Character(pos, '{'),
            HeaderParserValue::Invalid(pos),
        });
        const auto create = test.Factory();
        test.Matchers({
            create.Type(HeaderParserValueType::NAMESPACE),
            create.Identifier(),
            create.Char('+'),
        });

        REQUIRE(!test.PerformTest());
    }

    TEST_CASE("Matcher: Ensure match callback is called", "[parsing][matcher]")
    {
        MatchersTestsHelper test;
        const TokenPos pos;
        auto callbackCalled = false;
        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Identifier(pos, new std::string("test_namespace")),
            HeaderParserValue::Character(pos, '{'),
            HeaderParserValue::Invalid(pos),
        });
        const auto create = test.Factory();
        test.Matchers({
            create.Type(HeaderParserValueType::NAMESPACE),
        });
        test.MatchCallback(
            [&callbackCalled](sequence_result_t& result)
            {
                callbackCalled = true;
            });

        REQUIRE(test.PerformTest());
        REQUIRE(callbackCalled);
    }

    TEST_CASE("Matcher: Ensure match callback is not called on fail", "[parsing][matcher]")
    {
        MatchersTestsHelper test;
        const TokenPos pos;
        auto callbackCalled = false;
        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Invalid(pos),
        });
        const auto create = test.Factory();
        test.Matchers({
            create.Type(HeaderParserValueType::NAMESPACE),
        });
        test.MatchCallback(
            [&callbackCalled](sequence_result_t& result)
            {
                callbackCalled = true;
            });

        REQUIRE(!test.PerformTest());
        REQUIRE(!callbackCalled);
    }

    TEST_CASE("Matcher: Ensure can match simple OR token sequence", "[parsing][matcher]")
    {
        static constexpr auto TAG_NAMESPACE = 1;
        static constexpr auto TAG_STRUCT = 2;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create.Or({
                create.Type(HeaderParserValueType::NAMESPACE).Tag(TAG_NAMESPACE),
                create.Type(HeaderParserValueType::STRUCT).Tag(TAG_STRUCT),
            }),
        });

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Identifier(pos, new std::string("test_namespace")),
            HeaderParserValue::Character(pos, '{'),
            HeaderParserValue::Invalid(pos),
        });
        test.MatchCallback(
            [](sequence_result_t& result)
            {
                REQUIRE(result.NextTag() == TAG_NAMESPACE);
            });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 1);

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Identifier(pos, new std::string("test_struct")),
            HeaderParserValue::Character(pos, '{'),
            HeaderParserValue::Invalid(pos),
        });
        test.MatchCallback(
            [](sequence_result_t& result)
            {
                REQUIRE(result.NextTag() == TAG_STRUCT);
            });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 1);
    }

    TEST_CASE("Matcher: Ensure OR matcher can fail", "[parsing][matcher]")
    {
        static constexpr auto TAG_NAMESPACE = 1;
        static constexpr auto TAG_STRUCT = 2;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create.Or({
                create.Type(HeaderParserValueType::NAMESPACE).Tag(TAG_NAMESPACE),
                create.Type(HeaderParserValueType::STRUCT).Tag(TAG_STRUCT),
            }),
        });

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::ENUM),
            HeaderParserValue::Identifier(pos, new std::string("test_namespace")),
            HeaderParserValue::Character(pos, '{'),
            HeaderParserValue::Invalid(pos),
        });
        test.MatchCallback(
            [](sequence_result_t& result)
            {
                FAIL();
            });

        REQUIRE(!test.PerformTest());
    }

    TEST_CASE("Matcher: Ensure can match simple LOOP token sequence", "[parsing][matcher]")
    {
        static constexpr auto TAG_NAMESPACE = 1;
        static constexpr auto TAG_STRUCT = 2;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create.Loop(create.Type(HeaderParserValueType::NAMESPACE).Tag(TAG_NAMESPACE)),
            create.Type(HeaderParserValueType::STRUCT).Tag(TAG_STRUCT),
        });

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Invalid(pos),
        });
        test.MatchCallback(
            [](sequence_result_t& result)
            {
                REQUIRE(result.NextTag() == TAG_NAMESPACE);
                REQUIRE(result.NextTag() == TAG_NAMESPACE);
                REQUIRE(result.NextTag() == TAG_NAMESPACE);
                REQUIRE(result.NextTag() == TAG_STRUCT);
            });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 4);
    }

    TEST_CASE("Matcher: Ensure LOOP token sequence must be called at least once to succeed", "[parsing][matcher]")
    {
        static constexpr auto TAG_NAMESPACE = 1;
        static constexpr auto TAG_STRUCT = 2;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create.Loop(create.Type(HeaderParserValueType::NAMESPACE).Tag(TAG_NAMESPACE)),
            create.Type(HeaderParserValueType::STRUCT).Tag(TAG_STRUCT),
        });

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Invalid(pos),
        });

        REQUIRE(!test.PerformTest());
    }

    TEST_CASE("Matcher: Ensure OPTIONAL_LOOP token sequence can be called zero times", "[parsing][matcher]")
    {
        static constexpr auto TAG_NAMESPACE = 1;
        static constexpr auto TAG_STRUCT = 2;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create.OptionalLoop(create.Type(HeaderParserValueType::NAMESPACE).Tag(TAG_NAMESPACE)),
            create.Type(HeaderParserValueType::STRUCT).Tag(TAG_STRUCT),
        });

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Invalid(pos),
        });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 1);
    }

    TEST_CASE("Matcher: Ensure OPTIONAL token sequence can be called zero times", "[parsing][matcher]")
    {
        static constexpr auto TAG_NAMESPACE = 1;
        static constexpr auto TAG_STRUCT = 2;
        static constexpr auto TAG_ENUM = 3;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create.Type(HeaderParserValueType::NAMESPACE).Tag(TAG_NAMESPACE),
            create.Optional(create.Type(HeaderParserValueType::STRUCT).Tag(TAG_STRUCT)),
            create.Type(HeaderParserValueType::ENUM).Tag(TAG_ENUM),
        });

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::ENUM),
            HeaderParserValue::Invalid(pos),
        });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 2);
    }

    TEST_CASE("Matcher: Ensure OPTIONAL token sequence can be called once", "[parsing][matcher]")
    {
        static constexpr auto TAG_NAMESPACE = 1;
        static constexpr auto TAG_STRUCT = 2;
        static constexpr auto TAG_ENUM = 3;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create.Type(HeaderParserValueType::NAMESPACE).Tag(TAG_NAMESPACE),
            create.Optional(create.Type(HeaderParserValueType::STRUCT).Tag(TAG_STRUCT)),
            create.Type(HeaderParserValueType::ENUM).Tag(TAG_ENUM),
        });

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::ENUM),
            HeaderParserValue::Invalid(pos),
        });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 3);
    }

    TEST_CASE("Matcher: Ensure OPTIONAL token sequence can not be called more than once", "[parsing][matcher]")
    {
        static constexpr auto TAG_NAMESPACE = 1;
        static constexpr auto TAG_STRUCT = 2;
        static constexpr auto TAG_ENUM = 3;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create.Type(HeaderParserValueType::NAMESPACE).Tag(TAG_NAMESPACE),
            create.Optional(create.Type(HeaderParserValueType::STRUCT).Tag(TAG_STRUCT)),
            create.Type(HeaderParserValueType::ENUM).Tag(TAG_ENUM),
        });

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::ENUM),
            HeaderParserValue::Invalid(pos),
        });

        REQUIRE(!test.PerformTest());
    }

    TEST_CASE("Matcher: Ensure LOOP matchers are greedy", "[parsing][matcher]")
    {
        static constexpr auto TAG_NAMESPACE = 1;
        static constexpr auto TAG_STRUCT = 2;
        static constexpr auto TAG_ENUM = 3;
        static constexpr auto TAG_TYPEDEF = 4;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create.Type(HeaderParserValueType::NAMESPACE).Tag(TAG_NAMESPACE),
            create.Loop(create.Or({
                create.Type(HeaderParserValueType::STRUCT).Tag(TAG_STRUCT),
                create.Type(HeaderParserValueType::TYPEDEF).Tag(TAG_TYPEDEF),
            })),
            create.Or({
                create.Type(HeaderParserValueType::TYPEDEF).Tag(TAG_TYPEDEF),
                create.Type(HeaderParserValueType::ENUM).Tag(TAG_ENUM),
            }),
        });

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::TYPEDEF),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::TYPEDEF),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::ENUM),
            HeaderParserValue::Invalid(pos),
        });

        test.MatchCallback(
            [](sequence_result_t& result)
            {
                REQUIRE(result.NextTag() == TAG_NAMESPACE);
                REQUIRE(result.NextTag() == TAG_STRUCT);
                REQUIRE(result.NextTag() == TAG_STRUCT);
                REQUIRE(result.NextTag() == TAG_TYPEDEF);
                REQUIRE(result.NextTag() == TAG_STRUCT);
                REQUIRE(result.NextTag() == TAG_TYPEDEF);
                REQUIRE(result.NextTag() == TAG_ENUM);
            });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 7);
    }

    TEST_CASE("Matcher: Ensure LABEL matcher are working", "[parsing][matcher]")
    {
        static constexpr auto TAG_NAMESPACE = 1;
        static constexpr auto TAG_STRUCT = 2;

        static constexpr auto LABEL_TEST = 1;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create.Type(HeaderParserValueType::NAMESPACE).Tag(TAG_NAMESPACE),
            create.Label(LABEL_TEST),
        });
        test.LabeledMatchers(
            {
                create.Type(HeaderParserValueType::STRUCT).Tag(TAG_STRUCT),
                create.Type(HeaderParserValueType::STRUCT).Tag(TAG_STRUCT),
            },
            LABEL_TEST);

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Invalid(pos),
        });

        test.MatchCallback(
            [](sequence_result_t& result)
            {
                REQUIRE(result.NextTag() == TAG_NAMESPACE);
                REQUIRE(result.NextTag() == TAG_STRUCT);
                REQUIRE(result.NextTag() == TAG_STRUCT);
            });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 3);
    }

    TEST_CASE("Matcher: Ensure LABEL matchers can refer to themselves are working", "[parsing][matcher]")
    {
        static constexpr auto TAG_NAMESPACE = 1;
        static constexpr auto TAG_STRUCT = 2;

        static constexpr auto LABEL_TEST = 1;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create.Type(HeaderParserValueType::NAMESPACE).Tag(TAG_NAMESPACE),
            create.Label(LABEL_TEST),
        });
        test.LabeledMatchers(
            {
                create.Type(HeaderParserValueType::STRUCT).Tag(TAG_STRUCT),
                create.Optional(create.Label(LABEL_TEST)),
            },
            LABEL_TEST);

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Invalid(pos),
        });

        test.MatchCallback(
            [](sequence_result_t& result)
            {
                REQUIRE(result.NextTag() == TAG_NAMESPACE);
                REQUIRE(result.NextTag() == TAG_STRUCT);
                REQUIRE(result.NextTag() == TAG_STRUCT);
                REQUIRE(result.NextTag() == TAG_STRUCT);
            });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 4);
    }

    TEST_CASE("Matcher: Can capture tokens", "[parsing][matcher]")
    {
        static constexpr auto TAG_NAMESPACE = 1;

        static constexpr auto CAPTURE_NAMESPACE_NAME = 1;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create.Type(HeaderParserValueType::NAMESPACE).Tag(TAG_NAMESPACE),
            create.Identifier().Capture(CAPTURE_NAMESPACE_NAME),
            create.Char('{'),
        });

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Identifier(pos, new std::string("hello_world")),
            HeaderParserValue::Character(pos, '{'),
            HeaderParserValue::Invalid(pos),
        });

        test.MatchCallback(
            [](sequence_result_t& result)
            {
                REQUIRE(result.NextTag() == TAG_NAMESPACE);
                REQUIRE(result.NextTag() == matcher_t::NO_ID);

                REQUIRE(result.HasNextCapture(CAPTURE_NAMESPACE_NAME));
                {
                    const auto& capture = result.NextCapture(CAPTURE_NAMESPACE_NAME);
                    REQUIRE(capture.m_type == HeaderParserValueType::IDENTIFIER);
                    REQUIRE(capture.IdentifierValue() == "hello_world");
                }

                REQUIRE(!result.HasNextCapture(CAPTURE_NAMESPACE_NAME));
            });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 3);
    }

    TEST_CASE("Matcher: Can capture in OR matchers", "[parsing][matcher]")
    {
        static constexpr auto TAG_NAMESPACE = 1;
        static constexpr auto TAG_STRUCT = 2;

        static constexpr auto CAPTURE_NAMESPACE_NAME = 1;
        static constexpr auto CAPTURE_STRUCT_NAME = 2;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create.Or({
                create.And({
                    create.Type(HeaderParserValueType::NAMESPACE).Tag(TAG_NAMESPACE),
                    create.Identifier().Capture(CAPTURE_NAMESPACE_NAME),
                }),
                create.And({
                    create.Type(HeaderParserValueType::STRUCT).Tag(TAG_STRUCT),
                    create.Identifier().Capture(CAPTURE_STRUCT_NAME),
                }),
            }),
            create.Char('{'),
        });

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Identifier(pos, new std::string("hello_world")),
            HeaderParserValue::Character(pos, '{'),
            HeaderParserValue::Invalid(pos),
        });
        test.MatchCallback(
            [](sequence_result_t& result)
            {
                REQUIRE(result.NextTag() == TAG_NAMESPACE);
                REQUIRE(result.NextTag() == matcher_t::NO_ID);

                REQUIRE(result.HasNextCapture(CAPTURE_NAMESPACE_NAME));
                {
                    const auto& capture = result.NextCapture(CAPTURE_NAMESPACE_NAME);
                    REQUIRE(capture.m_type == HeaderParserValueType::IDENTIFIER);
                    REQUIRE(capture.IdentifierValue() == "hello_world");
                }

                REQUIRE(!result.HasNextCapture(CAPTURE_NAMESPACE_NAME));
            });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 3);

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Identifier(pos, new std::string("bye_struct")),
            HeaderParserValue::Character(pos, '{'),
            HeaderParserValue::Invalid(pos),
        });
        test.MatchCallback(
            [](sequence_result_t& result)
            {
                REQUIRE(result.NextTag() == TAG_STRUCT);
                REQUIRE(result.NextTag() == matcher_t::NO_ID);

                REQUIRE(result.HasNextCapture(CAPTURE_STRUCT_NAME));
                {
                    const auto& capture = result.NextCapture(CAPTURE_STRUCT_NAME);
                    REQUIRE(capture.m_type == HeaderParserValueType::IDENTIFIER);
                    REQUIRE(capture.IdentifierValue() == "bye_struct");
                }

                REQUIRE(!result.HasNextCapture(CAPTURE_STRUCT_NAME));
            });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 3);
    }

    TEST_CASE("Matcher: Can capture in LOOP matchers", "[parsing][matcher]")
    {
        static constexpr auto TAG_NAMESPACE = 1;

        static constexpr auto CAPTURE_NAMESPACE_NAME = 1;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create.Type(HeaderParserValueType::NAMESPACE).Tag(TAG_NAMESPACE),
            create.Loop(create.Identifier().Capture(CAPTURE_NAMESPACE_NAME)),
            create.Char('{'),
        });

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Identifier(pos, new std::string("hello_world")),
            HeaderParserValue::Identifier(pos, new std::string("hello_universe")),
            HeaderParserValue::Identifier(pos, new std::string("hello_everyone")),
            HeaderParserValue::Character(pos, '{'),
            HeaderParserValue::Invalid(pos),
        });

        test.MatchCallback(
            [](sequence_result_t& result)
            {
                REQUIRE(result.NextTag() == TAG_NAMESPACE);
                REQUIRE(result.NextTag() == matcher_t::NO_ID);

                REQUIRE(result.HasNextCapture(CAPTURE_NAMESPACE_NAME));
                {
                    const auto& capture = result.NextCapture(CAPTURE_NAMESPACE_NAME);
                    REQUIRE(capture.m_type == HeaderParserValueType::IDENTIFIER);
                    REQUIRE(capture.IdentifierValue() == "hello_world");
                }

                REQUIRE(result.HasNextCapture(CAPTURE_NAMESPACE_NAME));
                {
                    const auto& capture = result.NextCapture(CAPTURE_NAMESPACE_NAME);
                    REQUIRE(capture.m_type == HeaderParserValueType::IDENTIFIER);
                    REQUIRE(capture.IdentifierValue() == "hello_universe");
                }

                REQUIRE(result.HasNextCapture(CAPTURE_NAMESPACE_NAME));
                {
                    const auto& capture = result.NextCapture(CAPTURE_NAMESPACE_NAME);
                    REQUIRE(capture.m_type == HeaderParserValueType::IDENTIFIER);
                    REQUIRE(capture.IdentifierValue() == "hello_everyone");
                }

                REQUIRE(!result.HasNextCapture(CAPTURE_NAMESPACE_NAME));
            });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 5);
    }

    TEST_CASE("Matcher: Capturing an AND group captures all matched tokens", "[parsing][matcher]")
    {
        static constexpr auto TAG_AND_GROUP = 1;

        static constexpr auto CAPTURE_AND_GROUP = 1;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create
                .And({
                    create.Type(HeaderParserValueType::NAMESPACE),
                    create.Identifier(),
                })
                .Tag(TAG_AND_GROUP)
                .Capture(CAPTURE_AND_GROUP),
            create.Char('{'),
        });

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Identifier(pos, new std::string("hello_world")),
            HeaderParserValue::Character(pos, '{'),
            HeaderParserValue::Invalid(pos),
        });

        test.MatchCallback(
            [](sequence_result_t& result)
            {
                REQUIRE(result.NextTag() == TAG_AND_GROUP);
                REQUIRE(result.NextTag() == matcher_t::NO_ID);

                REQUIRE(result.HasNextCapture(CAPTURE_AND_GROUP));
                REQUIRE(result.NextCapture(CAPTURE_AND_GROUP).m_type == HeaderParserValueType::NAMESPACE);

                REQUIRE(result.HasNextCapture(CAPTURE_AND_GROUP));
                {
                    const auto& capture = result.NextCapture(CAPTURE_AND_GROUP);
                    REQUIRE(capture.m_type == HeaderParserValueType::IDENTIFIER);
                    REQUIRE(capture.IdentifierValue() == "hello_world");
                }

                REQUIRE(!result.HasNextCapture(CAPTURE_AND_GROUP));
            });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 3);
    }

    TEST_CASE("Matcher: Capturing an LOOP group captures all matched tokens", "[parsing][matcher]")
    {
        static constexpr auto TAG_LOOP_GROUP = 1;

        static constexpr auto CAPTURE_LOOP_GROUP = 1;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create
                .Loop(create.And({
                    create.Type(HeaderParserValueType::NAMESPACE),
                    create.Identifier(),
                }))
                .Tag(TAG_LOOP_GROUP)
                .Capture(CAPTURE_LOOP_GROUP),
            create.Char('{'),
        });

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Identifier(pos, new std::string("hello_world")),
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Identifier(pos, new std::string("hello_universe")),
            HeaderParserValue::Character(pos, '{'),
            HeaderParserValue::Invalid(pos),
        });

        test.MatchCallback(
            [](sequence_result_t& result)
            {
                REQUIRE(result.NextTag() == TAG_LOOP_GROUP);
                REQUIRE(result.NextTag() == matcher_t::NO_ID);

                REQUIRE(result.HasNextCapture(CAPTURE_LOOP_GROUP));
                REQUIRE(result.NextCapture(CAPTURE_LOOP_GROUP).m_type == HeaderParserValueType::NAMESPACE);

                REQUIRE(result.HasNextCapture(CAPTURE_LOOP_GROUP));
                {
                    const auto& capture = result.NextCapture(CAPTURE_LOOP_GROUP);
                    REQUIRE(capture.m_type == HeaderParserValueType::IDENTIFIER);
                    REQUIRE(capture.IdentifierValue() == "hello_world");
                }

                REQUIRE(result.HasNextCapture(CAPTURE_LOOP_GROUP));
                REQUIRE(result.NextCapture(CAPTURE_LOOP_GROUP).m_type == HeaderParserValueType::NAMESPACE);

                REQUIRE(result.HasNextCapture(CAPTURE_LOOP_GROUP));
                {
                    const auto& capture = result.NextCapture(CAPTURE_LOOP_GROUP);
                    REQUIRE(capture.m_type == HeaderParserValueType::IDENTIFIER);
                    REQUIRE(capture.IdentifierValue() == "hello_universe");
                }

                REQUIRE(!result.HasNextCapture(CAPTURE_LOOP_GROUP));
            });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 5);
    }

    TEST_CASE("Matcher: Capture transform works", "[parsing][matcher]")
    {
        static constexpr auto LABEL_TYPENAME = 1;

        static constexpr auto CAPTURE_TYPENAME = 1;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create.Type(HeaderParserValueType::STRUCT),
            create.Label(LABEL_TYPENAME).Capture(CAPTURE_TYPENAME),
            create.Char('{'),
        });
        test.LabeledMatchers(
            {
                create
                    .And({
                        create.Identifier(),
                        create.OptionalLoop(create.And({
                            create.Char(':'),
                            create.Char(':'),
                            create.Identifier(),
                        })),
                    })
                    .Transform(
                        [](HeaderMatcherFactory::token_list_t& values)
                        {
                            std::ostringstream str;
                            str << values[0].get().IdentifierValue();

                            for (auto i = 3u; i < values.size(); i += 3)
                                str << "::" << values[i].get().IdentifierValue();

                            return HeaderParserValue::TypeName(values[0].get().GetPos(), new std::string(str.str()));
                        }),
            },
            LABEL_TYPENAME);

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Identifier(pos, new std::string("hello")),
            HeaderParserValue::Character(pos, ':'),
            HeaderParserValue::Character(pos, ':'),
            HeaderParserValue::Identifier(pos, new std::string("world")),
            HeaderParserValue::Character(pos, '{'),
            HeaderParserValue::Invalid(pos),
        });

        test.MatchCallback(
            [](sequence_result_t& result)
            {
                REQUIRE(result.NextTag() == matcher_t::NO_ID);

                REQUIRE(result.HasNextCapture(CAPTURE_TYPENAME));
                {
                    const auto& capture = result.NextCapture(CAPTURE_TYPENAME);
                    REQUIRE(capture.m_type == HeaderParserValueType::TYPE_NAME);
                    REQUIRE(capture.TypeNameValue() == "hello::world");
                }

                REQUIRE(!result.HasNextCapture(CAPTURE_TYPENAME));
            });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 6);
    }

    TEST_CASE("Matcher: Can capture within transform", "[parsing][matcher]")
    {
        static constexpr auto LABEL_TYPENAME = 1;

        static constexpr auto CAPTURE_TYPENAME = 1;
        static constexpr auto CAPTURE_FIRST_TYPENAME_IDENTIFIER = 2;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create.Type(HeaderParserValueType::STRUCT),
            create.Label(LABEL_TYPENAME).Capture(CAPTURE_TYPENAME),
            create.Char('{'),
        });
        test.LabeledMatchers(
            {
                create
                    .And({
                        create.Identifier().Capture(CAPTURE_FIRST_TYPENAME_IDENTIFIER),
                        create.OptionalLoop(create.And({
                            create.Char(':'),
                            create.Char(':'),
                            create.Identifier(),
                        })),
                    })
                    .Transform(
                        [](HeaderMatcherFactory::token_list_t& values)
                        {
                            std::ostringstream str;
                            str << values[0].get().IdentifierValue();

                            for (auto i = 3u; i < values.size(); i += 3)
                                str << "::" << values[i].get().IdentifierValue();

                            return HeaderParserValue::TypeName(values[0].get().GetPos(), new std::string(str.str()));
                        }),
            },
            LABEL_TYPENAME);

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Identifier(pos, new std::string("hello")),
            HeaderParserValue::Character(pos, ':'),
            HeaderParserValue::Character(pos, ':'),
            HeaderParserValue::Identifier(pos, new std::string("world")),
            HeaderParserValue::Character(pos, '{'),
            HeaderParserValue::Invalid(pos),
        });

        test.MatchCallback(
            [](sequence_result_t& result)
            {
                REQUIRE(result.NextTag() == matcher_t::NO_ID);

                REQUIRE(result.HasNextCapture(CAPTURE_TYPENAME));
                {
                    const auto& capture = result.NextCapture(CAPTURE_TYPENAME);
                    REQUIRE(capture.m_type == HeaderParserValueType::TYPE_NAME);
                    REQUIRE(capture.TypeNameValue() == "hello::world");
                }

                REQUIRE(!result.HasNextCapture(CAPTURE_TYPENAME));

                REQUIRE(result.HasNextCapture(CAPTURE_FIRST_TYPENAME_IDENTIFIER));
                {
                    const auto& capture = result.NextCapture(CAPTURE_FIRST_TYPENAME_IDENTIFIER);
                    REQUIRE(capture.m_type == HeaderParserValueType::IDENTIFIER);
                    REQUIRE(capture.IdentifierValue() == "hello");
                }

                REQUIRE(!result.HasNextCapture(CAPTURE_FIRST_TYPENAME_IDENTIFIER));
            });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 6);
    }

    TEST_CASE("Matcher: Can transform and capture in the same matcher", "[parsing][matcher]")
    {
        static constexpr auto CAPTURE_NAME = 1;

        MatchersTestsHelper test;
        const TokenPos pos;
        const auto create = test.Factory();
        test.Matchers({
            create.Type(HeaderParserValueType::STRUCT),
            create.Identifier()
                .Capture(CAPTURE_NAME)
                .Transform(
                    [](HeaderMatcherFactory::token_list_t& tokens)
                    {
                        auto str = tokens[0].get().IdentifierValue();
                        std::ranges::transform(str, str.begin(), toupper);
                        return HeaderParserValue::Identifier(tokens[0].get().GetPos(), new std::string(std::move(str)));
                    }),
            create.Char('{'),
        });

        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::STRUCT),
            HeaderParserValue::Identifier(pos, new std::string("hello_world")),
            HeaderParserValue::Character(pos, '{'),
            HeaderParserValue::Invalid(pos),
        });

        test.MatchCallback(
            [](sequence_result_t& result)
            {
                REQUIRE(result.NextTag() == matcher_t::NO_ID);

                REQUIRE(result.HasNextCapture(CAPTURE_NAME));
                {
                    const auto& capture = result.NextCapture(CAPTURE_NAME);
                    REQUIRE(capture.m_type == HeaderParserValueType::IDENTIFIER);
                    REQUIRE(capture.IdentifierValue() == "HELLO_WORLD");
                }

                REQUIRE(!result.HasNextCapture(CAPTURE_NAME));
            });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 3);
    }

    TEST_CASE("Matcher: Ensure noconsume does not consume a token", "[parsing][matcher]")
    {
        MatchersTestsHelper test;
        const TokenPos pos;
        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Identifier(pos, new std::string("test_namespace")),
            HeaderParserValue::Character(pos, '{'),
            HeaderParserValue::Invalid(pos),
        });
        const auto create = test.Factory();
        test.Matchers({
            create.Type(HeaderParserValueType::NAMESPACE),
            create.Identifier(),
            create.Char('{').NoConsume(),
        });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 2);
    }

    TEST_CASE("Matcher: Ensure noconsume can be captured", "[parsing][matcher]")
    {
        static constexpr auto CAPTURE_NAME = 1;

        MatchersTestsHelper test;
        const TokenPos pos;
        test.Tokens({
            HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
            HeaderParserValue::Identifier(pos, new std::string("test_namespace")),
            HeaderParserValue::Character(pos, '{'),
            HeaderParserValue::Invalid(pos),
        });
        const auto create = test.Factory();
        test.Matchers({
            create.Type(HeaderParserValueType::NAMESPACE),
            create.Identifier().NoConsume().Capture(CAPTURE_NAME),
        });
        test.MatchCallback(
            [](sequence_result_t& result)
            {
                REQUIRE(result.HasNextCapture(CAPTURE_NAME));
                {
                    const auto& capture = result.NextCapture(CAPTURE_NAME);
                    REQUIRE(capture.m_type == HeaderParserValueType::IDENTIFIER);
                    REQUIRE(capture.IdentifierValue() == "test_namespace");
                }
            });

        REQUIRE(test.PerformTest());
        REQUIRE(test.GetConsumedTokenCount() == 1);
    }
} // namespace test::parsing::matcher
