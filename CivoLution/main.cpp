#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

#include "MapData.hpp"
#include "MapGenerator.hpp"
#include "DisplayMapWhole.hpp"

const int TILE_SIZE = 1;
const int MAP_WIDTH = 1800;
const int MAP_HEIGHT = 900;


int main() {
    MapGenerator mapGenerator( MAP_WIDTH, MAP_HEIGHT );
    MapData* mapData = mapGenerator.GenerateMap( MAP_WIDTH, MAP_HEIGHT, TILE_SIZE );
    DisplayMapWhole displayMapWhole( mapData );
    displayMapWhole.DisplayMapImage(TILE_SIZE);

    return 0;
}
