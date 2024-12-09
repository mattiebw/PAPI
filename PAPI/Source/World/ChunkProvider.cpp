#include "papipch.h"
#include "World/ChunkProvider.h"
#include "World/TileSets.h"

DefaultChunkProvider::DefaultChunkProvider()
{
    srand(time(0));

    int Cellular_Seed = rand();

    PNoise_1.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    PNoise_1.SetSeed(rand());
    PNoise_1.SetFrequency(0.1f);
    PNoise_1.SetFractalType(FastNoiseLite::FractalType_FBm);
    PNoise_1.SetFractalOctaves(5);
    PNoise_1.SetFractalLacunarity(0.15f);
    PNoise_1.SetFractalGain(0.2f);

    CNoise_1.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    CNoise_1.SetSeed(Cellular_Seed);
    CNoise_1.SetFrequency(0.15f);
    CNoise_1.SetFractalType(FastNoiseLite::FractalType_FBm);
    CNoise_1.SetFractalOctaves(3);
    CNoise_1.SetFractalLacunarity(2.0f);
    CNoise_1.SetFractalGain(0.0f);
    CNoise_1.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Hybrid);
    CNoise_1.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);
    CNoise_1.SetCellularJitter(1.0f);

    CNoise_2.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    CNoise_2.SetSeed(Cellular_Seed);
    CNoise_2.SetFrequency(0.15f);
    CNoise_2.SetFractalType(FastNoiseLite::FractalType_FBm);
    CNoise_2.SetFractalOctaves(4);
    CNoise_2.SetFractalLacunarity(0.75f);
    CNoise_2.SetFractalGain(0.2f);
    CNoise_2.SetFractalWeightedStrength(0.2f);
    CNoise_2.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Euclidean);
    CNoise_2.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
    CNoise_2.SetCellularJitter(1.0f);
}

uint16_t DefaultChunkProvider::GetTileAt(int x, int y) const
{
    float Cnoise_1 = CNoise_1.GetNoise(static_cast<float>(x), static_cast<float>(y));
    float Pnoise_1 = PNoise_1.GetNoise(static_cast<float>(x), static_cast<float>(y));
    float Cnoise_2 = CNoise_2.GetNoise(static_cast<float>(x), static_cast<float>(y));
    // uses perlin noise to make rocks
    if (Pnoise_1 >= 0.08f && Pnoise_1 < 0.25f)
        return TileSets::StoneFloor;
    else if (Pnoise_1 >= 0.25f)
        return TileSets::StoneWall;
    // uses two layers of Cellular noise over the Perlin noise to make ponds
    else if (Cnoise_1 >= 0.8f)
    {
        if (Cnoise_2 >= -0.55f && Cnoise_2 < -0.4f)
        {
            int LWater = rand() % 6;
            switch (LWater)
            {
            case 0:
                return TileSets::LWater1;
                break;
            case 1:
                return TileSets::LWater2;
                break;
            case 2:
                return TileSets::LWater3;
                break;
            case 3:
                return TileSets::LWater4;
                break;
            case 4:
                return TileSets::LWater5;
                break;
            case 5:
                return TileSets::LWater6;
                break;
            }
        }
        else if (Cnoise_2 >= -0.4f)
        {
            return TileSets::Sand;
        }
        else
        {
            int DWater = rand() % 6;
            switch (DWater)
            {
            case 0:
                return TileSets::DWater1;
                break;
            case 1:
                return TileSets::DWater2;
                break;
            case 2:
                return TileSets::DWater3;
                break;
            case 3:
                return TileSets::DWater4;
                break;
            case 4:
                return TileSets::DWater5;
                break;
            case 5:
                return TileSets::DWater6;
                break;
            }
        }
    }
    // else = all remaining tiles are 50% grass and 12.5% for each flower
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