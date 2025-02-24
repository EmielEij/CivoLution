#ifndef MAP_DATA_H
#define MAP_DATA_H

#include <vector>

enum class TileType
{
    WATER = 0,
    GRASS_LAND = 1,
    FOREST = 2,
    DESERT = 3,
    MOUNTAIN = 4,
    ICE = 5,
    RIVER = 6
};

class MapData
{
    public:
        MapData(int width, int height, int tileSize);
        std::vector<std::vector<TileType>> *getMapTiles();
        // void setMapTiles(std::vector<std::vector<TileType>> mapTiles);
        int getWidth();
        int getHeight();
        int getTileSize();
        ~MapData();
    private:
        std::vector<std::vector<TileType>> *mapTiles;
        int width;
        int height;
        int tileSize;
};

#endif