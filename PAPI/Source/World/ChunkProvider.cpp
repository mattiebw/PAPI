#include "papipch.h"
#include "World/ChunkProvider.h"

#include "World/TileSets.h"

DefaultChunkProvider::DefaultChunkProvider()
{
    m_Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_Noise.SetFrequency(0.2f);
    m_Noise.SetFractalLacunarity(2.0f);
    m_Noise.SetFractalGain(0.5f);
    m_Noise.SetFractalType(FastNoiseLite::FractalType_FBm);
}

uint16_t DefaultChunkProvider::GetTileAt(int x, int y) const
{
    float noise = m_Noise.GetNoise(static_cast<float>(x), static_cast<float>(y));
    if (noise >= 0.08f && noise < 0.3f)
        return TileSets::StoneWall;
    if (noise >= 0.3f)
        return TileSets::StoneFloor;
    
    return TileSets::Grass;
}

void Noise_Map()
{
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.2f);
    noise.SetFractalLacunarity(2.0f);
    noise.SetFractalGain(0.5f);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);

    std::vector<float> noiseData(64 * 64);
    int index = 0;
    int Tile{};

    for (int y = 0; y < 64; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            noiseData[index++] = noise.GetNoise((float)x, (float)y);
            if (noise.GetNoise((float)x, (float)y) >= 0.08f && noise.GetNoise((float)x, (float)y) < 0.3f)
                Tile = 1;
            else if (noise.GetNoise((float)x, (float)y) >= 0.3f)
                Tile = 2;
            else
                Tile = 0;
        }
        std::cout << std::endl;
    }
}