#pragma once

namespace FMOD::Studio
{
	class System;
	class EventInstance;
}

struct SoundHandle
{
public:
	SoundHandle(FMOD::Studio::EventInstance* instance)
		: m_Instance(instance)
	{}
	
	void Stop();
	
private:
	FMOD::Studio::EventInstance* m_Instance;
};

class AudioManager
{
public:
	static bool Init();
	static void Update();
	static void Shutdown();
	static void PlayBackgroundMusic();

	// TODO: Fade out and in background music, keep records of previous music?
	// PlaySound(string name)
	// return a handle to the sound, so you can pause it, set volume.
	static SoundHandle PlaySound(const char* soundName);

private:
	static FMOD::Studio::System* m_FMODSystem;
	static FMOD::Studio::EventInstance* m_BackgroundMusicInstance;

	static bool LoadBank(const std::string& bankName);
};
