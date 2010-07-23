#pragma once

namespace Utilities
{
	template <class Node>
	struct node_wrap
	{
		Node* ptr;

		node_wrap(Node* p = 0) : ptr(p) {}

		Node& operator*() const { return *ptr; }
		Node* operator->() const { return ptr; }

		node_wrap& operator++() { ptr = ptr->next; return *this; }
		node_wrap operator++(int) { node_wrap tmp = *this; ++*this; return tmp; }

		bool operator==(const node_wrap& i) const { return ptr == i.ptr; }
		bool operator!=(const node_wrap& i) const { return ptr != i.ptr; }
	};
}