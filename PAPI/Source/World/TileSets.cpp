#include "papipch.h"
#include "World/TileSets.h"

#include "World/TileSet.h"
#include "Render/Texture.h"
#include "Render/SpriteSheet.h"

Ref<TileSet> TileSets::MainTileSet = nullptr;
uint16_t TileSets::Grass = 0;
uint16_t TileSets::Stone = 0;

void TileSets::Init()
{
    Ref<Texture> texture = CreateRef<Texture>("Content/Textures/TerrainSpritesheet.png");
    Ref<SpriteSheet> spriteSheet = CreateRef<SpriteSheet>(texture);
    spriteSheet->CreateTilesFromTileSize(16, 16);
    MainTileSet = CreateRef<TileSet>(spriteSheet);

    TileData grassData;
    grassData.SpriteIndex = 0;
    grassData.IsSolid = false;
    Grass = MainTileSet->AddTile(grassData);

    TileData stoneData;
    stoneData.SpriteIndex = 9;
    stoneData.IsSolid = true;
    Stone = MainTileSet->AddTile(stoneData);
}
