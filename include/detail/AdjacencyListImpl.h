#pragma once

#include <StorageGen.h>
#include <detail/MapInterface.h>

namespace Brilliant
{
    namespace Graph
    {
        namespace detail
        {
            //Compile time check for correct container types
            //compiler output should be easier to parse if incorrect container given to AdjacencyList template args
            template<class Edge, class EdgeContainer>
            concept ValidEdgeContainer = requires 
            { 
                //if void Edge type, container just needs to be used like a range
                (std::is_same_v<Edge, void> && std::ranges::range<EdgeContainer>) ||

                //if Edge type is not void, the container should be able to be used like a map
                (!std::is_same_v<Edge, void> && has_map_interface_v<EdgeContainer>);
            };

            template<class Vertex, class Edge, MapInterface VertexContainer, ValidEdgeContainer<Edge> EdgeContainer>
            class AdjacencyListImpl
            {
            public:
                using vertex = Vertex;
                using reference = vertex&;
                using pointer = vertex*;
                using const_reference = const vertex&;
                using const_pointer = const vertex*;

                using edge = Edge;

                using edge_set = EdgeContainer;
                using adjacency_list = VertexContainer;

                auto& AddVertex(const_reference vert)
                {
                    auto result = list.emplace(vert, edge_set{});
                    if constexpr (std::is_same_v<adjacency_list::iterator, std::decay_t<decltype(result)>>)
                    {
                        return *result;
                    }
                    else //this is the case of multimap style containers
                    {
                        return *result.first;
                    }
                }

                template<class... Args>
                auto& EmplaceVertex(Args&&... args)
                {
                    auto result = list.emplace(std::piecewise_construct, std::forward_as_tuple(args...), std::forward_as_tuple(edge_set{}));
                    if constexpr (std::is_same_v<adjacency_list::iterator, std::decay_t<decltype(result)>>)
                    {
                        return *result;
                    }
                    else //this is the case of multimap style containers
                    {
                        return *result.first;
                    }
                }

                bool RemoveVertex(const_reference vert)
                {
                    if (auto found = list.find(vert); found != list.end())
                    {
                        RemoveVertex(found);
                        return true;
                    }
                    return false;
                }

                void Clear() noexcept
                {
                    list.clear();
                }

                auto NumVertices() const noexcept
                {
                    return list.size();
                }

                auto NumEdges() const noexcept
                {
                    const auto edges_view = list | std::views::values;
                    return std::accumulate(std::ranges::begin(edges_view), std::ranges::end(edges_view), std::size_t{ 0 }, [](std::size_t i, auto& edges) {
                        return i + edges.size();
                        });
                }

                auto Size() const noexcept
                {
                    return std::make_pair(NumVertices(), NumEdges());
                }

                void RemoveEdge(const_reference from, const_reference to)
                {
                    if (auto found_from = list.find(from), found_to = list.find(to);
                        found_from != list.end() && found_to != list.end())
                    {
                        RemoveEdge(found_from, found_to);
                    }
                }

                void RemoveAllEdges(const_reference vert)
                {
                    if (auto found = list.find(vert); found != list.end())
                    {
                        RemoveAllEdges(found);
                    }
                }

                bool HasEdge(const_reference from, const_reference to)
                {
                    if (auto found_from = this->list.find(from), found_to = this->list.find(to);
                        found_from != this->list.end() && found_to != this->list.end())
                    {
                        return FindInEdges(found_from, found_to) != found_from->second.end();
                    }
                    return false;
                }

                //TODO: make non-const versions, test mutability of returned views
                auto GetVertices() const
                {
                    return list | std::views::keys;
                }

                auto GetEdges(const_reference vert) const
                {
                    auto range = list.equal_range(vert);
                    //values_view gives us a range<edge_list>
                    //join_view flattens the values_view and gives us range<const_pointer> which is what we want
                    return std::ranges::subrange(range.first, range.second) | std::views::values | std::views::join;
                }

                auto GetAllEdges() const
                {
                    return list | std::views::values | std::views::join;
                }

            private:
                void RemoveEdge(adjacency_list::iterator from, adjacency_list::iterator to)
                {
                    if (auto found = FindInEdges(from, to); found != from->second.end())
                    {
                        from->second.erase(found);
                    }
                }

                void RemoveAllEdges(adjacency_list::iterator vert)
                {
                    vert->second.clear();
                }

                void RemoveVertex(adjacency_list::iterator vert)
                {
                    //find all other vertices such that they have edges to vert and erase vert
                    for (auto iter = list.begin(), end = list.end(); iter != end; ++iter)
                    {
                        if (iter != vert)
                        {
                            //what if edge_set is a multiset/multimap?
                            if (auto found = FindInEdges(iter, vert); found != iter->second.end())
                            {
                                iter->second.erase(found);
                            }
                        }
                    }

                    list.erase(vert);
                }

                auto FindInEdges(adjacency_list::iterator from, adjacency_list::iterator to_find)
                {
                    //for some reason std::ranges::find doesn't like when edge_set is a mappish type
                    //assuming it expects a pair for the search object?
                    if constexpr (detail::has_map_interface_v<edge_set>) 
                    {
                        return from->second.find(&to_find->first);
                    }
                    else
                    {
                        return std::ranges::find(from->second, &to_find->first);
                    }
                }

            protected:
                adjacency_list list;
            };
        }
    }
}