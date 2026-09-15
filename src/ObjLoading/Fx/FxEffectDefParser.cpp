#include "FxEffectDefParser.h"

#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/ParserSingleInputStream.h"
#include "Parsing/ParsingException.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Utils/Logging/Log.h"

#include <algorithm>
#include <cmath>
#include <format>
#include <limits>
#include <string_view>
#include <type_traits>
#include <utility>

namespace fx
{
    size_t FxEditorCurve::KeyCount() const
    {
        return dimensionCount == 0u ? 0u : keys.size() / (dimensionCount + 1u);
    }

    float FxEditorCurve::Sample(const float time, const size_t dimension) const
    {
        const auto keyCount = KeyCount();
        if (keyCount < 2u || dimension >= dimensionCount)
            return 0.0f;

        const auto keySize = dimensionCount + 1u;
        auto nextKey = 1u;
        while (nextKey + 1u < keyCount && keys[nextKey * keySize] < time)
            nextKey++;

        const auto previousKey = nextKey - 1u;
        const auto previousTime = keys[previousKey * keySize];
        const auto nextTime = keys[nextKey * keySize];
        const auto fraction = (time - previousTime) / (nextTime - previousTime);
        const auto previousValue = keys[previousKey * keySize + dimension + 1u];
        const auto nextValue = keys[nextKey * keySize + dimension + 1u];
        return previousValue + (nextValue - previousValue) * fraction;
    }

    namespace
    {
        constexpr auto MAX_ELEMENT_COUNT = 32u;
        constexpr auto MAX_VISUAL_COUNT = 32u;
        constexpr auto MAX_DECAL_VISUAL_COUNT = 16u;
        constexpr auto MAX_TRAIL_VERTEX_COUNT = 64u;
        constexpr auto MAX_TRAIL_INDEX_COUNT = 128u;
        constexpr auto MAX_CURVE_VALUE_COUNT = 512u;

        class EffectDefParser
        {
        public:
            EffectDefParser(ILexer<SimpleParserValue>& lexer, const FxEffectDefParserConfig& config, FxEditorEffect& effect)
                : m_lexer(lexer),
                  m_config(config),
                  m_effect(effect)
            {
            }

            void Parse()
            {
                ExpectIdentifier("iwfx");
                m_effect.version = ReadInteger();
                if (m_effect.version > m_config.maxVersion)
                    Throw(std::format("Version {} is too high; expected at most {}", m_effect.version, m_config.maxVersion));

                while (!IsEof() && !IsCharacter('{'))
                    ParseEffectField(ReadIdentifier());

                while (!IsEof())
                {
                    if (m_effect.elements.size() >= MAX_ELEMENT_COUNT)
                        Throw(std::format("Cannot have more than {} segments", MAX_ELEMENT_COUNT));

                    ExpectCharacter('{');
                    ParseElement(m_effect.elements.emplace_back());
                }
            }

        private:
            [[nodiscard]] const SimpleParserValue& Token() const
            {
                return m_lexer.GetToken(0u);
            }

            [[noreturn]] void Throw(std::string message) const
            {
                throw ParsingException(Token().GetPos(), std::move(message));
            }

            void Pop() const
            {
                m_lexer.PopTokens(1u);
            }

            [[nodiscard]] bool IsEof() const
            {
                return Token().m_type == SimpleParserValueType::END_OF_FILE;
            }

            [[nodiscard]] bool IsCharacter(const char value) const
            {
                return Token().m_type == SimpleParserValueType::CHARACTER && Token().CharacterValue() == value;
            }

            void ExpectCharacter(const char value) const
            {
                if (!IsCharacter(value))
                    Throw(std::format("Expected '{}'", value));
                Pop();
            }

            std::string ReadIdentifier() const
            {
                if (Token().m_type != SimpleParserValueType::IDENTIFIER)
                    Throw("Expected identifier");

                auto result = Token().IdentifierValue();
                Pop();
                return result;
            }

            void ExpectIdentifier(const std::string_view value) const
            {
                if (Token().m_type != SimpleParserValueType::IDENTIFIER || Token().IdentifierValue() != value)
                    Throw(std::format("Expected '{}'", value));
                Pop();
            }

            int ReadInteger() const
            {
                if (Token().m_type != SimpleParserValueType::INTEGER)
                    Throw("Expected integer");

                const auto result = Token().IntegerValue();
                Pop();
                return result;
            }

            float ReadFloat() const
            {
                float result;
                if (Token().m_type == SimpleParserValueType::INTEGER)
                    result = static_cast<float>(Token().IntegerValue());
                else if (Token().m_type == SimpleParserValueType::FLOATING_POINT)
                    result = static_cast<float>(Token().FloatingPointValue());
                else
                    Throw("Expected number");

                Pop();
                return result;
            }

            std::string ReadString() const
            {
                if (Token().m_type != SimpleParserValueType::STRING)
                    Throw("Expected quoted string");

                auto result = Token().StringValue();
                Pop();
                return result;
            }

            template<typename T> void ReadRange(FxEditorRange<T>& range) const
            {
                if constexpr (std::is_same_v<T, float>)
                {
                    range.base = ReadFloat();
                    range.amplitude = ReadFloat();
                }
                else
                {
                    range.base = ReadInteger();
                    range.amplitude = ReadInteger();
                }
            }

            template<size_t Size> void ReadVector(std::array<float, Size>& value) const
            {
                for (auto& component : value)
                    component = ReadFloat();
            }

            void ReadFlags(unsigned& editorFlags, unsigned& flags, unsigned& atlasFlags, unsigned& effectFlags, const FxFlagType allowedType) const
            {
                while (!IsCharacter(';'))
                {
                    const auto flagName = ReadIdentifier();
                    const auto flagDef = std::ranges::find_if(m_config.flagDefs,
                                                              [&flagName, allowedType](const FxFlagDef& value)
                                                              {
                                                                  return value.type == allowedType && value.name == flagName;
                                                              });
                    if (flagDef == m_config.flagDefs.end())
                        Throw(std::format("Unknown flag '{}'", flagName));

                    unsigned* output = nullptr;
                    switch (flagDef->type)
                    {
                    case FxFlagType::EDITOR:
                        output = &editorFlags;
                        break;
                    case FxFlagType::NATIVE:
                        output = &flags;
                        break;
                    case FxFlagType::ATLAS:
                        output = &atlasFlags;
                        break;
                    case FxFlagType::EFFECT:
                        output = &effectFlags;
                        break;
                    }

                    *output &= ~flagDef->mask;
                    *output |= flagDef->value;
                }
            }

            void ReadElementFlags(FxEditorElem& element) const
            {
                while (!IsCharacter(';'))
                {
                    const auto flagName = ReadIdentifier();
                    const auto flagDef =
                        std::ranges::find_if(m_config.flagDefs,
                                             [&flagName](const FxFlagDef& value)
                                             {
                                                 return (value.type == FxFlagType::EDITOR || value.type == FxFlagType::NATIVE) && value.name == flagName;
                                             });
                    if (flagDef == m_config.flagDefs.end())
                        Throw(std::format("Unknown element flag '{}'", flagName));

                    auto& output = flagDef->type == FxFlagType::EDITOR ? element.editorFlags : element.flags;
                    output &= ~flagDef->mask;
                    output |= flagDef->value;
                }
            }

            FxEditorCurve ReadCurve(const size_t dimensionCount, const float minValue, const float maxValue) const
            {
                ExpectCharacter('{');
                FxEditorCurve result;
                result.dimensionCount = dimensionCount;
                const auto keySize = dimensionCount + 1u;

                while (!IsCharacter('}'))
                {
                    if (result.keys.size() + keySize > MAX_CURVE_VALUE_COUNT)
                        Throw(std::format("{}-dimensional curve exceeds {} values", dimensionCount, MAX_CURVE_VALUE_COUNT));

                    result.keys.emplace_back(ReadFloat());
                    for (auto dimension = 0u; dimension < dimensionCount; dimension++)
                        result.keys.emplace_back(std::clamp(ReadFloat(), minValue, maxValue));
                }
                ExpectCharacter('}');

                const auto keyCount = result.KeyCount();
                if (keyCount < 2u)
                    Throw("Curves must have at least two keys");
                if (result.keys.front() != 0.0f || result.keys[(keyCount - 1u) * keySize] != 1.0f)
                    Throw("Curves must start at time 0 and end at time 1");
                for (auto keyIndex = 1u; keyIndex < keyCount; keyIndex++)
                {
                    if (result.keys[(keyIndex - 1u) * keySize] >= result.keys[keyIndex * keySize])
                        Throw("Curve times must be monotonically increasing");
                }

                return result;
            }

            void ReadGraph(FxEditorGraph& graph, const size_t dimensionCount, const float minValue, const float maxValue) const
            {
                graph.scale = ReadFloat();
                ExpectCharacter('{');
                graph.curves[0] = ReadCurve(dimensionCount, minValue, maxValue);
                graph.curves[1] = ReadCurve(dimensionCount, minValue, maxValue);
                ExpectCharacter('}');
            }

            void SetVisualType(FxEditorElem& element, const FxEditorElemType type) const
            {
                if (element.type != FxEditorElemType::NONE)
                    Throw("More than one visual type is present in effect element");
                element.type = type;
            }

            void ReadVisuals(FxEditorElem& element, const FxEditorElemType type, const size_t maxCount) const
            {
                SetVisualType(element, type);
                ExpectCharacter('{');
                while (!IsCharacter('}'))
                {
                    if (element.visuals.size() >= maxCount)
                        Throw(std::format("More than {} visuals in array", maxCount));
                    element.visuals.emplace_back(ReadString());
                }
                ExpectCharacter('}');
            }

            void ReadTrail(FxEditorElem& element) const
            {
                ExpectCharacter('{');
                while (!IsCharacter('}'))
                {
                    if (element.trailVertices.size() >= MAX_TRAIL_VERTEX_COUNT)
                        Throw(std::format("More than {} trail vertices", MAX_TRAIL_VERTEX_COUNT));

                    auto& vertex = element.trailVertices.emplace_back();
                    vertex.pos[0] = ReadFloat();
                    vertex.pos[1] = ReadFloat();
                    vertex.texCoord = ReadFloat();
                }
                ExpectCharacter('}');

                ExpectCharacter('{');
                while (!IsCharacter('}'))
                {
                    if (element.trailIndices.size() >= MAX_TRAIL_INDEX_COUNT)
                        Throw(std::format("More than {} trail indices", MAX_TRAIL_INDEX_COUNT));

                    const auto index = ReadInteger();
                    if (index < 0 || index > std::numeric_limits<std::uint16_t>::max())
                        Throw(std::format("Trail index {} is outside the uint16 range", index));
                    element.trailIndices.emplace_back(static_cast<std::uint16_t>(index));
                }
                ExpectCharacter('}');
            }

            void ReadSpawnSound(FxEditorElem& element) const
            {
                ExpectCharacter('{');
                if (!IsCharacter('}'))
                    element.spawnSound = ReadString();
                if (!IsCharacter('}'))
                    Throw("Only one element spawn sound is allowed");
                ExpectCharacter('}');
            }

            void ParseEffectField(const std::string& field)
            {
                if (!m_config.supportsEffectFields)
                    Throw(std::format("Unknown effect field '{}'", field));

                unsigned unusedEditorFlags = 0u;
                unsigned unusedFlags = 0u;
                unsigned unusedAtlasFlags = 0u;
                if (field == "efBoundingBoxDim")
                    ReadVector(m_effect.boundingBoxDim);
                else if (field == "efBoundingBoxCentre")
                    ReadVector(m_effect.boundingBoxCentre);
                else if (field == "efFlags")
                    ReadFlags(unusedEditorFlags, unusedFlags, unusedAtlasFlags, m_effect.flags, FxFlagType::EFFECT);
                else if (field == "efPriority")
                    m_effect.priority = ReadInteger();
                else
                    Throw(std::format("Unknown effect field '{}'", field));

                ExpectCharacter(';');
            }

            void ParseElement(FxEditorElem& element)
            {
                if (m_effect.version < 2)
                    element.editorFlags = m_config.backCompatVelocityFlag;

                while (!IsCharacter('}'))
                {
                    if (IsEof())
                        Throw("Unexpected end of file in effect element");

                    const auto field = ReadIdentifier();
                    ParseElementField(field, element);
                    ExpectCharacter(';');
                }
                ExpectCharacter('}');

                if (element.type == FxEditorElemType::NONE)
                    Throw("No visual type specified");
                if (element.type == FxEditorElemType::TRAIL)
                    element.flags &= ~m_config.runMask;
            }

            void ParseElementField(const std::string& field, FxEditorElem& element)
            {
                if (field == "name")
                    element.name = ReadString();
                else if (field == "editorFlags" || field == "flags")
                    ReadElementFlags(element);
                else if (field == "spawnRange")
                    ReadRange(element.spawnRange);
                else if (field == "fadeInRange")
                    ReadRange(element.fadeInRange);
                else if (field == "fadeOutRange")
                    ReadRange(element.fadeOutRange);
                else if (field == "spawnFrustumCullRadius")
                    element.spawnFrustumCullRadius = ReadFloat();
                else if (field == "spawnLooping")
                {
                    element.spawnLoopingIntervalMsec = ReadInteger();
                    element.spawnLoopingCount = ReadInteger();
                }
                else if (field == "spawnOneShot")
                    ReadRange(element.spawnOneShotCount);
                else if (field == "spawnDelayMsec")
                    ReadRange(element.spawnDelayMsec);
                else if (field == "lifeSpanMsec")
                    ReadRange(element.lifeSpanMsec);
                else if (field == "spawnOrgX" || field == "spawnOrgY" || field == "spawnOrgZ")
                    ReadRange(element.spawnOrigin[field.back() - 'X']);
                else if (field == "spawnOffsetRadius")
                    ReadRange(element.spawnOffsetRadius);
                else if (field == "spawnOffsetHeight")
                    ReadRange(element.spawnOffsetHeight);
                else if (field == "spawnAnglePitch")
                    ReadRange(element.spawnAngles[0]);
                else if (field == "spawnAngleYaw")
                    ReadRange(element.spawnAngles[1]);
                else if (field == "spawnAngleRoll")
                    ReadRange(element.spawnAngles[2]);
                else if (field == "angleVelPitch")
                    ReadRange(element.angularVelocity[0]);
                else if (field == "angleVelYaw")
                    ReadRange(element.angularVelocity[1]);
                else if (field == "angleVelRoll")
                    ReadRange(element.angularVelocity[2]);
                else if (field == "initialRot")
                    ReadRange(element.initialRotation);
                else if (field == "rotationAxis" && m_config.supportsT5Fields)
                    ReadVector(element.rotationAxis);
                else if (field == "gravity")
                    ReadRange(element.gravity);
                else if (field == "elasticity")
                    ReadRange(element.elasticity);
                else if (field == "windinfluence" && m_config.supportsWind)
                    element.windInfluence = ReadFloat();
                else if (field == "atlasBehavior")
                {
                    unsigned unusedEditorFlags = 0u;
                    unsigned unusedFlags = 0u;
                    unsigned unusedEffectFlags = 0u;
                    ReadFlags(unusedEditorFlags, unusedFlags, element.atlasBehavior, unusedEffectFlags, FxFlagType::ATLAS);
                }
                else if (field == "atlasIndex")
                    element.atlasIndex = ReadInteger();
                else if (field == "atlasFps")
                    element.atlasFps = ReadInteger();
                else if (field == "atlasLoopCount")
                    element.atlasLoopCount = ReadInteger();
                else if (field == "atlasColIndexBits")
                    element.atlasColIndexBits = ReadInteger();
                else if (field == "atlasRowIndexBits")
                    element.atlasRowIndexBits = ReadInteger();
                else if (field == "atlasEntryCount")
                    element.atlasEntryCount = ReadInteger();
                else if (field == "atlasIndexRange" && m_config.supportsT5Fields)
                    element.atlasIndexRange = ReadInteger();
                else if (field.starts_with("velGraph") && field.size() == 10u && (field[8] == '0' || field[8] == '1') && field[9] >= 'X' && field[9] <= 'Z')
                    ReadGraph(element.velocityGraphs[field[8] - '0'][field[9] - 'X'], 1u, -0.5f, 0.5f);
                else if (field == "rotGraph")
                    ReadGraph(element.rotationGraph, 1u, -0.5f, 0.5f);
                else if (field == "sizeGraph0" || field == "sizeGraph1")
                    ReadGraph(element.sizeGraphs[field.back() - '0'], 1u, 0.0f, 1.0f);
                else if (field == "scaleGraph")
                    ReadGraph(element.scaleGraph, 1u, 0.0f, 1.0f);
                else if (field == "colorGraph")
                    ReadGraph(element.colorGraph, 3u, 0.0f, 1.0f);
                else if (field == "alphaGraph")
                    ReadGraph(element.alphaGraph, 1u, 0.0f, 1.0f);
                else if (field == "lightingFrac")
                    element.lightingFrac = ReadFloat();
                else if (field == "collOffset")
                    ReadVector(element.collOffset);
                else if (field == "collRadius")
                    element.collRadius = ReadFloat();
                else if (field == "fxOnImpact")
                    element.effectOnImpact = ReadString();
                else if (field == "fxOnDeath")
                    element.effectOnDeath = ReadString();
                else if (field == "sortOrder")
                    element.sortOrder = ReadInteger();
                else if (field == "efPriority" && m_config.supportsWind)
                {
                    const auto priority = ReadInteger();
                    if (!m_config.supportsEffectFields || m_effect.version < 3)
                        m_effect.priority = priority;
                }
                else if (field == "emission")
                    element.emission = ReadString();
                else if (field == "emitDist")
                    ReadRange(element.emitDist);
                else if (field == "emitDistVariance")
                    ReadRange(element.emitDistVariance);
                else if (field == "attachment" && m_config.supportsT5Fields)
                    element.attachment = ReadString();
                else if (field == "trailRepeatTime")
                    ReadFloat();
                else if (field == "trailSplitDist")
                    element.trailSplitDist = ReadInteger();
                else if (field == "trailScrollTime")
                    element.trailScrollTime = ReadFloat();
                else if (field == "trailRepeatDist")
                    element.trailRepeatDist = ReadInteger();
                else if (field == "trailDef")
                    ReadTrail(element);
                else if (field == "billboardSprite")
                    ReadVisuals(element, FxEditorElemType::BILLBOARD_SPRITE, MAX_VISUAL_COUNT);
                else if (field == "orientedSprite")
                    ReadVisuals(element, FxEditorElemType::ORIENTED_SPRITE, MAX_VISUAL_COUNT);
                else if (field == "rotatedSprite" && m_config.supportsT5Fields)
                    ReadVisuals(element, FxEditorElemType::ROTATED_SPRITE, MAX_VISUAL_COUNT);
                else if (field == "tail")
                    ReadVisuals(element, FxEditorElemType::TAIL, MAX_VISUAL_COUNT);
                else if (field == "line" && m_config.supportsLine)
                    ReadVisuals(element, FxEditorElemType::LINE, MAX_VISUAL_COUNT);
                else if (field == "trail")
                    ReadVisuals(element, FxEditorElemType::TRAIL, MAX_VISUAL_COUNT);
                else if (field == "cloud")
                    ReadVisuals(element, FxEditorElemType::CLOUD, MAX_VISUAL_COUNT);
                else if (field == "model")
                    ReadVisuals(element, FxEditorElemType::MODEL, MAX_VISUAL_COUNT);
                else if (field == "light")
                    SetVisualType(element, FxEditorElemType::LIGHT);
                else if (field == "spotLight")
                    SetVisualType(element, FxEditorElemType::SPOT_LIGHT);
                else if (field == "sound")
                    ReadVisuals(element, FxEditorElemType::SOUND, MAX_VISUAL_COUNT);
                else if (field == "decal")
                    ReadVisuals(element, FxEditorElemType::DECAL, MAX_DECAL_VISUAL_COUNT);
                else if (field == "runner")
                    ReadVisuals(element, FxEditorElemType::RUNNER, MAX_VISUAL_COUNT);
                else if (field == "alphafadetimemsec" && m_config.supportsT5Fields)
                    element.alphaFadeTimeMsec = ReadInteger();
                else if (field == "maxwind_mag" && m_config.supportsT5Fields)
                    element.maxWindMagnitude = ReadInteger();
                else if (field == "maxwind_life" && m_config.supportsT5Fields)
                    element.maxWindLife = ReadInteger();
                else if (field == "maxwind_interval" && m_config.supportsT5Fields)
                    element.maxWindInterval = ReadInteger();
                else if (field == "billboardTopWidth" && m_config.supportsT5Fields)
                    element.billboardTopWidth = ReadFloat();
                else if (field == "billboardBottomWidth" && m_config.supportsT5Fields)
                    element.billboardBottomWidth = ReadFloat();
                else if (field == "elemSpawnSound" && m_config.supportsT5Fields)
                    ReadSpawnSound(element);
                else if (field == "cloudDensity" && m_config.supportsT5Fields)
                    ReadRange(element.cloudDensity);
                else if (field == "billboardPivot" && m_config.supportsT5Fields)
                    ReadVector(element.billboardPivot);
                else
                    Throw(std::format("Unknown effect element field '{}'", field));
            }

            ILexer<SimpleParserValue>& m_lexer;
            const FxEffectDefParserConfig& m_config;
            FxEditorEffect& m_effect;
        };
    } // namespace

    bool ParseEffectDef(std::istream& stream, const std::string& fileName, const FxEffectDefParserConfig& config, FxEditorEffect& effect)
    {
        ParserSingleInputStream parserStream(stream, fileName);
        CommentRemovingStreamProxy commentRemovingStream(&parserStream);
        SimpleLexer::Config lexerConfig;
        lexerConfig.m_string_escape_sequences = true;
        SimpleLexer lexer(&commentRemovingStream, std::move(lexerConfig));

        try
        {
            EffectDefParser parser(lexer, config, effect);
            parser.Parse();
            return true;
        }
        catch (const ParsingException& e)
        {
            const auto pos = e.Position();
            const auto line = lexer.GetLineForPos(pos);
            if (!line.IsEof() && line.m_line.size() >= pos.m_column)
                con::error("{}\n{}", e.FullMessage(), line.m_line.substr(pos.m_column - 1u));
            else
                con::error(e.FullMessage());
            return false;
        }
    }
} // namespace fx
