#pragma once

#include <concepts>
#include <ranges>
#include <type_traits>

namespace Brilliant
{
    namespace Graph
    {
        namespace detail
        {
            template<class, class = void>
            struct has_set_interface : std::false_type {};

            template<class Container>
            struct has_set_interface<Container,
                std::void_t<typename Container::key_type, typename Container::value_type>>
                : std::bool_constant<std::is_same_v<typename Container::key_type, typename Container::value_type>> {};

            template<class Container>
            inline constexpr auto has_set_interface_v = has_set_interface<Container>::value;

            template<class Container>
            concept SetInterface = std::ranges::range<Container> && has_set_interface_v<Container>;
        }
    }
}