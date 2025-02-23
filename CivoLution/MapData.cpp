#include "MapData.hpp"

MapData::MapData(int width, int height, int tileSize)
{
    this->mapTiles = std::vector<std::vector<int>>(height, std::vector<int>(width));

};

// make this return a pointer if speed is an issue
std::vector<std::vector<int>> MapData::getMapTiles()
{
    return this->mapTiles;
};

void MapData::setMapTiles(std::vector<std::vector<int>> mapTiles)
{
    this->mapTiles = mapTiles;
};
