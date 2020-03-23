#include "../../stdafx.h"
#include "skiplist.h"
#include <iostream>
#include <iomanip>
#include "../view/view.h"

template<class T>
SkiplistNode<T>::SkiplistNode(std::unique_ptr<T> obj)
{
	this->obj = std::move(obj);
	this->backward = NULL;
	this->levels = std::vector<SkiplistLevel>();
}

template<class T>
SkiplistNode<T>* SkiplistNode<T>::Create(int level, std::unique_ptr<T> obj)
{
	SkiplistNode<T> *node = new SkiplistNode(std::move(obj));
	for (int i = 0; i < level; ++i) {
		SkiplistLevel skiplist_level = SkiplistLevel();
		skiplist_level.forward = NULL;
		skiplist_level.span = 0;
		node->levels.push_back(skiplist_level);
	}
	node->backward = NULL;
	return node;
}

template<class T>
void SkiplistNode<T>::remove(SkiplistNode<T>* node)
{
	if (node) {
		// Delete a deleted obj is undefined behaviour. When removing a node,
		// its forward ptrs and backward may already be deleted. It's Skiplist level
		// method responsibility to choose the order of removal and record the
		// next node to be removed. Thus, making forward and backward ptrs as
		// nullptr to avoid double-removal.
		for (auto skiplist_level : node->levels) {
			skiplist_level.forward = nullptr;
		}
		node->backward = nullptr;
		delete node;
	}
}

template<class T>
Skiplist<T>* Skiplist<T>::Create(int(*compare)(const T *, const T *), std::string(*to_string)(const T *))
{
	Skiplist<T> *skiplist = new Skiplist<T>();
	skiplist->head = SkiplistNode<T>::Create(1, NULL);
	skiplist->tail = NULL;
	skiplist->compare = compare;
	skiplist->to_string = to_string;
	skiplist->length = 0;
	skiplist->level = 1;
	return skiplist;
}

template<class T>
SkiplistNode<T>* Skiplist<T>::insert(std::unique_ptr<T> obj)
{
	// Insert new node after update[i] for each level i.
	SkiplistNode<T> *update[MAX_LEVEL];

	// Spans between head to update[i] for each level i.
	int spans[MAX_LEVEL];
	for (int i = 0; i < MAX_LEVEL; ++i) {
		update[i] = NULL;
		spans[i] = 0;
	}

	// Update spans array in 1 iteration. Lower levels can init spans from the upper level.
	SkiplistNode<T> *p = this->head;
	for (int i = this->level - 1; i >= 0; --i) {
		spans[i] = i == this->level - 1 ? 0 : spans[i + 1];
		while (p->levels[i].forward && this->compare(p->levels[i].forward->obj.get(), obj.get()) < 0) {
			spans[i] += p->levels[i].span;
			p = p->levels[i].forward;
		}
		update[i] = p;
	}

	// If obj exists, return null.
	if (update[0]->levels[0].forward && this->compare(update[0]->levels[0].forward->obj.get(), obj.get()) == 0) {
		return NULL;
	}

	// Insert a new node with random levels.
	int level = rand() % MAX_LEVEL + 1;
	if (level > this->level) {
		for (int i = this->level; i < level; ++i) {
			update[i] = this->head;
			SkiplistNode<T>::SkiplistLevel skiplist_level = SkiplistNode<T>::SkiplistLevel();
			skiplist_level.forward = NULL;
			skiplist_level.span = this->length;
			update[i]->levels.push_back(skiplist_level);
		}
		this->level = level;
	}
	SkiplistNode<T> *inserted_node = SkiplistNode<T>::Create(level, std::move(obj));
	for (int i = level - 1; i >= 0; --i) {
		inserted_node->levels[i].forward = update[i]->levels[i].forward;
		update[i]->levels[i].forward = inserted_node;

		inserted_node->levels[i].span = update[i]->levels[i].span - (spans[0] - spans[i]);
		update[i]->levels[i].span = spans[0] - spans[i] + 1;
	}

	// Increase spans for untouched levels.
	for (int i = level; i < this->level; ++i) {
		update[i]->levels[i].span++;
	}

	inserted_node->backward = update[0];
	if (inserted_node->levels[0].forward) {
		(inserted_node->levels[0].forward)->backward = inserted_node;
	}
	else {
		this->tail = inserted_node;
	}

	this->length++;
	return inserted_node;
}

template<class T>
SkiplistNode<T> *Skiplist<T>::find(T *obj) {
	SkiplistNode<T> *update[MAX_LEVEL];
	int spans[MAX_LEVEL];
	for (int i = 0; i < MAX_LEVEL; ++i) {
		update[i] = NULL;
		spans[i] = 0;
	}

	SkiplistNode<T> *p = this->head;
	for (int i = this->level - 1; i >= 0; --i) {
		// p MUST have levels i because if p->levels[i].forward exists, then the succeedor MUST at least have level i.
		if (p->levels[i].forward && this->compare(p->levels[i].forward->obj.get(), obj) < 0) {
			spans[i] += p->levels[i].span;
			p = p->levels[i].forward;
		}
		update[i] = p;
	}

	// If obj does not exist, return nullptr.
	if (!p->levels[0].forward || this->compare(p->levels[0].forward->obj.get(), obj) != 0) {
		return nullptr;
	}

	// update[i] maynot be the predecessor to found_node at level i, so found_node may don't have levels i.
	SkiplistNode<T> *found_node = p->levels[0].forward;
	for (int i = this->level - 1; i >= 0; --i) {
		// If found_node appears in this level, merge intervals.
		if (i < (int)found_node->levels.size()) {
			update[i]->levels[i].forward = found_node->levels[i].forward;
			update[i]->levels[i].span += found_node->levels[i].span - 1;
		}
		// Else if found_node is included in this invertal at this level, but does not exist in this level, decrease level span.
		else if (update[i]->levels[i].forward) {
			update[i]->levels[i].span--;
		}
	}
	if (found_node->levels[0].forward) {
		found_node->levels[0].forward->backward = update[0];
	}
	else {
		this->tail = update[0];
	}
	this->length--;
	for (int i = MAX_LEVEL - 1; i >= 0; --i) {
		if (this->head->levels[i].forward) {
			this->level = i + 1;
			break;
		}
	}
	return found_node;
}

template<class T>
bool Skiplist<T>::remove(T * obj)
{
	SkiplistNode<T> *p = this->find(obj);
	if (p) {
		SkiplistNode<T>::remove(p);
		return true;
	}
	return false;
}

template<class T>
bool Skiplist<T>::remove()
{
	SkiplistNode<T> *p = this->tail;
	// head is removed via the 1st node.backward ptr, tail is removed as well. After while-loop,
	// both head and tail allocated memory are removed.
	while (p) {
		SkiplistNode<T> *prev = p->backward;
		SkiplistNode<T>::remove(p);
		p = prev;
	}
	return true;
}

template<class T>
void Skiplist<T>::visualize()
{
	std::cout << this->visualize_str() << std::endl;
}

template<class T>
std::string Skiplist<T>::visualize_str()
{
	return this->visualize_str(10, 5);
}

template<class T>
std::string Skiplist<T>::visualize_str(int block_width, int span_num_width)
{
	std::string visualization_output = "";
	SkiplistNode<T> *p = this->head;
	for (int i = MAX_LEVEL - 1; i >= 0; --i) {
		// head
		visualization_output += view::centered_str(block_width, "L" + std::to_string(i), ' ', "|");
		p = this->head;
		while (i < (int)p->levels.size() && p->levels[i].forward) {
			visualization_output += view::centered_str(p->levels[i].span + span_num_width, std::to_string(p->levels[i].span), '-') + ">";
			visualization_output += view::centered_str(block_width, this->to_string(p->levels[i].forward->obj.get()), ' ', "|");
			p = p->levels[i].forward;
		}
		visualization_output += "-->" + view::centered_str(block_width, "NULL") + "\n";
	}
	// tail backward
	std::string backward_str = "\n";
	p = this->tail;
	while (p && p != this->head) {
		backward_str = "<--b---" + view::centered_str(block_width, this->to_string(p->obj.get()), ' ', "|") + backward_str;
		p = p->backward;
	}
	backward_str = view::centered_str(block_width, "BACK", ' ', "|") + backward_str;
	return visualization_output + backward_str;
}

template<class T>
std::vector<std::string> Skiplist<T>::get_all()
{
	std::vector<std::string> list;
	SkiplistNode<T> *p = this->head;
	while (p->levels[0].forward) {
		list.push_back(this->to_string(p->levels[0].forward->obj.get()));
		std::cout << list.back() << std::endl;
		p = p->levels[0].forward;
	}
	return list;
}
