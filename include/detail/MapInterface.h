#pragma once

#include <ranges>
#include <concepts>
#include <type_traits>

namespace Brilliant
{
    namespace Graph
    {
        namespace detail
        {
            template<class, class = void>
            struct has_map_interface : std::false_type {};

            template<class Container> 
            struct has_map_interface<Container, 
                std::void_t<typename Container::key_type, typename Container::mapped_type>> : std::true_type{};

            template<class Container>
            inline constexpr auto has_map_interface_v = has_map_interface<Container>::value;

            template<class Container>
            concept MapInterface = std::ranges::range<Container> && has_map_interface_v<Container>;
        }
    }
}