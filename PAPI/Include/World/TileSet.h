#pragma once

struct SpriteSheetSprite;
class SpriteSheet;

struct TileData
{
    uint16_t SpriteIndex;
    bool IsSolid = false;
};

class TileSet
{
public:
    TileSet(const Ref<SpriteSheet> &spritesheet);
    ~TileSet();
    TileSet(const TileSet &other) = delete;
    TileSet(TileSet &&other) noexcept = delete;
    TileSet & operator=(const TileSet &other) = delete;
    TileSet & operator=(TileSet &&other) noexcept = delete;

    uint16_t AddTile(const TileData& data);
    NODISCARD FORCEINLINE TileData& GetTile(uint16_t tileIndex) { return m_Tiles[tileIndex]; }

    NODISCARD FORCEINLINE const Ref<SpriteSheet>& GetSpritesheet() const { return m_Spritesheet; }

    NODISCARD SpriteSheetSprite GetSpriteForTile(uint16_t tile) const;

protected:
    std::vector<TileData> m_Tiles;
    
    Ref<SpriteSheet> m_Spritesheet;
};
