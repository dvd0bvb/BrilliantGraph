#pragma once

#include <map>
#include <set>
#include <ranges>
#include <numeric>
#include <concepts>

#include <detail/AdjacencyListImpl.h>

namespace Brilliant
{
    namespace Graph
    {
        //adjacency list where edges have values
        template<class Vertex, class Edge = void,
            class VertexContainerTag = map,
            class EdgeContainerTag = std::conditional_t<std::is_same_v<Edge, void>, set, map>>
        class AdjacencyList 
            : public detail::AdjacencyListImpl<Vertex, Edge, 
                storage_gen_t<VertexContainerTag, Vertex, storage_gen_t<EdgeContainerTag, const Vertex*, Edge>>,
                storage_gen_t<EdgeContainerTag, const Vertex*, Edge>>
        {
            using base = detail::AdjacencyListImpl<Vertex, Edge,
                storage_gen_t<VertexContainerTag, Vertex, storage_gen_t<EdgeContainerTag, const Vertex*, Edge>>,
                storage_gen_t<EdgeContainerTag, const Vertex*, Edge>>;

        public:
            using vertex_type = typename base::vertex;
            using reference = typename base::reference;
            using pointer = typename base::pointer;
            using const_reference = typename base::const_reference;
            using const_pointer = typename base::const_pointer;

            using edge_type = typename base::edge;

            bool AddEdge(const_reference from, const_reference to)
            {
                if (auto found_from = this->list.find(from), found_to = this->list.find(to);
                    found_from != this->list.end() && found_to != this->list.end())
                {
                    found_from->second.emplace(&found_to->first, edge_type{});
                }
                return false;
            }

            bool AddEdge(const_reference from, const_reference to, edge_type edge)
            {
                if (auto found_from = this->list.find(from), found_to = this->list.find(to);
                    found_from != this->list.end() && found_to != this->list.end())
                {
                    found_from->second.emplace(&found_to->first, edge);
                }
                return false;
            }

            template<class... Args>
            bool AddEdge(const_reference from, const_reference to, Args&&... args)
            {
                if (auto found_from = this->list.find(from), found_to = this->list.find(to);
                    found_from != this->list.end() && found_to != this->list.end())
                {
                    found_from->second.try_emplace(&found_to->first, std::forward<Args>(args)...);
                }
                return false;
            }
        };

        //adjacency list where edges are binary - they exist or they dont
        template<class Vertex, class VertexContainerTag, class EdgeContainerTag>
        class AdjacencyList<Vertex, void, VertexContainerTag, EdgeContainerTag> 
            : public detail::AdjacencyListImpl<Vertex, void,
                            storage_gen_t<VertexContainerTag, Vertex, storage_gen_t<EdgeContainerTag, const Vertex*>>, 
                            storage_gen_t<EdgeContainerTag, const Vertex*>>
        {
            using base = detail::AdjacencyListImpl<Vertex, void,
                storage_gen_t<VertexContainerTag, Vertex, storage_gen_t<EdgeContainerTag, const Vertex*>>,
                storage_gen_t<EdgeContainerTag, const Vertex*>>;

        public:
            using vertex_type = typename base::vertex;
            using reference = typename base::reference;
            using pointer = typename base::pointer;
            using const_reference = typename base::const_reference;
            using const_pointer = typename base::const_pointer;

            using edge_type = typename base::edge;

            bool AddEdge(const_reference from, const_reference to)
            {
                if (auto found_from = this->list.find(from), found_to = this->list.find(to);
                    found_from != this->list.end() && found_to != this->list.end())
                {
                    //TODO: Add functions to customize insertion based on container type
                    found_from->second.insert(&found_to->first);
                    return true;
                }
                return false;
            }
        };
    }
}