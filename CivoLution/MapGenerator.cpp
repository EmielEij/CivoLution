#include "MapGenerator.hpp"

MapGenerator::MapGenerator( float noiseScale )
{
    this->noiseScale = noiseScale;
}

MapGenerator::MapGenerator( int width, int height )
{
    this->noiseScale = 3.0f / std::max(width, height);
}

MapData* MapGenerator::GenerateMap(int width, int height, int tileSize, int seed)
{
MapData* mapData = new MapData(width, height, tileSize);

FastNoiseLite noise;
// noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
noise.SetFrequency(this->noiseScale);
noise.SetSeed(seed);

std::vector<std::vector<TileType>> *map = mapData->getMapTiles();
// Generate terrain
for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
        float n = (noise.GetNoise((float)x, (float)y) + 1) / 2;
        if (n < 0.45f) (*map)[y][x] = TileType::WATER;      // 45% water
        else if (n < 0.70f) (*map)[y][x] = TileType::GRASS_LAND; // 25% grassland
        else if (n < 0.80f) (*map)[y][x] = TileType::FOREST;     // 10% forest
        else if (n < 0.90f) (*map)[y][x] = TileType::MOUNTAIN;   // 10% mountains
        else (*map)[y][x] = TileType::ICE;                       // 10% ice
    }
}

GenerateRivers(mapData);

return mapData;
}

void MapGenerator::GenerateRivers(MapData* mapData)
{
    int width = mapData->getWidth();
    int height = mapData->getHeight();
    auto& map = *mapData->getMapTiles();

    std::vector<std::pair<int, int>> mountainTiles;
    
    // Elevation mapping
    std::unordered_map<TileType, int> elevation = {
        {TileType::WATER, 0},
        {TileType::GRASS_LAND, 1},
        {TileType::FOREST, 2},
        {TileType::DESERT, 3},
        {TileType::MOUNTAIN, 4},
        {TileType::ICE, 5},
        {TileType::RIVER, 1} // Rivers should behave like grassland
    };

    // Step 1: Collect all mountain tiles as potential river sources
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (map[y][x] == TileType::MOUNTAIN) {
                mountainTiles.emplace_back(x, y);
            }
        }
    }

    // Step 2: Randomly select some mountains to be river sources
    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(mountainTiles.begin(), mountainTiles.end(), rng);

    int numRivers = std::max(1, static_cast<int>(mountainTiles.size() * 0.10)); // 10% of mountains

    for (int i = 0; i < numRivers && i < static_cast<int>(mountainTiles.size()); ++i) {
        int x = mountainTiles[i].first;
        int y = mountainTiles[i].second;

        // Step 3: Follow the lowest neighboring tile
        while (true) {
            map[y][x] = TileType::RIVER; // Mark as river
            
            std::vector<std::pair<int, int>> neighbors = {
                {x, y - 1}, {x, y + 1}, {x - 1, y}, {x + 1, y} // Up, Down, Left, Right
            };

            std::shuffle(neighbors.begin(), neighbors.end(), rng); // Add randomness

            std::pair<int, int> nextTile = {x, y};
            int minElevation = elevation[map[y][x]]; // Current tile's elevation

            for (auto& neighbor : neighbors) {
                int nx = neighbor.first;
                int ny = neighbor.second;
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    int neighborElevation = elevation[map[ny][nx]];

                    if (map[ny][nx] == TileType::WATER) {
                        return; // Stop when reaching water
                    }

                    if (neighborElevation < minElevation) { // Prefer lower elevation
                        minElevation = neighborElevation;
                        nextTile = {nx, ny};
                    }
                }
            }

            // If no movement possible, stop
            if (nextTile == std::make_pair(x, y)) break;

            x = nextTile.first;
            y = nextTile.second;
        }
    }

    // Step 4: Expand rivers to make them wider
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            if (map[y][x] == TileType::RIVER) {
                for (auto& direction : std::vector<std::pair<int, int>>{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}) {
                    int nx = direction.first;
                    int ny = direction.second;
                    if (map[ny][nx] != TileType::WATER) {
                        map[ny][nx] = TileType::RIVER;
                    }
                }
            }
        }
    }
}