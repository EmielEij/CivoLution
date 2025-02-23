#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#include <vector>
#include "MapData.hpp"
#include "lib/FastNoiseLite/FastNoiseLite.h"

class MapGenerator
{
public:
    MapGenerator(float noiseScale);
    MapGenerator(int width, int height);
    MapData* GenerateMap(int width, int height, int tileSize, int seed);
private:
    float noiseScale;
};

#endif
