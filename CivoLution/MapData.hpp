#ifndef MAP_DATA_H
#define MAP_DATA_H

#include <vector>

class MapData
{
    public:
        MapData(int width, int height, int tileSize);
        std::vector<std::vector<int>> getMapTiles();
        void setMapTiles(std::vector<std::vector<int>> mapTiles);
        ~MapData();
    private:
        std::vector<std::vector<int>> mapTiles;
        int width;
        int height;
        int tileSize;
};

#endif