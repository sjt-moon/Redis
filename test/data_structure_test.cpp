#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Redis/util/data_structure/skiplist.h"
#include "../Redis/util/data_structure/skiplist.cpp"
#include "../Redis/util/view/view.h"
#include "../Redis/util/view/view.cpp"
#include <iomanip>
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{		
	TEST_CLASS(SkiplistTest)
	{
	public:

		static Skiplist<std::string> *get_string_skiplist() {
			Skiplist<std::string> *skiplist = Skiplist<std::string>::Create(
				[](const std::string *a, const std::string *b) {
				std::string x = *a;
				return (*a).compare(*b);
			},
				[](const std::string *s) { return *s; }
			);
			std::vector<std::string> inserted_strings = { "bar", "coffee", "next", "good", "fantastic", "lol" };
			for (auto s : inserted_strings) {
				skiplist->insert(std::move(std::make_unique<std::string>(s)));
			}
			return skiplist;
		}
		
		TEST_METHOD(Insertion)
		{
			Skiplist<std::string> *skiplist = get_string_skiplist();
			std::string expected_visualization_str =
				"|    L3    |---2--->|  coffee  |-->   NULL   \n"
				"|    L2    |---2--->|  coffee  |----4---->|   next   |-->   NULL   \n"
				"|    L1    |--1--->|   bar    |--1--->|  coffee  |--1--->|fantastic |---3---->|   next   |-->   NULL   \n"
				"|    L0    |--1--->|   bar    |--1--->|  coffee  |--1--->|fantastic |--1--->|   good   |--1--->|   lol    |--1--->|   next   |-->   NULL   \n"
				"|   BACK   |<--b---|   bar    |<--b---|  coffee  |<--b---|fantastic |<--b---|   good   |<--b---|   lol    |<--b---|   next   |\n";
			Assert::AreEqual(expected_visualization_str, skiplist->visualize_str());
		}

		TEST_METHOD(removal)
		{
			Skiplist<std::string> *skiplist = get_string_skiplist();
			std::string original_visualization_str =
				"|    L3    |---2--->|  coffee  |-->   NULL   \n"
				"|    L2    |---2--->|  coffee  |----4---->|   next   |-->   NULL   \n"
				"|    L1    |--1--->|   bar    |--1--->|  coffee  |--1--->|fantastic |---3---->|   next   |-->   NULL   \n"
				"|    L0    |--1--->|   bar    |--1--->|  coffee  |--1--->|fantastic |--1--->|   good   |--1--->|   lol    |--1--->|   next   |-->   NULL   \n"
				"|   BACK   |<--b---|   bar    |<--b---|  coffee  |<--b---|fantastic |<--b---|   good   |<--b---|   lol    |<--b---|   next   |\n";
			Assert::AreEqual(original_visualization_str, skiplist->visualize_str());

			std::string s = "bad";
			Assert::IsFalse(skiplist->remove(&s));
			std::string t = "good";
			Assert::IsTrue(skiplist->remove(&t));
			Assert::AreEqual(5, skiplist->length);
			std::string expected_visualization_str =
				"|    L3    |---2--->|  coffee  |-->   NULL   \n"
				"|    L2    |---2--->|  coffee  |---3---->|   next   |-->   NULL   \n"
				"|    L1    |--1--->|   bar    |--1--->|  coffee  |--1--->|fantastic |---2--->|   next   |-->   NULL   \n"
				"|    L0    |--1--->|   bar    |--1--->|  coffee  |--1--->|fantastic |--1--->|   lol    |--1--->|   next   |-->   NULL   \n"
				"|   BACK   |<--b---|   bar    |<--b---|  coffee  |<--b---|fantastic |<--b---|   lol    |<--b---|   next   |\n";
			Assert::AreEqual(expected_visualization_str, skiplist->visualize_str());
		}
	};
}