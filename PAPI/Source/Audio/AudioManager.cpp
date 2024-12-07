﻿#include "papipch.h"
#include "Audio/AudioManager.h"

#include <fmod_studio.hpp>
#include <fmod_errors.h>

#include "Core/Application.h"

FMOD::Studio::System *AudioManager::m_FMODSystem = nullptr;
FMOD::Studio::EventInstance* AudioManager::m_BackgroundMusicInstance = nullptr;

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

	PlayBackgroundMusic();

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
	FMOD::Studio::EventDescription* eventDescription = nullptr;
	FMOD_RESULT result = m_FMODSystem->getEvent("event:/Music/BackgroundMusic", &eventDescription);
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
		std::string error = fmt::format("Failed to get background music instance! Error: {}", FMOD_ErrorString(result));
		PAPI_ERROR("{}", error);
		Application::Get()->ShowError(error.c_str(), "FMOD Error");
		return;
	}

	result = m_BackgroundMusicInstance->start();
	if (result != FMOD_OK)
	{
		std::string error = fmt::format("Failed to start background music instance! Error: {}", FMOD_ErrorString(result));
		PAPI_ERROR("{}", error);
		Application::Get()->ShowError(error.c_str(), "FMOD Error");
		return;
	}
}

SoundHandle AudioManager::PlaySound(const char *soundName)
{
	// TODO
	return SoundHandle(nullptr);
}

void AudioManager::Update()
{
	if (!Application::Get()->HasFrontend())
		return;
	
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
