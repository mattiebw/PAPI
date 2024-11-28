#pragma once
#include "Texture.h"

namespace msdf_atlas
{
	class GlyphGeometry;
}

namespace msdfgen
{
	class FreetypeHandle;
}

class Font
{
public:
	Font(const std::filesystem::path& fontPath);
	~Font();

	static void InitFontSystem();
	static void ShutdownFontSystem();

	NODISCARD FORCEINLINE int GetGlyphCount() const { return m_GlyphCount; }
	NODISCARD FORCEINLINE const Ref<Texture>& GetTexture() const { return m_Texture; }
	
	NODISCARD static FORCEINLINE const Ref<Font>& GetDefaultFont() { return s_DefaultFont; }
	
private:
	Ref<Texture> m_Texture;
	std::unique_ptr<struct MSDFData> m_Data;
	int m_GlyphCount;
	
	static Ref<Font> s_DefaultFont;
	static msdfgen::FreetypeHandle* s_FTHandle;
};
