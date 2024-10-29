#pragma once
#include <SDL3/SDL_events.h>

#include "Scancode.h"

class Input
{
public:
	static void Init();
	static void Shutdown();

	NODISCARD static FORCEINLINE bool IsKeyDown(const Scancode key) { return s_Keys[key]; }
	NODISCARD static FORCEINLINE bool IsKeyDownThisFrame(const Scancode key)
	{
		return s_Keys[key] && !s_PreviousKeys[key];
	}

	NODISCARD static FORCEINLINE bool IsKeyUp(const Scancode key) { return !s_Keys[key]; }
	NODISCARD static FORCEINLINE bool IsKeyUpThisFrame(const Scancode key)
	{
		return !s_Keys[key] && s_PreviousKeys[key];
	}

	FORCEINLINE static void UpdateKeyArrays()
	{
		// Can't use memcpy_s here because it's not available on all compilers :(
		memcpy(s_PreviousKeys, s_Keys, PAPI_KEY_COUNT * sizeof(bool));
	}

	static void ProcessInputEvent(const SDL_KeyboardEvent &event);

protected:
	// Remember to update UpdateKeyArrays() and Init() if changing the size of these arrays from PAPI_KEY_COUNT
	// These arrays have to be the same size!
	static bool s_Keys[PAPI_KEY_COUNT];
	static bool s_PreviousKeys[PAPI_KEY_COUNT];
};
