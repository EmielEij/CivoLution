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
    std::vector<std::vector<TileType>>* map = mapData->getMapTiles();
    
    // Significantly reduce the number of rivers
    int numRivers = (width * height) / 40000;  // 1 river per 40000 tiles (was 10000)
    numRivers = std::max(numRivers, 3);        // At least 3 rivers (was 5)
    numRivers = std::min(numRivers, 15);       // No more than 15 rivers
    
    // Track visited tiles to avoid river crossings
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    
    // Direction vectors (8 directions: N, NE, E, SE, S, SW, W, NW)
    const int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
    const int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    
    // Seed random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Find potential river source points (mountains and ice)
    std::vector<std::pair<int, int>> sources;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if ((*map)[y][x] == TileType::MOUNTAIN || (*map)[y][x] == TileType::ICE) {
                sources.push_back(std::make_pair(x, y));
            }
        }
    }
    
    // If no mountains or ice, we can't generate rivers
    if (sources.empty()) {
        std::cerr << "No suitable river sources found (mountains or ice)" << std::endl;
        return;
    }
    
    // Shuffle sources to get random starting points
    std::shuffle(sources.begin(), sources.end(), gen);
    
    // Generate rivers
    int riversCreated = 0;
    std::vector<std::pair<int, int>> selectedSources;
    
    for (const std::pair<int, int>& source : sources) {
        if (riversCreated >= numRivers) break;
        
        int x = source.first;
        int y = source.second;
        
        // Skip if this tile is already visited
        if (visited[y][x]) continue;
        
        // Skip if too close to another river source (minimum distance check)
        bool tooClose = false;
        int minDistanceBetweenSources = width / 30; // Minimum distance between sources
        
        for (const std::pair<int, int>& existingSource : selectedSources) {
            int dx = x - existingSource.first;
            int dy = y - existingSource.second;
            int distanceSquared = dx*dx + dy*dy;
            
            if (distanceSquared < minDistanceBetweenSources*minDistanceBetweenSources) {
                tooClose = true;
                break;
            }
        }
        
        if (tooClose) continue;
        
        // Even with distance check, only use 20% of remaining sources (stricter filtering)
        std::uniform_real_distribution<> dis(0.0, 1.0);
        if (dis(gen) > 0.2) continue;
        
        selectedSources.push_back(std::make_pair(x, y));
        
        // Start river path
        std::vector<std::pair<int, int>> riverPath;
        riverPath.push_back(std::make_pair(x, y));
        visited[y][x] = true;
        
        bool reachedWater = false;
        int riverLength = 0;
        int maxRiverLength = std::min(width, height) / 2;  // Limit river length
        
        // Keep track of previous directions for smoother turns
        int prevDirection = -1;
        
        // Generate river path
        while (riverLength < maxRiverLength && !reachedWater) {
            typedef std::tuple<int, int, double> NeighborInfo;
            std::vector<NeighborInfo> neighbors;
            
            for (int dir = 0; dir < 8; ++dir) {
                int nx = x + dx[dir];
                int ny = y + dy[dir];
                
                // Check bounds
                if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
                
                // Skip visited tiles to prevent river crossings
                if (visited[ny][nx]) continue;
                
                // If we reached water, stop here
                if ((*map)[ny][nx] == TileType::WATER) {
                    riverPath.push_back(std::make_pair(nx, ny));
                    reachedWater = true;
                    break;
                }
                
                // Calculate weight based on terrain type (preference for flowing downhill)
                double weight = 0.0;
                switch ((*map)[ny][nx]) {
                    case TileType::ICE:        weight = 0.1; break;  // Least likely
                    case TileType::MOUNTAIN:   weight = 0.3; break;
                    case TileType::FOREST:     weight = 0.6; break;
                    case TileType::GRASS_LAND: weight = 0.8; break;  // Most likely
                    case TileType::DESERT:     weight = 0.5; break;
                    default:                   weight = 0.0; break;
                }
                
                // Add small randomness - less than before
                weight += dis(gen) * 0.1;  // Was 0.2, reduced for more predictable flow
                
                // Strong preference for continuing in similar direction (smoother rivers)
                if (prevDirection != -1) {
                    // Calculate how much this direction differs from previous
                    int dirDiff = std::min(std::abs(dir - prevDirection), 
                                          8 - std::abs(dir - prevDirection));
                    
                    // Highest weight for same direction, lower for sharp turns
                    if (dirDiff == 0) weight += 0.6;  // Same direction
                    else if (dirDiff == 1) weight += 0.4;  // 45 degree turn
                    else if (dirDiff == 2) weight += 0.1;  // 90 degree turn
                    // No bonus for sharp turns
                }
                
                neighbors.push_back(std::make_tuple(nx, ny, weight));
            }
            
            // If no valid neighbors or we reached water, stop
            if (neighbors.empty() || reachedWater) break;
            
            // Sort by weight descending
            std::sort(neighbors.begin(), neighbors.end(), 
                      [](const NeighborInfo& a, const NeighborInfo& b) { 
                          return std::get<2>(a) > std::get<2>(b); 
                      });
            
            // Select next tile (rarely take second best for slight variation)
            int nextIdx = 0;
            if (neighbors.size() > 1 && dis(gen) < 0.1) {  // Reduced from 0.3 to 0.1
                nextIdx = 1;
            }
            
            x = std::get<0>(neighbors[nextIdx]);
            y = std::get<1>(neighbors[nextIdx]);
            
            // Figure out which direction we moved in
            for (int dir = 0; dir < 8; ++dir) {
                if (x - riverPath.back().first == dx[dir] && 
                    y - riverPath.back().second == dy[dir]) {
                    prevDirection = dir;
                    break;
                }
            }
            
            riverPath.push_back(std::make_pair(x, y));
            visited[y][x] = true;
            
            riverLength++;
        }
        
        // If river reached water or is long enough, mark it on the map
        if (reachedWater || riverLength >= 20) {  // Increased minimum length from 10 to 15
            // Calculate river "strength" - rivers get wider/stronger as they flow
            const int maxRiverStrength = 3;  // Maximum river width enhancement
            
            for (size_t i = 0; i < riverPath.size(); ++i) {
                int rx = riverPath[i].first;
                int ry = riverPath[i].second;
                
                // Don't overwrite water tiles
                if ((*map)[ry][rx] != TileType::WATER) {
                    (*map)[ry][rx] = TileType::RIVER;
                    
                    // Rivers get stronger (wider) as they approach the end
                    double strengthFactor = static_cast<double>(i) / riverPath.size();
                    int riverStrength = static_cast<int>(strengthFactor * maxRiverStrength);
                    
                    // Add width to major rivers near their end
                    if (riverStrength > 0 && i > riverPath.size() / 2) {
                        for (int r = 1; r <= riverStrength; ++r) {
                            // Try to add river tiles in adjacent cells (width)
                            for (int dir = 0; dir < 8; dir += 2) {  // Use only cardinal directions
                                int nx = rx + dx[dir];
                                int ny = ry + dy[dir];
                                
                                if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
                                    (*map)[ny][nx] != TileType::WATER && 
                                    (*map)[ny][nx] != TileType::RIVER &&
                                    dis(gen) < 0.3) {  // Random chance to add width
                                    (*map)[ny][nx] = TileType::RIVER;
                                }
                            }
                        }
                    }
                }
            }
            
            riversCreated++;
        }
    }
    
    std::cout << "Generated " << riversCreated << " rivers" << std::endl;
}

// void MapGenerator::GenerateRivers(MapData* mapData)
// {
//     int width = mapData->getWidth();
//     int height = mapData->getHeight();
//     std::vector<std::vector<TileType>>* map = mapData->getMapTiles();

//     // Track visited tiles to prevent looping
//     std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));

//     // Direction vectors (N, NE, E, SE, S, SW, W, NW)
//     const int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
//     const int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};

//     std::random_device rd;
//     std::mt19937 gen(rd());

//     // Elevation mapping (lower values flow towards water)
//     std::unordered_map<TileType, int> elevation = {
//         {TileType::WATER, 0},      // Lowest elevation
//         {TileType::RIVER, 1},      // Low elevation
//         {TileType::GRASS_LAND, 2}, // Mid elevation
//         {TileType::FOREST, 3},     // Mid-high elevation
//         {TileType::DESERT, 4},     // High elevation
//         {TileType::MOUNTAIN, 5},   // Very high elevation
//         {TileType::ICE, 6}         // Highest elevation
//     };

//     std::vector<std::pair<int, int>> sources;
//     for (int y = 0; y < height; ++y) {
//         for (int x = 0; x < width; ++x) {
//             if ((*map)[y][x] == TileType::MOUNTAIN || (*map)[y][x] == TileType::ICE) {
//                 sources.push_back({x, y});
//             }
//         }
//     }

//     if (sources.empty()) return;

//     std::shuffle(sources.begin(), sources.end(), gen);
//     int numRivers = std::min(10, std::max(3, static_cast<int>(sources.size() * 0.05))); 

//     for (int i = 0; i < numRivers && !sources.empty(); ++i) {
//         int x = sources[i].first;
//         int y = sources[i].second;

//         if (visited[y][x]) continue;

//         std::vector<std::pair<int, int>> riverPath;
//         riverPath.push_back({x, y});
//         visited[y][x] = true;

//         bool reachedWater = false;
//         // int prevDirection = -1;
//         int maxRiverLength = std::min(width, height) / 2;
//         int riverLength = 0;

//         while (riverLength < maxRiverLength && !reachedWater) {
//             typedef std::tuple<int, int, int> NeighborInfo;
//             std::vector<NeighborInfo> neighbors;

//             for (int dir = 0; dir < 8; ++dir) {
//                 int nx = x + dx[dir];
//                 int ny = y + dy[dir];

//                 if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
//                 if (visited[ny][nx]) continue; 

//                 TileType type = (*map)[ny][nx];

//                 if (type == TileType::WATER) {
//                     reachedWater = true;
//                     break;
//                 }

//                 int heightValue = elevation[type];
//                 neighbors.push_back({nx, ny, heightValue});
//             }

//             if (neighbors.empty() || reachedWater) break;

//             std::sort(neighbors.begin(), neighbors.end(),
//                       [](const NeighborInfo& a, const NeighborInfo& b) { return std::get<2>(a) < std::get<2>(b); });

//             int nextIdx = 0;
//             if (neighbors.size() > 1 && (std::uniform_real_distribution<>(0.0, 1.0)(gen) < 0.2)) {
//                 nextIdx = 1;  
//             }

//             x = std::get<0>(neighbors[nextIdx]);
//             y = std::get<1>(neighbors[nextIdx]);

//             // prevDirection = nextIdx;
//             riverPath.push_back({x, y});
//             visited[y][x] = true;
//             riverLength++;
//         }

//         if (reachedWater || riverLength > 15) {
//             for (size_t i = 0; i < riverPath.size(); ++i) {
//                 int rx = riverPath[i].first;
//                 int ry = riverPath[i].second;

//                 if ((*map)[ry][rx] != TileType::WATER) {
//                     (*map)[ry][rx] = TileType::RIVER;
//                 }

//                 if (i > riverPath.size() / 3) {
//                     for (int r = 1; r <= 2; ++r) {
//                         for (int dir = 0; dir < 8; dir += 2) {
//                             int nx = rx + dx[dir];
//                             int ny = ry + dy[dir];

//                             if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
//                                 (*map)[ny][nx] != TileType::WATER && 
//                                 (*map)[ny][nx] != TileType::RIVER) {
//                                 (*map)[ny][nx] = TileType::RIVER;
//                             }
//                         }
//                     }
//                 }
//             }
//         }
//     }
// }