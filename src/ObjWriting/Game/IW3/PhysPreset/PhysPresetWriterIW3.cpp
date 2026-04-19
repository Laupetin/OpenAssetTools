#include "FontWriterIW3.h"

//#include "Game/IW3/FontConstantsIW3.h"
#include "Font/AbstractFontWriter.h"
#include "ObjWriting.h"

#include <cassert>
#include <cmath>
#include <sstream>

using namespace IW3;

namespace
{
    class FontWriter final : public ::font::AbstractBaseWriter, public font::IWriterIW3
    {
    public:
        explicit FontWriter(std::ostream& stream)
            : AbstractBaseWriter(stream)
        {
        }

        void WriteFont(const Font_s& font) override
        {
            size_t size = 16 + 24 * font.glyphCount;
            WriteRawU32(size);
            WriteRawU32(font.pixelHeight);
            WriteRawU32(font.glyphCount);

            size_t adjustedSize = size + strlen(font.fontName) + 1;
            if (font.material && font.material->info.name)
            {
                if (strstr(font.material->info.name, ","))
                {
                    adjustedSize += strlen(&font.material->info.name[1]) + 1;
                }
                else
                {
                    adjustedSize += strlen(font.material->info.name) + 1;
                }
            }
            adjustedSize -= 15;
            WriteRawU32(adjustedSize);

            for (int i = 0; i < font.glyphCount; i++)
            {
                Glyph glyph = font.glyphs[i];

                WriteRawU16(glyph.letter);
                WriteRawU8(glyph.x0);
                WriteRawU8(glyph.y0);
                WriteRawU8(glyph.dx);
                WriteRawU8(glyph.pixelWidth);
                WriteRawU8(glyph.pixelHeight);
                WriteRawU8(0);
                WriteRawSingle(glyph.s0);
                WriteRawSingle(glyph.t0);
                WriteRawSingle(glyph.s1);
                WriteRawSingle(glyph.t1);
            }

            WriteCString(font.fontName);
            if (font.material && font.material->info.name)
            {
                if (strstr(font.material->info.name, ","))
                {
                    WriteCString(&font.material->info.name[1]);
                }
                else
                {
                    WriteCString(font.material->info.name);
                }
            }

            return;
        }

    private:
    };
} // namespace

namespace font
{
    std::unique_ptr<IWriterIW3> CreateFontWriterIW3(std::ostream& stream)
    {
        return std::make_unique<FontWriter>(stream);
    }
} // namespace menu
