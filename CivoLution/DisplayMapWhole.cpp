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

// void DisplayMapWhole::DisplayMapImage(int tileSize, int seed)
// {
//     int width = mapData->getWidth();
//     int height = mapData->getHeight();
    
//     // Create a window that fits the entire map
//     sf::RenderWindow window(sf::VideoMode(width * tileSize, height * tileSize), "Map Display");
    
//     // Create a shape that will be used for drawing tiles
//     sf::RectangleShape tile(sf::Vector2f(tileSize, tileSize));
    
//     std::vector<std::vector<TileType>>* map = mapData->getMapTiles();
    
//     while (window.isOpen())
//     {
//         sf::Event event;
//         while (window.pollEvent(event))
//         {
//             if (event.type == sf::Event::Closed)
//                 window.close();
//         }
        
//         window.clear();
        
//         // Draw each tile
//         for (int y = 0; y < height; ++y)
//         {
//             for (int x = 0; x < width; ++x)
//             {
//                 // Set position for current tile
//                 tile.setPosition(x * tileSize, y * tileSize);
                
//                 // Set color based on tile type
//                 switch ((*map)[y][x])
//                 {
//                     case TileType::WATER:
//                         tile.setFillColor(sf::Color(0, 0, 255));      // Blue
//                         break;
//                     case TileType::GRASS_LAND:
//                         tile.setFillColor(sf::Color(34, 139, 34));    // Forest Green
//                         break;
//                     case TileType::FOREST:
//                         tile.setFillColor(sf::Color(0, 100, 0));      // Dark Green
//                         break;
//                     case TileType::DESERT:
//                         tile.setFillColor(sf::Color(238, 214, 175));  // Sand
//                         break;
//                     case TileType::MOUNTAIN:
//                         tile.setFillColor(sf::Color(139, 137, 137));  // Gray
//                         break;
//                     case TileType::ICE:
//                         tile.setFillColor(sf::Color(255, 255, 255));  // White
//                         break;
//                 }
                
//                 window.draw(tile);
//             }
//         }
//         // add the seed to the window title
//         std::string title = "Map Display - Seed: " + std::to_string(seed);
//         std::cout << title << std::endl;
//         window.setTitle(title);
//         window.display();
//     }
// }

bool DisplayMapWhole::DisplayMapImageWithSeedAndNext(int tileSize, int seed)
{
    int width = mapData->getWidth();
    int height = mapData->getHeight();

    // Get the desktop resolution for fullscreen
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();

    // Create a fullscreen window
    sf::RenderWindow window(desktopMode, "Map Display", sf::Style::Fullscreen);

    // Calculate the scaling factor to fit the map to the screen
    float scaleX = static_cast<float>(desktopMode.width) / (width * tileSize);
    float scaleY = static_cast<float>(desktopMode.height) / (height * tileSize);
    float scale = std::min(scaleX, scaleY); // Use the smaller scale to ensure the map fits entirely

    // Create a shape that will be used for drawing tiles
    sf::RectangleShape tile(sf::Vector2f(tileSize * scale, tileSize * scale));

    // Create buttons
    sf::RectangleShape generateButton(sf::Vector2f(150 * scale, 50 * scale)); // Generate New Map button
    generateButton.setPosition(10 * scale, 10 * scale); // Top-left corner
    generateButton.setFillColor(sf::Color::Green);

    sf::RectangleShape acceptButton(sf::Vector2f(150 * scale, 50 * scale)); // Accept Map button
    acceptButton.setPosition(desktopMode.width - 160 * scale, 10 * scale); // Top-right corner
    acceptButton.setFillColor(sf::Color::Red);

    // Button labels
    sf::Font font;
    if (!font.loadFromFile("Resources/Fonts/liberation-sans-italic.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return false;
    }

    sf::Text generateText("Generate", font, 20 * scale);
    generateText.setPosition(20 * scale, 20 * scale);
    generateText.setFillColor(sf::Color::White);

    sf::Text acceptText("Accept", font, 20 * scale);
    acceptText.setPosition(desktopMode.width - 140 * scale, 20 * scale);
    acceptText.setFillColor(sf::Color::White);

    std::vector<std::vector<TileType>>* map = mapData->getMapTiles();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle mouse clicks
            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                // Check if the "Generate New Map" button is clicked
                if (generateButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                {
                    window.close();
                    return false; // Return false to indicate a new map should be generated
                }

                // Check if the "Accept Map" button is clicked
                if (acceptButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                {
                    window.close();
                    return true; // Return true to indicate the map is accepted
                }
            }
        }

        window.clear();

        // Draw each tile
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                // Set position for current tile
                tile.setPosition(x * tileSize * scale, y * tileSize * scale);

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

        // Draw buttons
        window.draw(generateButton);
        window.draw(acceptButton);
        window.draw(generateText);
        window.draw(acceptText);

        // Update the window title with the seed
        std::string title = "Map Display - Seed: " + std::to_string(seed);
        window.setTitle(title);

        window.display();
    }

    return false; // Default return value if the window is closed without clicking a button
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