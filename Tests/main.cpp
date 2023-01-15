#include <cassert>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <detail/SetInterface.h>
#include <detail/MapInterface.h>
#include <AdjacencyList.h>

void TestMapInterface()
{
	using namespace Brilliant::Graph;
	
	static_assert(detail::MapInterface<std::map<int, int>>);
	static_assert(detail::MapInterface<std::unordered_map<int, int>>);
	static_assert(detail::MapInterface<std::multimap<int, int>>);
	static_assert(detail::MapInterface<std::unordered_multimap<int, int>>);

	static_assert(!detail::MapInterface<std::vector<int>>);
	static_assert(!detail::MapInterface<std::set<int>>);
}

void TestSetInterface()
{
	using namespace Brilliant::Graph;

	static_assert(detail::SetInterface<std::set<int>>);
	static_assert(detail::SetInterface<std::unordered_set<int>>);
	static_assert(detail::SetInterface<std::multiset<int>>);
	static_assert(detail::SetInterface<std::unordered_multiset<int>>);

	static_assert(!detail::SetInterface<std::vector<int>>);
	static_assert(!detail::SetInterface<std::map<int, int>>);
}

template<class V, class E, class... TheRest>
void AddEdges(Brilliant::Graph::AdjacencyList<V, E, TheRest...>& graph)
{
	graph.AddEdge(0, 1);	//          8
	graph.AddEdge(1, 2);	//          |
	graph.AddEdge(0, 3, 1);	//  0 > 3 > 5 > 7 
	graph.AddEdge(3, 4, 4);	//  v   v   v
	graph.AddEdge(3, 5, {});//  1 - 4   6
	graph.AddEdge(5, 6, 13);//  v
	graph.AddEdge(5, 7);	//  2
	graph.AddEdge(5, 8);	//  |
	graph.AddEdge(8, 5);	//  9
	graph.AddEdge(2, 9);
	graph.AddEdge(9, 2);
	graph.AddEdge(1, 4);
	graph.AddEdge(4, 1);
}

template<class V, class... TheRest>
void AddEdges(Brilliant::Graph::AdjacencyList<V, void, TheRest...>& graph)
{
	graph.AddEdge(0, 1);	//          8
	graph.AddEdge(1, 2);	//          |
	graph.AddEdge(0, 3);	//  0 > 3 > 5 > 7 
	graph.AddEdge(3, 4);	//  v   v   v
	graph.AddEdge(3, 5);	//  1 - 4   6
	graph.AddEdge(5, 6);	//  v
	graph.AddEdge(5, 7);	//  2
	graph.AddEdge(5, 8);	//  |
	graph.AddEdge(8, 5);	//  9
	graph.AddEdge(2, 9);
	graph.AddEdge(9, 2);
	graph.AddEdge(1, 4);
	graph.AddEdge(4, 1);
}

template<class V, class E, class... TheRest>
void TestGetEdges(Brilliant::Graph::AdjacencyList<V, E, TheRest...>& graph)
{
	auto fives_edges = graph.GetEdges(5);
	assert(std::ranges::distance(std::ranges::begin(fives_edges), std::ranges::end(fives_edges)) == 3);
	assert(std::ranges::find_if(fives_edges, [](auto& item) {
		return *item.first == 8;
		}) != std::ranges::end(fives_edges));
	assert(std::ranges::find_if(fives_edges, [](auto& item) {
		return *item.first == 9;
		}) == std::ranges::end(fives_edges));

	auto found = std::ranges::find_if(fives_edges, [](auto& item) {
		return *item.first == 6;
		});
	assert(found != std::ranges::end(fives_edges));
	assert(found->second == 13);
}

template<class V, class... TheRest>
void TestGetEdges(Brilliant::Graph::AdjacencyList<V, void, TheRest...>& graph)
{
	auto fives_edges = graph.GetEdges(5);
	assert(std::ranges::distance(std::ranges::begin(fives_edges), std::ranges::end(fives_edges)) == 3);
	assert(std::ranges::find_if(fives_edges, [](auto& item) {
		return *item == 8;
		}) != std::ranges::end(fives_edges));
	assert(std::ranges::find_if(fives_edges, [](auto& item) {
		return *item == 9;
		}) == std::ranges::end(fives_edges));
}

template<class Graph>
void TestAdjList(Graph& graph)
{
	constexpr auto num_vert{ 10 };
	constexpr auto num_edge{ 13 };

	for (auto i : std::views::iota(0, num_vert - 1))
	{
		graph.AddVertex(i);
	}

	graph.EmplaceVertex(num_vert - 1);
							 
	AddEdges(graph);

	assert(graph.HasEdge(0, 1));
	assert(graph.HasEdge(1, 2));
	assert(graph.HasEdge(2, 9));
	assert(!graph.HasEdge(5, 4));
	
	const auto [num_vertices, num_edges] = graph.Size();
	assert(num_vertices == num_vert);
	assert(num_edges == num_edge);

	TestGetEdges(graph);

	graph.RemoveEdge(1, 4);
	assert(!graph.HasEdge(1, 4));

	graph.RemoveVertex(4);
	assert(!graph.HasEdge(3, 4));

	graph.Clear();
	const auto new_size = graph.Size();
	assert(new_size.first == 0 && new_size.second == 0);
}

struct custom_set {};

template<class Key>
struct Brilliant::Graph::StorageGen<custom_set, Key> { using type = std::set<Key, std::greater<>, std::allocator<Key>>; };

int main()
{
	TestSetInterface();
	TestMapInterface();

	Brilliant::Graph::AdjacencyList<int> g1;
	TestAdjList(g1);

	//use unordered set instead of default std::set
	Brilliant::Graph::AdjacencyList<int, void, Brilliant::Graph::map, Brilliant::Graph::hash_set> g2;
	TestAdjList(g2);

	//use unordered_multiset and unordered_multimap for some reason
	Brilliant::Graph::AdjacencyList<int, void, Brilliant::Graph::hash_multimap, Brilliant::Graph::hash_multiset> g3;
	TestAdjList(g3);

	//use set with custom storage type
	Brilliant::Graph::AdjacencyList<int, void, Brilliant::Graph::map, custom_set> g4;
	TestAdjList(g4);

	Brilliant::Graph::AdjacencyList<int, int> g5;
	TestAdjList(g5);

	//use vector
	//Brilliant::Graph::AdjacencyList<int, void, Brilliant::Graph::map, Brilliant::Graph::vector> g5;
	//TestAdjList(g5);

	return 0;
}