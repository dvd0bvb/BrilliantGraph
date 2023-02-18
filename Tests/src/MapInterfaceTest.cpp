#include <gtest/gtest.h>
#include <detail/MapInterface.h>


TEST(MapInterfaceTest, Test)
{
    using namespace Brilliant::Graph;
	
	static_assert(detail::MapInterface<std::map<int, int>>);
	static_assert(detail::MapInterface<std::unordered_map<int, int>>);
	static_assert(detail::MapInterface<std::multimap<int, int>>);
	static_assert(detail::MapInterface<std::unordered_multimap<int, int>>);

	static_assert(!detail::MapInterface<std::vector<int>>);
	static_assert(!detail::MapInterface<std::set<int>>);
}