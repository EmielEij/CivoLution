#ifndef DISPLAY_MAP_WHOLE_H
#define DISPLAY_MAP_WHOLE_H

#include "MapData.hpp"
#include <iostream>
#include <SFML/Graphics.hpp>


class DisplayMapWhole
{
    public:
        DisplayMapWhole(MapData* mapData);
        ~DisplayMapWhole();
        void DisplayMapText();
        void DisplayMapImage(int tileSize);
        bool DisplayMapImageWithSeedAndNext(int tileSize, int seed);
    private:
        MapData* mapData;
};

#endif