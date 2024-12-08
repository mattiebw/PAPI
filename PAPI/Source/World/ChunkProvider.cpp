#include "papipch.h"
#include "World/ChunkProvider.h"
#include "World/TileSets.h"

DefaultChunkProvider::DefaultChunkProvider()
{
    srand(time(0));

    m_Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_Noise.SetSeed(rand());
    m_Noise.SetFractalOctaves(5);
    m_Noise.SetFrequency(0.1f);
    m_Noise.SetFractalLacunarity(0.15f);
    m_Noise.SetFractalGain(0.2f);
    m_Noise.SetFractalType(FastNoiseLite::FractalType_FBm);
}

uint16_t DefaultChunkProvider::GetTileAt(int x, int y) const
{
    float noise = m_Noise.GetNoise(static_cast<float>(x), static_cast<float>(y));
    if (noise >= 0.08f && noise < 0.25f)
        return TileSets::StoneFloor;
    else if (noise >= 0.25f)
        return TileSets::StoneWall;
    else 
    {
        int Coin = rand() % 2;
        
        switch (Coin)
        {
        case 0:
            return TileSets::Grass;
            break;
        case 1:
            int Random_Flowers = rand() % 4;
            switch (Random_Flowers)
            {
            case 0:
                return TileSets::Flowers;
                break;
            case 1:
                return TileSets::Sunflowers;
                break;
            case 2:
                return TileSets::Lilies;
                break;
            case 3:
                return TileSets::Roses;
                break;
            }
            break;
        }
    }
    // 50% grass and 12.5% for each flower
}
