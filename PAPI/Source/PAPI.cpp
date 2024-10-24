#include "papipch.h"

#include "Core/PAPILog.h"

bool s_ShouldRestart = false;

int main()
{
	InitLog();
	
	do
	{
		PAPI_INFO("Welcome to PAPI, papi");
        s_ShouldRestart = false;
	}
	while (s_ShouldRestart);
}
