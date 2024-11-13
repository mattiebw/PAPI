#pragma once

class Texture;

struct SpriteSheetSprite
{
	glm::vec2 TexCoordsMin;
	glm::vec2 TexCoordsMax;
};

class SpriteSheet
{
public:
	SpriteSheet(const Ref<Texture> &texture);

	uint32_t CreateSprite(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	uint32_t CreateTilesFromTileSize(uint32_t tileWidth, uint32_t tileHeight);

private:
	std::vector<SpriteSheetSprite> m_Sprites;
	Ref<Texture>                   m_Texture;
};
