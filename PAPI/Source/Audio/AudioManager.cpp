#include "papipch.h"
#include "Audio/AudioManager.h"

#include <fmod_studio.hpp>
#include <fmod_errors.h>

#include "Core/Application.h"

FMOD::Studio::System* AudioManager::m_FMODSystem = nullptr;
FMOD::Studio::EventInstance* AudioManager::m_BackgroundMusicInstance = nullptr;

std::vector<std::string> AudioManager::m_BackgroundMusicPaths = {
	"event:/Music/BackgroundMusic1",
	"event:/Music/BackgroundMusic2",
	"event:/Music/BackgroundMusic3"
};

std::vector<int> AudioManager::m_TrackHistory = {};

bool AudioManager::m_FadingOut = false;
bool AudioManager::m_FadingIn = false;
float AudioManager::m_CurrentVolume = 1.0f;
float AudioManager::m_TargetVolume = 1.0f; 
float AudioManager::m_FadeDuration = 1.0f;
float AudioManager::m_FadeTimer = 0.0f;

int AudioManager::m_PendingNextTrackIndex = -1;
bool AudioManager::m_PendingPlayPrevious = false;

bool AudioManager::Init()
{
	if (!Application::Get()->HasFrontend())
		return false;

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
		Application::Get()->ShowError(error.c_str(), "FMOD Error!");
		return false;
	}

	if (!LoadBank("Master.bank"))
	{
		return false;
	}

	if (!LoadBank("Master.strings.bank"))
	{
		return false;
	}

	int newIndex = rand() % m_BackgroundMusicPaths.size();
	m_TrackHistory.push_back(newIndex);
	PlayTrack(newIndex, true);

	sw.End();
	PAPI_TRACE("Initialised AudioManager/FMOD in {0}ms", sw.GetElapsedMilliseconds());
	return true;
}

bool AudioManager::LoadBank(const std::string& bankName)
{
	PAPI_ASSERT(m_FMODSystem && "Attempting to load bank from uninitialised FMOD system");

	FMOD::Studio::Bank* bank;
	FMOD_RESULT result = m_FMODSystem->loadBankFile(fmt::format("Content/Audio/{}", bankName).c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
	if (result != FMOD_OK)
	{
		std::string error = fmt::format("Failed to load bank file {}! Error: {}", bankName, FMOD_ErrorString(result));
		PAPI_ERROR("{}", error);
		Application::Get()->ShowError(error.c_str(), "FMOD Error");
		return false;
	}
	return true;
}

void AudioManager::PlayBackgroundMusic()
{
	int lastIndex = m_TrackHistory.empty() ? -1 : m_TrackHistory.back();
	int newIndex;

	do {
		newIndex = rand() % m_BackgroundMusicPaths.size();
	} while (newIndex == lastIndex && m_BackgroundMusicPaths.size() > 1);

	m_PendingNextTrackIndex = newIndex;
	m_PendingPlayPrevious = false;

	if (m_BackgroundMusicInstance)
	{
		StartFadeOut();
	}
	else
	{
		m_TrackHistory.push_back(newIndex);
		PlayTrack(newIndex, true);
	}
}

void AudioManager::PlayPreviousMusic()
{
	if (m_TrackHistory.size() < 2)
	{
		PAPI_WARN("No previous background music instance to play!");
		return;
	}

	m_PendingPlayPrevious = true;
	m_PendingNextTrackIndex = -1;

	if (m_BackgroundMusicInstance)
	{
		StartFadeOut();
	}
	else
	{
		m_TrackHistory.pop_back();
		int previousIndex = m_TrackHistory.back();
		PlayTrack(previousIndex, true);
	}
}

void AudioManager::PlayTrack(int trackIndex, bool fadeIn)
{
	FMOD::Studio::EventDescription* eventDescription = nullptr;
	FMOD_RESULT result = m_FMODSystem->getEvent(m_BackgroundMusicPaths[trackIndex].c_str(), &eventDescription);
	if (result != FMOD_OK)
		{
			std::string error = fmt::format("Failed to get background music event! Error: {}", FMOD_ErrorString(result));
		PAPI_ERROR("{}", error);
		Application::Get()->ShowError(error.c_str(), "FMOD Error");
		return;
		}
	
	result = eventDescription->createInstance(&m_BackgroundMusicInstance);
	if (result != FMOD_OK)
	{
		std::string error = fmt::format("Failed to create background music instance! Error: {}", FMOD_ErrorString(result));
		PAPI_ERROR("{}", error);
		Application::Get()->ShowError(error.c_str(), "FMOD Error");
		return;
	}

	if (fadeIn)
	{
		m_BackgroundMusicInstance->setVolume(0.0f);
		StartFadeIn();
	}
	else
	{
		m_BackgroundMusicInstance->setVolume(1.0f);
		m_CurrentVolume = 1.0f;
	}

	result = m_BackgroundMusicInstance->start();
	if (result != FMOD_OK)
	{
		std::string error = fmt::format("Failed to start background music instance! Error: {}", FMOD_ErrorString(result));
		PAPI_ERROR("{}", error);
		Application::Get()->ShowError(error.c_str(), "FMOD Error");
		return;
	}

	PAPI_INFO("Playing track index: {} ({})", trackIndex, m_BackgroundMusicPaths[trackIndex]);
    PAPI_INFO("Current History: ");
    for (auto idx : m_TrackHistory) {
        PAPI_INFO("   {}", m_BackgroundMusicPaths[idx]);
    }

	PAPI_INFO("Playing track index: {} ({})", trackIndex, m_BackgroundMusicPaths[trackIndex]);
	PAPI_INFO("Current History: ");
	for (auto idx : m_TrackHistory) {
		PAPI_INFO("   {}", m_BackgroundMusicPaths[idx]);
	}
}

void AudioManager::StopCurrentMusic()
{
	if (m_BackgroundMusicInstance)
	{
		m_BackgroundMusicInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
		m_BackgroundMusicInstance->release();
		m_BackgroundMusicInstance = nullptr;
		m_CurrentVolume = 1.0f;
	}
}

void AudioManager::StartFadeOut()
{
	m_FadingOut = true;
	m_FadingIn = false;
	m_TargetVolume = 0.0f;
	m_FadeTimer = 0.0f;
}

void AudioManager::StartFadeIn()
{
	m_FadingIn = true;
	m_FadingOut = false;
	m_TargetVolume = 1.0f;
	m_FadeTimer = 0.0f;
	m_CurrentVolume = 0.0f;
}

SoundHandle AudioManager::PlaySound(const char* soundName)
{
	// TODO
	return SoundHandle(nullptr);
}

void AudioManager::Update()
{
	if (!Application::Get()->HasFrontend())
		return;

	float deltaTime = Application::Get()->GetDeltaTime();

	if (m_FadingOut && m_BackgroundMusicInstance)
	{
		m_FadeTimer += deltaTime;
		float progress = m_FadeTimer / m_FadeDuration;
		if (progress >= 1.0f)
		{
			progress = 1.0f;
			m_FadingOut = false;
			m_CurrentVolume = 0.0f;
			m_BackgroundMusicInstance->setVolume(m_CurrentVolume);
			StopCurrentMusic();

			if (m_PendingPlayPrevious)
			{
				m_PendingPlayPrevious = false;
				m_TrackHistory.pop_back();
				int previousIndex = m_TrackHistory.back();
				PlayTrack(previousIndex, true);
			}
			else if (m_PendingNextTrackIndex != -1)
			{
				m_TrackHistory.push_back(m_PendingNextTrackIndex);
				int newIndex = m_PendingNextTrackIndex;
				m_PendingNextTrackIndex = -1;
				PlayTrack(newIndex, true);
			}
		}
		else
		{
			m_CurrentVolume = 1.0f - progress;
			m_BackgroundMusicInstance->setVolume(m_CurrentVolume);
		}
	}

	if (m_FadingIn && m_BackgroundMusicInstance)
	{
		m_FadeTimer += deltaTime;
		float progress = m_FadeTimer / m_FadeDuration;
		if (progress >= 1.0f)
		{
			progress = 1.0f;
			m_FadingIn = false;
		}
		m_CurrentVolume = progress;
		m_BackgroundMusicInstance->setVolume(m_CurrentVolume);
	}

	if (m_FMODSystem)
	{
		FMOD_RESULT result = m_FMODSystem->update();
		if (result != FMOD_OK)
		{
			std::string error = fmt::format("Failed to update FMOD system! Error: {}", FMOD_ErrorString(result));
			PAPI_ERROR("{}", error);
			Application::Get()->ShowError(error.c_str(), "FMOD Error");
		}
	}
}

void AudioManager::Shutdown()
{
	if (!Application::Get()->HasFrontend())
		return;

	PAPI_TRACE("Shutting down AudioManager and FMOD.");

	if (m_BackgroundMusicInstance)
	{
		m_BackgroundMusicInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
		m_BackgroundMusicInstance->release();
		m_BackgroundMusicInstance = nullptr;
	}

	if (m_FMODSystem)
	{
		m_FMODSystem->release();
		m_FMODSystem = nullptr;
	}
}