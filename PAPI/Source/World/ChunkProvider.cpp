#include "papipch.h"
#include "World/ChunkProvider.h"

uint16_t DefaultChunkProvider::GetTileAt(int x, int y) const
{
    return TileSets::Grass;
}
