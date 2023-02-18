#include <set>
#include <unordered_set>
#include <map>
#include <unordered_set>
#include <gtest/gtest.h>
#include <detail/SetInterface.h>

TEST(SetInterfaceTest, Test)
{
    using namespace Brilliant::Graph;

	static_assert(detail::SetInterface<std::set<int>>);
	static_assert(detail::SetInterface<std::unordered_set<int>>);
	static_assert(detail::SetInterface<std::multiset<int>>);
	static_assert(detail::SetInterface<std::unordered_multiset<int>>);

	static_assert(!detail::SetInterface<std::vector<int>>);
	static_assert(!detail::SetInterface<std::map<int, int>>);
}