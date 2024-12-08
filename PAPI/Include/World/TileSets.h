#pragma once

class TileSet;

class TileSets
{
public:
	static void Init();
	static void Shutdown();

	static Ref<TileSet> MainTileSet;
	static uint16_t     Empty, Grass, Flowers, Sunflowers, Lilies, Roses, StoneFloor, StoneWall,
						Water1, Water2, Water3, Water4, Water5, Water6;
};
