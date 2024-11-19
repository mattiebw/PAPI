#pragma once

class TileSet;

class TileSets
{
public:
    static void Init();
    
    static Ref<TileSet> MainTileSet;
    static uint16_t Grass, Stone;
};
