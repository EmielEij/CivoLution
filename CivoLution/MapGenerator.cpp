#include "MapGenerator.hpp"

MapGenerator::MapGenerator( float noiseScale )
{
    this->noiseScale = noiseScale;
}

MapGenerator::MapGenerator( int width, int height )
{
    this->noiseScale = 3.0f / std::max(width, height);
}

MapData* MapGenerator::GenerateMap(int width, int height, int tileSize)
{
MapData* mapData = new MapData(width, height, tileSize);

FastNoiseLite noise;
// noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
noise.SetFrequency(this->noiseScale);

std::vector<std::vector<TileType>> *map = mapData->getMapTiles();
// Generate terrain
for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
        float n = (noise.GetNoise((float)x, (float)y) + 1) / 2;
        if (n < 0.45f) (*map)[y][x] = TileType::WATER;      // 45% water
        else if (n < 0.70f) (*map)[y][x] = TileType::GRASS_LAND; // 25% grassland
        else if (n < 0.80f) (*map)[y][x] = TileType::FOREST;     // 10% forest
        else if (n < 0.90f) (*map)[y][x] = TileType::MOUNTAIN;   // 10% mountains
        else (*map)[y][x] = TileType::ICE;                       // 10% ice
    }
}

return mapData;
}

