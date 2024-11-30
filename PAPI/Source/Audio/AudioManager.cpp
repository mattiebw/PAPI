#include "papipch.h"
#include "Audio/AudioManager.h"

#include <fmod_studio.hpp>
#include <fmod_errors.h>

#include "Core/Application.h"

FMOD::Studio::System *AudioManager::m_FMODSystem = nullptr;

bool AudioManager::Init()
{
	PAPI_TRACE("Initialising AudioManager and FMOD.");
	Stopwatch sw;

	FMOD_RESULT result = FMOD::Studio::System::create(&m_FMODSystem);
	if (result != FMOD_OK)
	{
		std::string error = fmt::format("Failed to create FMOD system! Error: {}", FMOD_ErrorString(result));
		PAPI_ERROR("{}", error);
		Application::Get()->ShowError(error.c_str(), "FMOD Error");
		return false;
	}

	FMOD_RESULT initResult = m_FMODSystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
	if (initResult != FMOD_OK)
	{
		std::string error = fmt::format("Failed to initialise FMOD system! Error: {}", FMOD_ErrorString(initResult));
		PAPI_ERROR("{}", error);
		Application::Get()->ShowError(error.c_str(), "FMOD Error");
		return false;
	}

	sw.End();
	PAPI_TRACE("Initialised AudioManager/FMOD in {0}ms", sw.GetElapsedMilliseconds());
	return true;
}

void AudioManager::Shutdown()
{
	PAPI_TRACE("Shutting down AudioManager and FMOD.");
	
	if (m_FMODSystem)
	{
		m_FMODSystem->release();
		m_FMODSystem = nullptr;
	}
}
