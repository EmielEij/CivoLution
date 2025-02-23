#include "DisplayMapWhole.hpp"

DisplayMapWhole::DisplayMapWhole(MapData* mapData)
{
    this->mapData = mapData;
}

void DisplayMapWhole::DisplayMapText()
{
    std::vector<std::vector<TileType>> *map = this->mapData->getMapTiles();
    for (int y = 0; y < this->mapData->getHeight(); ++y) {
        for (int x = 0; x < this->mapData->getWidth(); ++x) {
            TileType tileType = (*map)[y][x];
           switch (tileType) {
           case TileType::WATER:
               std::cout << "W";
               break;
           case TileType::GRASS_LAND:
               std::cout << "G";
               break;
           case TileType::FOREST:
               std::cout << "F";
               break;
           case TileType::DESERT:
               std::cout << "D";
               break;
           case TileType::MOUNTAIN:
               std::cout << "M";
               break;
           case TileType::ICE:
               std::cout << "I";
               break;
            }
        }
        std::cout << std::endl;
    }
}

void DisplayMapWhole::DisplayMapImage(int tileSize)
{
    int width = mapData->getWidth();
    int height = mapData->getHeight();
    
    // Create a window that fits the entire map
    sf::RenderWindow window(sf::VideoMode(width * tileSize, height * tileSize), "Map Display");
    
    // Create a shape that will be used for drawing tiles
    sf::RectangleShape tile(sf::Vector2f(tileSize, tileSize));
    
    std::vector<std::vector<TileType>>* map = mapData->getMapTiles();
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        window.clear();
        
        // Draw each tile
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                // Set position for current tile
                tile.setPosition(x * tileSize, y * tileSize);
                
                // Set color based on tile type
                switch ((*map)[y][x])
                {
                    case TileType::WATER:
                        tile.setFillColor(sf::Color(0, 0, 255));      // Blue
                        break;
                    case TileType::GRASS_LAND:
                        tile.setFillColor(sf::Color(34, 139, 34));    // Forest Green
                        break;
                    case TileType::FOREST:
                        tile.setFillColor(sf::Color(0, 100, 0));      // Dark Green
                        break;
                    case TileType::DESERT:
                        tile.setFillColor(sf::Color(238, 214, 175));  // Sand
                        break;
                    case TileType::MOUNTAIN:
                        tile.setFillColor(sf::Color(139, 137, 137));  // Gray
                        break;
                    case TileType::ICE:
                        tile.setFillColor(sf::Color(255, 255, 255));  // White
                        break;
                }
                
                window.draw(tile);
            }
        }
        
        window.display();
    }
}