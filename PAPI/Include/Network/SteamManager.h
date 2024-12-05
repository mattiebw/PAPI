#pragma once

struct Lobby
{
	std::string Name;
};

class SteamManager
{
public:
	bool Init();
	void Update();
	void Shutdown();

	void SetRequestingLobbies();
	void SetNotRequestingLobbies();
	std::vector<Lobby> GetLobbies() { return m_Lobbies; };

	STEAM_CALLBACK(SteamManager, OnScreenshot, ScreenshotReady_t);

private:
	bool m_RequestingLobbies = false;
	std::vector<Lobby> m_Lobbies;
	
	bool m_SteamworksInitialised = false;
};
