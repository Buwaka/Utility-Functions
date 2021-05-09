#pragma once
#include <vector>
#include "Grid.h"
#include "utils.h"

std::vector<Sutils::Direction> Dijkstra(const Grid& map, Vector2f start, Vector2f destination);
std::vector<Sutils::Direction> Astar(const Grid& map, Vector2f start, Vector2f destination, Sutils::Heuristic type = Sutils::manhatten);

