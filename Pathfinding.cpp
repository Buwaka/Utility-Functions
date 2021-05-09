#include "stdafx.h"
#include "Pathfinding.h"
#include "Grid.h"
#include <deque>
#include <queue>
#include <vector>
#include <algorithm>



struct Node
{
	int weight;
	Vector2I position;
	std::vector<Sutils::Direction> directions;
};



//dijkstra
std::vector<Sutils::Direction> Dijkstra(const Grid& map, Vector2f start, Vector2f destination)
{
	Vector2I gridstart = map.GetGridPosition(start.ToVector2I());
	Vector2I gridend = map.GetGridPosition(destination.ToVector2I());

	std::deque<Node*> queue;
	std::vector<Node> workingPaths;
	std::vector<std::vector<bool>> visited;

	if (!map.GetvalueGrid(gridend.x, gridend.y))
	{
		std::vector<Sutils::Direction> re = {};
		return re;
	}


	//init visited
	for (int x = 0; x < map.GetGridWidth(); x++)
	{
		std::vector<bool> temp;
		for (int y = 0; y < map.GetGridHeight(); y++)
		{
			temp.push_back(false);
		}
		visited.push_back(temp);
	}






	Node *up, *down, *left, *right;

	//up
	if ( gridstart.y + 1 < map.GetGridHeight() )
	{
		if (map.GetvalueGrid(gridstart.x, gridstart.y + 1) && !visited[gridstart.x][gridstart.y + 1])
		{
			up = new Node;
			up->directions.push_back(Sutils::Up);
			up->position = Vector2I{ gridstart.x, gridstart.y + 1 };
			up->weight = 1;
			if (up->position == gridend)
			{
				workingPaths.push_back(*up);
			}
			queue.push_back(up);
		}
	}


	//down
	if ( gridstart.y - 1 > 0)
	{
		if (map.GetvalueGrid(gridstart.x, gridstart.y - 1) && !visited[gridstart.x][gridstart.y - 1])
		{
			down = new Node;
			down->directions.push_back(Sutils::Down);
			down->position = Vector2I{ gridstart.x, gridstart.y - 1 };
			down->weight = 1;
			if (down->position == gridend)
			{
				workingPaths.push_back(*down);
			}
			queue.push_back(down);
		}
	}


	//left
	if ( gridstart.x - 1 > 0)
	{
		if (map.GetvalueGrid(gridstart.x - 1, gridstart.y) &&  !visited[gridstart.x - 1][gridstart.y])
		{
			left = new Node;
			left->directions.push_back(Sutils::Left);
			left->position = Vector2I{ gridstart.x - 1, gridstart.y };
			left->weight = 1;
			if (left->position == gridend)
			{
				workingPaths.push_back(*left);
			}
			queue.push_back(left);
		}
	}


	//right
	if ( gridstart.x + 1 < map.GetGridWidth())
	{
		if (map.GetvalueGrid(gridstart.x + 1, gridstart.y) &&  !visited[gridstart.x + 1][gridstart.y])
		{
			right = new Node;
			right->directions.push_back(Sutils::Right);
			right->position = Vector2I{ gridstart.x + 1, gridstart.y };
			right->weight = 1;
			if (right->position == gridend)
			{
				workingPaths.push_back(*right);
			}
			queue.push_back(right);
		}
	}

	visited[gridstart.x][gridstart.y] = true;


	while (!queue.empty())
	{
		Node* current = queue.front();

		//up
		if ( current->position.y + 1 < map.GetGridHeight())
		{
			if (map.GetvalueGrid(current->position.x, current->position.y + 1) && !visited[current->position.x][current->position.y + 1])
			{
				up = new Node{ *current };
				up->directions.push_back(Sutils::Up);
				up->position = Vector2I{ current->position.x, current->position.y + 1 };
				up->weight += 1;
				if (up->position == gridend)
				{
					workingPaths.push_back(*up);
				}
				queue.push_back(up);
				visited[current->position.x][current->position.y + 1] = true;
			}
		}


		//down
		if ( current->position.y - 1 >= 0)
		{
			if (map.GetvalueGrid(current->position.x, current->position.y - 1) && !visited[current->position.x][current->position.y - 1])
			{
				down = new Node{ *current };
				down->directions.push_back(Sutils::Down);
				down->position = Vector2I{ current->position.x, current->position.y - 1 };
				down->weight += 1;
				if (down->position == gridend)
				{
					workingPaths.push_back(*down);
				}
				queue.push_back(down);
				visited[current->position.x][current->position.y - 1] = true;
			}
		}


		//left
		if ( current->position.x - 1 >= 0)
		{
			if (map.GetvalueGrid(current->position.x - 1, current->position.y) && !visited[current->position.x - 1][current->position.y])
			{
				left = new Node{ *current };
				left->directions.push_back(Sutils::Left);
				left->position = Vector2I{ current->position.x - 1, current->position.y };
				left->weight += 1;
				if (left->position == gridend)
				{
					workingPaths.push_back(*left);
				}
				queue.push_back(left);
				visited[current->position.x - 1][current->position.y] = true;
			}
		}


		//right
		if ( current->position.x + 1 < map.GetGridWidth())
		{
			if (map.GetvalueGrid(current->position.x + 1, current->position.y) && !visited[current->position.x + 1][current->position.y])
			{
				right = new Node{ *current };
				right->directions.push_back(Sutils::Right);
				right->position = Vector2I{ current->position.x + 1, current->position.y };
				right->weight += 1;
				if (right->position == gridend)
				{
					workingPaths.push_back(*right);
				}
				queue.push_back(right);
				visited[current->position.x + 1][current->position.y] = true;
			}
		}


		visited[current->position.x][current->position.y] = true;
		delete current;
		queue.pop_front();
	}

	if (!workingPaths.empty())
	{
		auto NodeSort = [](const Node & a, const Node & b) -> bool { return a.weight > b.weight; };

		std::sort(workingPaths.begin(), workingPaths.end(), NodeSort);
		std::vector<Sutils::Direction> temp = std::vector<Sutils::Direction>(workingPaths.front().directions);

		return temp;
	}
	
	std::vector<Sutils::Direction> re = {};
	return re;
}


int DistanceSwitch(Vector2I start, Vector2I destination, Sutils::Heuristic type)
{
	switch (type)
	{
	case Sutils::Heuristic::manhatten: return Sutils::Manhattan(start, destination);
		break;
	case Sutils::Heuristic::euclidean: return Sutils::Euclidean(start, destination);
		break;
	case Sutils::Heuristic::octile: return Sutils::Octile(start, destination);
		break;
	case Sutils::Heuristic::chebyshev: return Sutils::Chebyshev(start, destination);
		break;
	}
	return 0; // shouldn't happen
}



std::vector<Sutils::Direction> Astar(const Grid& map, Vector2f start, Vector2f destination, Sutils::Heuristic type)
{
	Vector2I gridstart = map.GetGridPosition(start.ToVector2I());
	Vector2I gridend = map.GetGridPosition(destination.ToVector2I());


	bool found = false;

	auto NodeSort = [](const Node * a, const Node * b) -> bool { return a->weight < b->weight; };

	std::deque<Node*> queue;
	Node workingPaths;
	std::vector<std::vector<bool>> visited;

	if (!map.GetvalueGrid(gridend) || gridstart == gridend)
	{
		std::vector<Sutils::Direction> re = {};
		return re;
	}


	//init visited
	for (int x = 0; x < map.GetGridWidth(); x++)
	{
		std::vector<bool> temp;
		for (int y = 0; y < map.GetGridHeight(); y++)
		{
			temp.push_back(false);
		}
		visited.push_back(temp);
	}






	Node *up, *down, *left, *right;

	//up
	if (gridstart.y + 1 < map.GetGridHeight())
	{
		if (map.GetvalueGrid(gridstart.x, gridstart.y + 1) && !visited[gridstart.x][gridstart.y + 1])
		{
			up = new Node;
			up->directions.push_back(Sutils::Up);
			up->position = Vector2I{ gridstart.x, gridstart.y + 1 };

			//weight is one + heuristic
			// multiplied by 100, to prevent precission error without using floats
			int heuristic = DistanceSwitch(gridstart, gridend, type) * 100;
			up->weight = 100 + heuristic;

			if (up->position == gridend)
			{
				found = true;
				workingPaths = *up;
			}
			queue.push_back(up);
		}
	}


	//down
	if (gridstart.y - 1 > 0)
	{
		if (map.GetvalueGrid(gridstart.x, gridstart.y - 1) && !visited[gridstart.x][gridstart.y - 1])
		{
			down = new Node;
			down->directions.push_back(Sutils::Down);
			down->position = Vector2I{ gridstart.x, gridstart.y - 1 };

			//weight is one + heuristic
			// multiplied by 100, to prevent precission error without using floats
			int heuristic = DistanceSwitch(gridstart, gridend, type) * 100;
			down->weight = 100 + heuristic;

			if (down->position == gridend)
			{
				found = true;
				workingPaths = *down;
			}
			queue.push_back(down);
		}
	}


	//left
	if (gridstart.x - 1 > 0)
	{
		if (map.GetvalueGrid(gridstart.x - 1, gridstart.y) && !visited[gridstart.x - 1][gridstart.y])
		{
			left = new Node;
			left->directions.push_back(Sutils::Left);
			left->position = Vector2I{ gridstart.x - 1, gridstart.y };

			//weight is one + heuristic
			// multiplied by 100, to prevent precission error without using floats
			int heuristic = DistanceSwitch(gridstart, gridend, type) * 100;
			left->weight = 100 + heuristic;

			if (left->position == gridend)
			{
				found = true;
				workingPaths = *left;
			}
			queue.push_back(left);
		}
	}


	//right
	if (gridstart.x + 1 < map.GetGridWidth())
	{
		if (map.GetvalueGrid(gridstart.x + 1, gridstart.y) && !visited[gridstart.x + 1][gridstart.y])
		{
			right = new Node;
			right->directions.push_back(Sutils::Right);
			right->position = Vector2I{ gridstart.x + 1, gridstart.y };

			//weight is one + heuristic
			// multiplied by 100, to prevent precission error without using floats
			int heuristic = DistanceSwitch(gridstart, gridend, type) * 100;
			right->weight = 100 + heuristic;

			if (right->position == gridend)
			{
				found = true;
				workingPaths = *right;
			}
			queue.push_back(right);
		}
	}

	visited[gridstart.x][gridstart.y] = true;


	while (!queue.empty() && !found)
	{
		Node* current = queue.front();

		//up
		if (current->position.y + 1 < map.GetGridHeight())
		{
			if (map.GetvalueGrid(current->position.x, current->position.y + 1) && !visited[current->position.x][current->position.y + 1])
			{
				up = new Node{ *current };
				up->directions.push_back(Sutils::Up);
				up->position = Vector2I{ current->position.x, current->position.y + 1 };

				//weight is one + heuristic
				// multiplied by 100, to prevent precission error without using floats
				int heuristic = DistanceSwitch(current->position, gridend, type) * 100;
				up->weight += 100 + heuristic;

				if (up->position == gridend)
				{
					found = true;
					workingPaths = *up;
				}
				queue.push_back(up);
				visited[current->position.x][current->position.y + 1] = true;
			}
		}


		//down
		if (current->position.y - 1 >= 0)
		{
			if (map.GetvalueGrid(current->position.x, current->position.y - 1) && !visited[current->position.x][current->position.y - 1])
			{
				down = new Node{ *current };
				down->directions.push_back(Sutils::Down);
				down->position = Vector2I{ current->position.x, current->position.y - 1 };

				//weight is one + heuristic
				// multiplied by 100, to prevent precission error without using floats
				int heuristic = DistanceSwitch(current->position, gridend, type) * 100;
				down->weight += 100 + heuristic;

				if (down->position == gridend)
				{
					found = true;
					workingPaths = *down;
				}
				queue.push_back(down);
				visited[current->position.x][current->position.y - 1] = true;
			}
		}


		//left
		if (current->position.x - 1 >= 0)
		{
			if (map.GetvalueGrid(current->position.x - 1, current->position.y) && !visited[current->position.x - 1][current->position.y])
			{
				left = new Node{ *current };
				left->directions.push_back(Sutils::Left);
				left->position = Vector2I{ current->position.x - 1, current->position.y };

				//weight is one + heuristic
				// multiplied by 100, to prevent precission error without using floats
				int heuristic = DistanceSwitch(current->position, gridend, type) * 100;
				left->weight += 100 + heuristic;

				if (left->position == gridend)
				{
					found = true;
					workingPaths = *left;
				}
				queue.push_back(left);
				visited[current->position.x - 1][current->position.y] = true;
			}
		}


		//right
		if (current->position.x + 1 < map.GetGridWidth())
		{
			if (map.GetvalueGrid(current->position.x + 1, current->position.y) && !visited[current->position.x + 1][current->position.y])
			{
				right = new Node{ *current };
				right->directions.push_back(Sutils::Right);
				right->position = Vector2I{ current->position.x + 1, current->position.y };

				//weight is one + heuristic
				// multiplied by 100, to prevent precission error without using floats
				int heuristic = DistanceSwitch(current->position, gridend, type) * 100;
				right->weight += 100 + heuristic;

				if (right->position == gridend)
				{
					found = true;
					workingPaths = *right;
				}
				queue.push_back(right);
				visited[current->position.x + 1][current->position.y] = true;
			}
		}


		visited[current->position.x][current->position.y] = true;
		delete current;
		queue.pop_front();
		std::sort(queue.begin(), queue.end(), NodeSort);
		
	}

	for (Node* node : queue)
	{
		delete node;
	}

	if (workingPaths.directions.size() > 0)
	{
		std::vector<Sutils::Direction> temp = std::vector<Sutils::Direction>(workingPaths.directions);

		return temp;
	}

	std::vector<Sutils::Direction> re = {};
	return re;
}


