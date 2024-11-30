#pragma once

namespace FMOD::Studio
{
	class System;
}

class AudioManager
{
public:
	static bool Init();
	static void Shutdown();

private:
	static FMOD::Studio::System* m_FMODSystem;
};
