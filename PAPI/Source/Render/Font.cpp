#include "papipch.h"
#include "Render/Font.h"

#include <msdf-atlas-gen.h>
#include <msdfgen.h>

msdfgen::FreetypeHandle* Font::s_FTHandle = nullptr;
Ref<Font> Font::s_DefaultFont = nullptr;

Font::Font(const std::filesystem::path &fontPath)
{
	PAPI_ASSERT(s_FTHandle && "Font system not initialized!");

	Stopwatch sw;
	std::string pathString = fontPath.string(); // Convert from path->string, as we need a c_str, but not wide!
	msdfgen::FontHandle* font = loadFont(s_FTHandle, pathString.c_str());

	if (!font)
	{
		PAPI_ERROR("Failed to load font from path: {0}", pathString);
		return;
	}

	sw.End();
	PAPI_TRACE("Loaded font from file \"{0}\" in {1}ms", pathString, sw.GetElapsedMilliseconds());
}

Font::~Font()
{
}

void Font::InitFontSystem()
{
	s_FTHandle = msdfgen::initializeFreetype();
	if (!s_FTHandle)
	{
		PAPI_ERROR("Failed to initialize FreeType!");
		return;
	}

	s_DefaultFont = CreateRef<Font>("Content/Fonts/OpenSans-Regular.ttf");
}

void Font::ShutdownFontSystem()
{
	deinitializeFreetype(s_FTHandle);
}
