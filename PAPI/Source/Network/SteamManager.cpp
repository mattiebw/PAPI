#include "papipch.h"
#include "Network/SteamManager.h"

#include "Core/Application.h"

extern "C" void SteamWarningMessage(int level, const char * message)
{
	if (level == 0)
		PAPI_INFO("Steamworks message: {0}", message);
	else if (level == 1)
		PAPI_WARN("Steamworks warning: {0}", message);
	else
		PAPI_ERROR("Steamworks error: {0}", message);

	PAPI_ASSERT(level == 0); // Break on warnings/errors
}

bool SteamManager::Init()
{
	SteamErrMsg error; // SteamErrMsg is a char[1024]
	if (SteamAPI_InitEx(&error) != k_ESteamAPIInitResult_OK)
	{
		std::string err = fmt::format("Failed to initialise Steamworks (make sure Steam is running): {}", static_cast<const char*>(error));
		PAPI_ERROR("{0}", err);
		Application::Get()->ShowError(err.c_str(), "Steamworks Initialisation Error");
		return false;
	}

	SteamUtils()->SetWarningMessageHook(&SteamWarningMessage);

	// TODO: Okay, lets try and print out all of our friends.
	// You'll want to look at https://partner.steamgames.com/doc/api/ISteamFriends
	// GetFriendCount(), GetFriendByIndex()
	for (int i = 0; i < SteamFriends()->GetFriendCount(k_EFriendFlagImmediate); i++)
	{
		auto steamFriend = SteamFriends()->GetFriendByIndex(i, k_EFriendFlagImmediate);
	}

	// For networking:
	// Lets initialise the Steam relay access.
	// MW @todo: Setup a callback so we know when we have (or don't have) access to the relay.
	SteamNetworkingUtils()->InitRelayNetworkAccess();
	
	m_SteamworksInitialised = true;
	PAPI_INFO("Successfully initialised Steamworks. App ID: {}", SteamUtils()->GetAppID());
	return true;
}

void SteamManager::Update()
{
	SteamAPI_RunCallbacks();
}

void SteamManager::Shutdown()
{
	if (!m_SteamworksInitialised)
		return;
	
	PAPI_TRACE("Shutting down Steamworks");
	SteamAPI_Shutdown();

	m_SteamworksInitialised = false;
}

void SteamManager::SetRequestingLobbies()
{
	m_RequestingLobbies = true;	
}

void SteamManager::SetNotRequestingLobbies()
{
	m_RequestingLobbies = false;
}

void SteamManager::OnScreenshot(ScreenshotReady_t *pParam)
{
	PAPI_INFO("Screenshot taken: {0}", pParam->m_eResult == k_EResultOK ? "OK" : "Failed");
}
