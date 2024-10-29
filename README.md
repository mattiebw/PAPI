# PAPI!
Team 12Z's Programming APIs coursework.

### The Project
For this project, we're making a small, multiplayer, top-down wizard combat game with some procedurally generated (and hopefully destructible) environments. Definitely not overscoped.

The APIs we're planning on using:
- [SDL3](https://github.com/libsdl-org/SDL) - Windowing, input, etc
- [OpenGL](https://en.wikipedia.org/wiki/OpenGL) - Rendering
- [Steamworks](https://partner.steamgames.com/doc/sdk) - Networking (GameNetworkingSockets), Steam integration
- [FMOD](https://www.fmod.com/) - Audio
- [FastNoiseLite](https://github.com/Auburn/FastNoiseLite) - Noise for world gen
- Maybe more! We could do physics? But probably not...

### Building
First, you have to clone the repo. Ensure you do this with the ``recursive`` flag. Better yet, use a GUI, such as [GitHub Desktop](https://desktop.github.com/download/), and use the clone button. It will clone recursively automatically.

Once you have cloned, run **Scripts/GenerateProjectsWindows.bat** (or **GenerateProjectsLinux.sh** if you're on Windows).

Assuming you're on Windows, there will now be a **PAPI.sln** file in the root directory, which you can open in Visual Studio 2022, and build like normal.
