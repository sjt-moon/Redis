#pragma once
#include <memory>
#include <vector>

const int MAX_LEVEL = 4;

template <class T>
class SkiplistNode {
public:
	// Not recommended to use raw pointer, as once the raw pointer is invalid (either delete or out of scope),
	// this will be set to zero. Instead, Skiplist should take the ownership of this element.
	std::unique_ptr<T> obj;
	SkiplistNode<T> *backward;

	class SkiplistLevel {
	public:
		SkiplistNode<T> *forward;
		int span;
	};
	std::vector<SkiplistLevel> levels;

	SkiplistNode(std::unique_ptr<T> obj);
	static SkiplistNode<T> *Create(int level, std::unique_ptr<T> obj);
};

template <class T>
class Skiplist
{
public:
	SkiplistNode<T> *head;
	SkiplistNode<T> *tail;
	int(*compare)(const T *, const T *);
	std::string(*to_string)(const T *);
	int length;
	int level;

	static Skiplist<T> *Create(int(*compare)(const T *, const T *), std::string(*to_string)(const T *));

	SkiplistNode<T> *insert(std::unique_ptr<T> obj);

	bool remove(T* obj);

	void visualize();
	std::string visualize_str();
	// Visualize skiplist. block_width is for each level, including L%level and obj value. Each block has an arrow
	// pointing to the succeeder, where the arrow length is span length (of '-') + span value (centered number 
	// of length span_num_width filled with '-', e.g. --3---) + 1 (for '>'). 
	std::string visualize_str(int block_width, int span_num_width);

	std::vector<std::string> get_all();
};