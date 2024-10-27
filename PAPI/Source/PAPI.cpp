#include "papipch.h"

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_main.h>

#include "Core/Application.h"
#include "Core/PAPILog.h"

bool g_ShouldRestart = false;

int main(int argc, char *argv[])
{
	InitLog(SDL_GetPrefPath("PAPI", "papi"));

	do
	{
		PAPI_INFO("Welcome to PAPI, papi");

		Application application({"PAPI", SemVer(1, 0, 0)});
		if (application.Init())
		{
			application.Run();
			application.Shutdown();
		}
		else
		{
			g_ShouldRestart = false;
			return -1;
		}
	}
	while (g_ShouldRestart);

	return 0;
}
