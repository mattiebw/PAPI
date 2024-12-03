#pragma once

namespace FMOD::Studio
{
	class System;
	class EventInstance;
}

class AudioManager
{
public:
	static bool Init();
	static void Update();
	static void Shutdown();
	static void PlayBackgroundMusic();

private:
	static FMOD::Studio::System* m_FMODSystem;
	static FMOD::Studio::EventInstance* m_BackgroundMusicInstance;

	static bool LoadBank(const std::string& bankPath);
};
