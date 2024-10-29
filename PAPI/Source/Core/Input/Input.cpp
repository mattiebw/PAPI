#include "papipch.h"
#include "Core/Input/Input.h"

bool Input::s_Keys[PAPI_KEY_COUNT];
bool Input::s_PreviousKeys[PAPI_KEY_COUNT];

void Input::Init()
{
	memset(s_Keys, 0, PAPI_KEY_COUNT * sizeof(bool));
	memset(s_PreviousKeys, 0, PAPI_KEY_COUNT * sizeof(bool));
}

void Input::Shutdown()
{
	// Nothing to do here for now, but if we add static delegates or something, we might need to clean up.
}

void Input::ProcessInputEvent(const SDL_KeyboardEvent &event)
{
	s_Keys[event.scancode] = event.down;
}
