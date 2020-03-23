// ConsoleApplication1.cpp : 定义控制台应用程序的入口点。
//

#define _CRTDBG_MAP_ALLOC

#include "stdafx.h"
#include "util/data_structure/skiplist.h"
#include "util/data_structure/skiplist.cpp"
#include <iostream>
#include <memory>
#include <string>
#include <stdlib.h>
#include <crtdbg.h>

struct A {
	std::unique_ptr<std::string> s;
	A * p;
};

int main()
{
	{
		std::cout << "Redis" << std::endl;
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
		skiplist->visualize();
		std::string s = "good";
		std::cout << "remove " << s << ": " << skiplist->remove(&s) << std::endl;
		skiplist->visualize();
		/*
		std::string t = "bad";
		std::cout << "remove " << t << ": " << skiplist->remove(&t) << std::endl;
		skiplist->visualize();
		*/
		getchar();
		skiplist->remove();
		delete skiplist;
	}
	_CrtDumpMemoryLeaks();
	return 0;
}