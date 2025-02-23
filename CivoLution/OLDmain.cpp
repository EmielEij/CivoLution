
// const int TILE_SIZE = 2;
// const int MAP_WIDTH = 1000;
// const int MAP_HEIGHT = 550;
//#include "lib/FastNoiseLite/FastNoiseLite.h" // Include FastNoiseLite (download it from https://github.com/Auburn/FastNoiseLite)


#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include "lib/FastNoiseLite/FastNoiseLite.h" // Include FastNoiseLite (download it from https://github.com/Auburn/FastNoiseLite)

// Map constants
const int TILE_SIZE = 10;
const int MAP_WIDTH = 500;
const int MAP_HEIGHT = 500;
const float NOISE_SCALE = 3.0f / std::max(MAP_WIDTH, MAP_HEIGHT);
const float BASE_SCROLL_SPEED = 200.0f; 
const float SCROLL_SPEED = BASE_SCROLL_SPEED * (std::max(MAP_WIDTH, MAP_HEIGHT) / 100.0f);

const float BASE_ZOOM_SPEED = 1.1f;
const float ZOOM_SPEED = BASE_ZOOM_SPEED * (std::max(MAP_WIDTH, MAP_HEIGHT) / 100.0f);

const float MIN_ZOOM = std::max(0.3f, 5.0f / std::max(MAP_WIDTH, MAP_HEIGHT));
const float MAX_ZOOM = std::min(3.0f, std::max(MAP_WIDTH, MAP_HEIGHT) / 50.0f);
// const float SCROLL_SPEED = 200.0f;
// const float ZOOM_SPEED = 1.1f;
// const float MIN_ZOOM = 0.5f;
// const float MAX_ZOOM = 2.5f;

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "CivOlution");
    sf::View view(sf::FloatRect(0, 0, 800, 600));
    float zoomLevel = 1.0f;
    bool dragging = false;
    sf::Vector2i lastMousePos;

    std::vector<std::vector<int>> map(MAP_HEIGHT, std::vector<int>(MAP_WIDTH));

    // Noise generator
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(NOISE_SCALE);

    // Generate terrain
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            float n = (noise.GetNoise((float)x, (float)y) + 1) / 2;
            if (n < 0.2f) map[y][x] = 0;
            else if (n < 0.4f) map[y][x] = 1;
            else if (n < 0.55f) map[y][x] = 2;
            else if (n < 0.7f) map[y][x] = 3;
            else if (n < 0.85f) map[y][x] = 4;
            else map[y][x] = 5;
        }
    }

    sf::RectangleShape tile(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseWheelScrolled) {
                // Zoom in/out
                if (event.mouseWheelScroll.delta > 0 && zoomLevel > MIN_ZOOM) {
                    zoomLevel /= ZOOM_SPEED;
                    view.zoom(1 / ZOOM_SPEED);
                } else if (event.mouseWheelScroll.delta < 0 && zoomLevel < MAX_ZOOM) {
                    zoomLevel *= ZOOM_SPEED;
                    view.zoom(ZOOM_SPEED);
                }
            } else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                dragging = true;
                lastMousePos = sf::Mouse::getPosition(window);
            } else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                dragging = false;
            }
        }

        float deltaTime = clock.restart().asSeconds();
        sf::Vector2f movement(0.f, 0.f);

        // Keyboard movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) movement.y -= SCROLL_SPEED * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) movement.y += SCROLL_SPEED * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) movement.x -= SCROLL_SPEED * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) movement.x += SCROLL_SPEED * deltaTime;

        // Mouse drag movement
        if (dragging) {
            sf::Vector2i newMousePos = sf::Mouse::getPosition(window);
            sf::Vector2f delta = sf::Vector2f(lastMousePos - newMousePos) * zoomLevel;
            view.move(delta);
            lastMousePos = newMousePos;
        }

        view.move(movement);

        // Keep view within map boundaries
        sf::Vector2f viewSize = view.getSize();
        sf::Vector2f viewCenter = view.getCenter();
        float leftBound = viewSize.x / 2;
        float rightBound = MAP_WIDTH * TILE_SIZE - viewSize.x / 2;
        float topBound = viewSize.y / 2;
        float bottomBound = MAP_HEIGHT * TILE_SIZE - viewSize.y / 2;

        if (viewCenter.x < leftBound) view.setCenter(leftBound, viewCenter.y);
        if (viewCenter.x > rightBound) view.setCenter(rightBound, viewCenter.y);
        if (viewCenter.y < topBound) view.setCenter(viewCenter.x, topBound);
        if (viewCenter.y > bottomBound) view.setCenter(viewCenter.x, bottomBound);

        window.setView(view);
        window.clear();

        // Draw only the visible map area
        sf::Vector2f topLeft = view.getCenter() - view.getSize() / 2.f;
        int startX = std::max(0, static_cast<int>(topLeft.x / TILE_SIZE));
        int startY = std::max(0, static_cast<int>(topLeft.y / TILE_SIZE));
        int endX = std::min(MAP_WIDTH, startX + static_cast<int>(viewSize.x / TILE_SIZE) + 2);
        int endY = std::min(MAP_HEIGHT, startY + static_cast<int>(viewSize.y / TILE_SIZE) + 2);

        for (int y = startY; y < endY; ++y) {
            for (int x = startX; x < endX; ++x) {
                switch (map[y][x]) {
                    case 0: tile.setFillColor(sf::Color::Blue); break;
                    case 1: tile.setFillColor(sf::Color::Green); break;
                    case 2: tile.setFillColor(sf::Color(255, 255, 102)); break;
                    case 3: tile.setFillColor(sf::Color(34, 139, 34)); break;
                    case 4: tile.setFillColor(sf::Color(139, 69, 19)); break;
                    case 5: tile.setFillColor(sf::Color::White); break;
                }
                tile.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                window.draw(tile);
            }
        }

        window.display();
    }

    return 0;
}
