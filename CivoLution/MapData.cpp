#include "MapData.hpp"

MapData::MapData(int width, int height, int tileSize)
{
    this->mapTiles = new std::vector<std::vector<TileType>>(height, std::vector<TileType>(width));
    this->width = width;
    this->height = height;
    this->tileSize = tileSize;
};

// make this return a pointer if speed is an issue
std::vector<std::vector<TileType>> *MapData::getMapTiles()
{
    return this->mapTiles;
};

MapData::~MapData()
{
    delete this->mapTiles;
};

int MapData::getWidth()
{
    return this->width;
};

int MapData::getHeight()
{
    return this->height;
};

int MapData::getTileSize()
{
    return this->tileSize;
};

// void MapData::setMapTiles(std::vector<std::vector<TileType>> mapTiles)
// {
//     this->mapTiles =  mapTiles;
// };
