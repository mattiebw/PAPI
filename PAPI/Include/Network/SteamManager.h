#pragma once

class SteamManager
{
public:
	bool Init();
	void Shutdown();

	STEAM_CALLBACK(SteamManager, OnScreenshot, ScreenshotReady_t);

private:
	bool m_SteamworksInitialised = false;
};
