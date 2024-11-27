#pragma once

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

private:
	static Ref<Font> s_DefaultFont;
	static msdfgen::FreetypeHandle* s_FTHandle;
};
