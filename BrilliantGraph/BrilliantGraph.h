#pragma once

#include <algorithm>
#include <cassert>
#include <ranges>
#include <set>
#include <stack>
#include <queue>
#include <unordered_set>
#include <unordered_map>

namespace BrilliantGraph
{
	//vertex and edgetype require objects to be comparable
	//edgetype must have operator+ defined

	//Graph uses an unordered multimap to implement an adjacency list
	//Vertices are stored in sVertices and references to the vertices are stored in the map and edges to save memory
	template<class Vertex, class EdgeType>
	class DirectedGraph
	{
		struct Edge;
		
	public:
		using vert_t = Vertex;
		using vert_ref_t = std::reference_wrapper<const vert_t>;
		using edge_weight_t = EdgeType;
		using edge_t = Edge;

	private:
		struct Edge
		{
			vert_ref_t tNode;
			edge_weight_t uWeight;
		};		

	public:
		//copy vertex to graph
		auto AddVertex(const vert_t& v)
		{
			return sVertices.insert(v);
		}

		//emplace vertex in graph
		template<class... Ts>
		auto EmplaceVertex(Ts&&... args)
		{
			return sVertices.emplace(std::forward<Ts>(args)...);
		}

		[[nodiscard]] auto& GetVertices() const noexcept { return sVertices; }

		//map vFrom to edge with vTo and weight ew
		//add vertices and retry if vFrom or vTo are not in list of vertices
		void AddEdge(const vert_t& vFrom, const vert_t& vTo, const edge_weight_t& ew)
		{	
			if (auto foundFrom = std::ranges::find(sVertices, vFrom); foundFrom != std::ranges::end(sVertices))
			{
				if (auto foundTo = std::ranges::find(sVertices, vTo); foundTo != std::ranges::end(sVertices))
				{
					mGraphMap.emplace(std::cref(*foundFrom), edge_t{ std::cref(*foundTo), ew });
				}
				else
				{
					AddVertex(vTo);
					AddEdge(vFrom, vTo, ew);
				}
			}
			else
			{
				AddVertex(vFrom);
				AddEdge(vFrom, vTo, ew);
			}
		}

		auto GetEdges(const vert_t& v) const
		{
			auto pEdges = mGraphMap.equal_range(std::cref(v));
			return std::views::all(std::ranges::subrange(pEdges.first, pEdges.second));
			//std::views::values(...) //not implemented yet. use to get view with only edges
		}

	private: 
		std::unordered_multimap<vert_ref_t, edge_t, std::hash<vert_t>> mGraphMap;
		std::unordered_set<vert_t> sVertices;
	};

	//graph traversal implementations
	struct DepthFirstSearch
	{
		template<class Graph>
		[[nodiscard]] auto operator() (const Graph& graph, const typename Graph::vert_t& vFrom, const typename Graph::vert_t& vTo) const
		{
			assert(graph.GetVertices().find(vFrom) != graph.GetVertices().end()); //ok if we don't find vTo, return stack will be empty

			std::set<typename Graph::vert_ref_t> sExplored;
			std::stack<typename Graph::edge_t> sRet;
			(*this)(graph, vFrom, vTo, sExplored, sRet);
			return sRet;
		}

	private:
		template<class Graph>
		bool operator() (const Graph& graph, const typename Graph::vert_t& vFrom, const typename Graph::vert_t& vTo, std::set<typename Graph::vert_ref_t>& sExplored, std::stack<typename Graph::edge_t>& sRet) const
		{
			if (sExplored.insert(vFrom).second)
			{
				for (auto& [vert, edge] : graph.GetEdges(vFrom))
				{
					if (sExplored.find(edge.tNode) == sExplored.end() && 
						(edge.tNode == vTo || (*this)(graph, edge.tNode.get(), vTo, sExplored, sRet)))
					{
						sRet.push(edge);
						return true;
					}
				}
			}
			return false;
		}
	};

	struct BreadthFirstSearch
	{
		template<class Graph>
		[[nodiscard]] auto operator() (const Graph& graph, const typename Graph::vert_t& vFrom, const typename Graph::vert_t& vTo) const
		{
			assert(graph.GetVertices().find(vFrom) != graph.GetVertices().end());

			std::set<typename Graph::vert_ref_t> sExplored;
			std::stack<typename Graph::edge_t> sRet;
			(*this)(graph, vFrom, vTo, sExplored, sRet);
			return sRet;
		}

	private:
		template<class Graph>
		bool operator() (const Graph& graph, const typename Graph::vert_t& vFrom, const typename Graph::vert_t& vTo, std::set<typename Graph::vert_ref_t>& sExplored, std::stack<typename Graph::edge_t>& sRet) const
		{
			auto pInsert = sExplored.insert(vFrom);
			if (pInsert.second)
			{
				std::queue<typename Graph::edge_t> qEdges;
				for (auto& [vert, edge] : graph.GetEdges(vFrom))
				{
					if (edge.tNode == vTo)
					{
						sRet.push(edge);
						return true;
					}
					qEdges.push(edge);
				}

				while (!qEdges.empty())
				{
					auto& vCurr = qEdges.front();
					qEdges.pop();
					if ((*this)(graph, vCurr.tNode.get(), vTo, sExplored, sRet))
					{
						sRet.push(vCurr);
						return true;
					}
				}

				sExplored.erase(pInsert.first);
			}
		}
	};

	struct Dijkstras
	{
		template<class Graph> 
		requires std::invocable<std::plus<>, typename Graph::edge_weight_t, typename Graph::edge_weight_t> //need edge weights to be summable 
		[[nodiscard]] auto operator() (const Graph& graph, const typename Graph::vert_t& vFrom, const typename Graph::vert_t& vTo) const 
		{
			using vert_ref_t = typename Graph::vert_ref_t;
			using vert_t = typename Graph::vert_t;
			using edge_t = typename Graph::edge_t;
			using edge_weight_t = typename Graph::edge_weight_t;
			using q_t = std::pair<edge_weight_t, vert_ref_t>;

			assert(graph.GetVertices().find(vFrom) != graph.GetVertices().end());
			
			std::priority_queue<q_t, std::vector<q_t>, std::greater<q_t>> qQ;
			std::unordered_map<vert_ref_t, edge_weight_t, std::hash<vert_t>> mDistance;
			std::unordered_map<vert_ref_t, edge_t, std::hash<vert_t>> mPrev;

			for (auto& vert : graph.GetVertices())
			{
				mDistance.emplace(vert, std::numeric_limits<edge_weight_t>::max());
			}

			mDistance.at(vFrom) = std::numeric_limits<edge_weight_t>::min();
			qQ.emplace(mDistance.at(vFrom), vFrom);

			while (!qQ.empty())
			{
				auto current = qQ.top();
				qQ.pop();

				for (auto& [vert, edge] : graph.GetEdges(current.second))
				{
					if (mDistance.at(current.second) + edge.uWeight < mDistance.at(edge.tNode))
					{
						mDistance.at(edge.tNode) = mDistance.at(current.second) + edge.uWeight;
						qQ.emplace(mDistance.at(edge.tNode), edge.tNode);
						if (!mPrev.emplace(current.second, edge).second)
						{
							mPrev.at(current.second) = edge;
						}
					}
				}
			}

			std::stack<edge_t> sRet;
			auto vert = std::cref(vFrom);
			while (mPrev.count(vert))
			{
				sRet.push(mPrev.at(vert));
				vert = mPrev.at(vert).tNode;
			}

			return sRet;
		}
	};

	struct AStar
	{

	};

	template<class Graph, class SearchImpl = AStar> 
	[[nodiscard]] auto FindPath(const Graph& graph, const typename Graph::vert_t& vFrom, const typename Graph::vert_t& vTo, SearchImpl&& impl = SearchImpl{}) noexcept
	{
		return impl(graph, vFrom, vTo);
	}
}