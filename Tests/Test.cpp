#include <iostream>
#include "BrilliantGraph.h"

int main()
{
	BrilliantGraph::DirectedGraph<int, int> graph;
	graph.AddEdge(0, 1, 1);
	graph.AddEdge(0, 2, 2);
	graph.AddEdge(2, 3, 1);
	graph.AddEdge(3, 4, 1);
	graph.AddEdge(0, 5, 2);
	graph.AddEdge(1, 4, 5);
	graph.AddEdge(5, 6, 1);
	graph.AddEdge(4, 2, 1);
	graph.AddEdge(2, 0, 1);
	graph.AddEdge(1, 0, 1);

	auto s = BrilliantGraph::FindPath(graph, 1, 6, BrilliantGraph::Dijkstras());

	while (!s.empty())
	{
		std::cout << s.top().tNode << std::endl;
		s.pop();
	}

	return 0;
}