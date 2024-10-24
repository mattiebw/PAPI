#include "papipch.h"

bool s_ShouldRestart = false;

int main()
{
	do
	{
		std::cout << "PAPI!\n";
        s_ShouldRestart = false;
	}
	while (s_ShouldRestart);
}
