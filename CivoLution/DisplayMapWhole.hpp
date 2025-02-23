#ifndef DISPLAY_MAP_WHOLE_H
#define DISPLAY_MAP_WHOLE_H

#include "MapData.hpp"
#include <iostream>
#include <SFML/Graphics.hpp>


class DisplayMapWhole
{
    public:
        DisplayMapWhole(MapData* mapData);
        void DisplayMapText();
        void DisplayMapImage(int tileSize);
    private:
        MapData* mapData;
};

#endif