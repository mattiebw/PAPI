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

    CNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    CNoise.SetSeed(rand());
    CNoise.SetFrequency(0.15f);
    CNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    CNoise.SetFractalOctaves(3);
    CNoise.SetFractalLacunarity(2.0f);
    CNoise.SetFractalGain(0.0f);
    CNoise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Hybrid);
    CNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);
    CNoise.SetCellularJitter(1.0f);
}

uint16_t DefaultChunkProvider::GetTileAt(int x, int y) const
{
    float noise_1 = CNoise.GetNoise(static_cast<float>(x), static_cast<float>(y));
    if (noise_1 >= 0.8f)
    {
        int Water = rand() % 6;
        switch (Water)
        {
        case 0:
            return TileSets::Water1;
            break;
        case 1:
            return TileSets::Water2;
            break;
        case 2:
            return TileSets::Water3;
            break;
        case 3:
            return TileSets::Water4;
            break;
        case 4:
            return TileSets::Water5;
            break;
        case 5:
            return TileSets::Water6;
            break;
        }
    }
    else
    {
        float noise = m_Noise.GetNoise(static_cast<float>(x), static_cast<float>(y));
        if (noise >= 0.08f && noise < 0.25f)
            return TileSets::StoneFloor;
        else if (noise >= 0.25f)
            return TileSets::StoneWall;
        // else is 50% grass and 12.5% for each flower
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
    }
}