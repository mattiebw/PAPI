﻿#pragma once

#include "Texture.h"

namespace msdfgen
{
	class FreetypeHandle;
}

struct MSDFData;

class Font
{
public:
	Font(const std::filesystem::path &fontPath);
	~Font();

	static void InitFontSystem();
	static void ShutdownFontSystem();

	NODISCARD FORCEINLINE int                 GetGlyphCount() const { return m_GlyphCount; }
	NODISCARD FORCEINLINE const Ref<Texture>& GetAtlasTexture() const { return m_Texture; }
	NODISCARD FORCEINLINE const MSDFData*     GetData() const { return m_Data.get(); }

	NODISCARD static FORCEINLINE const Ref<Font>& GetDefaultFont() { return s_DefaultFont; }

private:
	Ref<Texture>    m_Texture;
	Scope<MSDFData> m_Data;
	int             m_GlyphCount;

	static Ref<Font>                s_DefaultFont;
	static msdfgen::FreetypeHandle *s_FTHandle;
};
