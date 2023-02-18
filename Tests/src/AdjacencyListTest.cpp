#include <gtest/gtest.h>
#include <AdjacencyList.h>

using namespace Brilliant::Graph;

template class AdjacencyList<int>;

TEST(AdjacencyListTest, AddVertexTest)
{
    AdjacencyList<int> graph;
    graph.AddVertex(1);
    ASSERT_TRUE(graph.HasVertex(1));   
}

TEST(AdjacencyListTest, EmplaceVertexTest)
{
    struct test_struct
    {
        explicit test_struct(int i) : _i(i) {}
        auto operator<=> (const test_struct&) const = default;
        int _i;
    };

    AdjacencyList<test_struct> graph;
    graph.EmplaceVertex(1);
    ASSERT_TRUE(graph.HasVertex(test_struct(1)));
}

TEST(AdjacencyListTest, AddEdgeTest)
{
    AdjacencyList<int> graph;
    graph.AddVertex(0);
    graph.AddVertex(1);
    graph.AddEdge(0, 1);
    ASSERT_TRUE(graph.HasEdge(0, 1));
    ASSERT_FALSE(graph.HasEdge(1, 0));

    AdjacencyList<int, int> graph1;
    graph1.AddVertex(0);
    graph1.AddVertex(1);
    graph1.AddEdge(0, 1, 3);
    ASSERT_TRUE(graph1.HasEdge(0, 1));
    ASSERT_FALSE(graph1.HasEdge(1, 0));
    
    auto edges = graph1.GetEdges(0);
    auto e1 = std::ranges::find_if(edges, [](auto& e){
        return e.second == 3;
    });
    ASSERT_EQ(3, e1->second);
}