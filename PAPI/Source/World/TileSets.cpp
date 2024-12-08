#include "papipch.h"
#include "World/TileSets.h"

#include "Core/Application.h"
#include "World/TileSet.h"
#include "Render/Texture.h"
#include "Render/SpriteSheet.h"

Ref<TileSet> TileSets::MainTileSet = nullptr;
uint16_t     TileSets::Empty       = 0;
uint16_t     TileSets::Grass       = 0;
uint16_t     TileSets::Flowers     = 0;
uint16_t     TileSets::Sunflowers  = 0;
uint16_t     TileSets::Lilies       = 0;
uint16_t     TileSets::Roses       = 0;
uint16_t     TileSets::StoneFloor  = 0;
uint16_t     TileSets::StoneWall   = 0;

void TileSets::Init()
{
	if (Application::Get()->HasFrontend())
	{
		TextureSpecification spec;
		spec.MagFilter = FilterMode::Nearest;
		spec.MinFilter = FilterMode::Nearest;
		spec.Wrap = WrapMode::ClampToEdge;
		spec.FlipVertically = false;
		Ref<Texture>     texture     = CreateRef<Texture>("Content/Textures/TerrainSpritesheet.png", spec);
		
		Ref<SpriteSheet> spriteSheet = CreateRef<SpriteSheet>(texture);
		spriteSheet->CreateTilesFromTileSize(16, 16);
		MainTileSet = CreateRef<TileSet>(spriteSheet);
	} else
	{
		MainTileSet = CreateRef<TileSet>(nullptr);
	}

	TileData grassData;
	grassData.SpriteIndex = 0;
	grassData.IsSolid     = false;
	Grass                 = MainTileSet->AddTile(grassData);

	TileData FlowersData;
	FlowersData.SpriteIndex = 10;
	FlowersData.IsSolid = false;
	Flowers = MainTileSet->AddTile(FlowersData);

	TileData SunflowersData;
	SunflowersData.SpriteIndex = 29;
	SunflowersData.IsSolid = false;
	Sunflowers = MainTileSet->AddTile(SunflowersData);

	TileData LiliesData;
	LiliesData.SpriteIndex = 30;
	LiliesData.IsSolid = false;
	Lilies = MainTileSet->AddTile(LiliesData);

	TileData RosesData;
	RosesData.SpriteIndex = 31;
	RosesData.IsSolid = false;
	Roses = MainTileSet->AddTile(RosesData);

	TileData stoneFloorData;
	stoneFloorData.SpriteIndex = 8;
	stoneFloorData.IsSolid     = false;
	StoneFloor                 = MainTileSet->AddTile(stoneFloorData);

	TileData stoneWallData;
	stoneWallData.SpriteIndex = 9;
	stoneWallData.IsSolid     = true;
	StoneWall                 = MainTileSet->AddTile(stoneWallData);
}

void TileSets::Shutdown()
{
	MainTileSet = nullptr;
}
