# PAPI!

Team 12Z's Programming APIs coursework.

### The Team

- **Addi** - Work on Steamworks (friends, avatars, etc)
- **Callum** - Work on Steamworks (screenshots, chat, etc)
- **Iain** - Noise-based procedural world generation
- **Louis** - Audio through FMOD
- **Matt** - OpenGL renderer, Steamworks multiplayer/networking system, core engine

### The Project

For this project, we're making a small, multiplayer, top-down wizard combat game with some procedurally generated (and
hopefully destructible) environments. Definitely not overscoped.

The APIs we're planning on using:

- [SDL3](https://github.com/libsdl-org/SDL) - Windowing, input, etc
- [OpenGL](https://en.wikipedia.org/wiki/OpenGL) - Rendering
- [Steamworks](https://partner.steamgames.com/doc/sdk) - Networking (GameNetworkingSockets), Steam integration
- [FMOD](https://www.fmod.com/) - Audio
- [FastNoiseLite](https://github.com/Auburn/FastNoiseLite) - Noise for world gen
- Maybe more! We could do physics? But probably not...

### Building

First, you have to clone the repo. Ensure you do this with the ``recursive`` flag. Better yet, use a GUI, such
as [GitHub Desktop](https://desktop.github.com/download/), and use the clone button. It will clone recursively
automatically.

Once you have cloned, run **Setup.bat** (or **Setup.sh** if you're on Linux).

Assuming you're on Windows, there will now be a **PAPI.sln** file in the root directory, which you can open in Visual
Studio 2022, and build and run like normal.

### Architecture

We have a main **Application** class. This is where the main game loop is, and where the game is updated and rendered.
We also have a **Window** class, that wraps the SDL window, and a **Input** class, which has Input utility functions.

Each application has multiple **World**s, which have multiple **Entity**s. Entities use a subclass architecture, where
each entity type is a subclass and will override the virtual functions such as **Update** and **Render** in the base
class.

The world is viewed through a **Viewport**, which contains a reference to a **Camera** and the **Scene** that it is
viewing.

### Contributing

#### Creating a new entity

To create a new entity type, you first need to create a class that publically inherits from Entity. So, in your header:

```c++
#pragma once

#include "World/Entity.h"

class Player : public Entity
{
};
```

You also need to define some basic functions, but there's an easy macro for that. Simply add
``ENTITY_CLASS_BODY(EntityName)``
in your classes body. So we have:

```c++
#pragma once

#include "World/Entity.h"

class Player : public Entity
{
    ENTITY_CLASS_BODY(Player);
};
```

Finally, you'll need to register the entity in Entity.cpp for dynamic creation. In Entity::CreateEntity, add an
ENTITY_TYPE_CASE to the switch statement:

```c++
switch (type)
{
	...
	ENTITY_TYPE_CASE(Player)
	default: break;
}
```

That's it, the entity should now work and you can override the Update() and Render() functions like normal.

#### Adding a New Tile

First, go to TileSets.h and add the tile type to the declarations. For example, if adding ``FlowerTile``, change:

```c++
static uint16_t Grass, StoneFloor, StoneWall;
```

to:

```c++
static uint16_t Grass, StoneFloor, StoneWall, FlowerTile;
```

Then, in TileSets.cpp, add the definition at the top of the file:

```c++
uint16_t TileSets::FlowerTile = 0;
```

And then, at the bottom of the Init() function, create the tile. For this example:

```c++
TileData flowerTileData;
flowerTileData.SpriteIndex = 10;
flowerTileData.IsSolid = false;
FlowerTile = MainTileSet->AddTile(flowerTileData);
```
