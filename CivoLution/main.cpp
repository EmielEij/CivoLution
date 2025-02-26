#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

#include "MapData.hpp"
#include "MapGenerator.hpp"
#include "DisplayMapWhole.hpp"

const int TILE_SIZE = 1;
const int MAP_WIDTH = 1800;
const int MAP_HEIGHT = 900;


int main() {
    int seed = 0;
    bool seedFound = false;
    while (seedFound == false)
    {
        seed = static_cast<int>(std::time(nullptr));
        MapGenerator mapGenerator( MAP_WIDTH, MAP_HEIGHT );
        MapData* mapData = mapGenerator.GenerateMap( MAP_WIDTH, MAP_HEIGHT, TILE_SIZE , seed );
        DisplayMapWhole displayMapWhole( mapData );
        seedFound = displayMapWhole.DisplayMapImageWithSeedAndNext(TILE_SIZE, seed);
        // delete mapData;
    }

    std::cout << "Seed: " << seed << std::endl;

    return 0;
}
