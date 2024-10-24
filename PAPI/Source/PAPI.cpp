#include "papipch.h"

#include <SDL3/SDL_main.h>

#include "Core/PAPILog.h"

bool s_ShouldRestart = false;

int main(int argc, char *argv[])
{
	InitLog();
	
	do
	{
		PAPI_INFO("Welcome to PAPI, papi");
        s_ShouldRestart = false;
	}
	while (s_ShouldRestart);

	return 0;
}
