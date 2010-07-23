#pragma once

#include <cgv/gui/group.h>

namespace cgv {
	namespace gui {

/// gui element iterator with callbacks to check, whether element is valid iterator position and whether iterator should descent into elements of group 
struct iterator
{
	std::stack<group_ptr> group_stack;
	std::stack<int> index_stack;
	bool (*is_valid)(element_ptr);
	bool (*descent)(group_ptr);

	iterator(group_ptr g, 
			 bool (*_is_valid)(element_ptr), 
			 bool (*_descent)(group_ptr))
		: is_valid(_is_valid), descent(_descent)
	{
		group_stack.push(g);
		index_stack.push(-1);
		if (is_valid(element_ptr(g)))
			return;
		goto_next();
	}
	element_ptr operator *() const
	{
		if (group_stack.empty())
			return element_ptr();
		int i = index_stack.top();
		return i == -1 ? element_ptr(group_stack.top().ref()) : 
			element_ptr(group_stack.top()->get_child(i).ref());
	}
	bool at_end() const
	{
		return group_stack.empty();
	}
	bool goto_next()
	{
		if (at_end())
			return false;

		// update stacks
		element_ptr e = *(*this);
		group_ptr g = group_ptr(dynamic_cast<group*>(e.ref()));
		if (!g.empty() && descent(g)) {
			group_stack.push(g);
			index_stack.push(-1);
		}
		else if (index_stack.top() == -1) {
			group_stack.pop();
			index_stack.pop();
		}

		while (!at_end()) {
			for (++index_stack.top(); index_stack.top() < (int)group_stack.top()->get_nr_children(); ++index_stack.top()) {
				e = element_ptr(group_stack.top()->get_child(index_stack.top()));
				if (is_valid(e))
					return true;

				group_ptr s = group_ptr(dynamic_cast<group*>(e.ref()));
				if (!s.empty() && descent(s)) {
					group_stack.push(s);
					index_stack.push(-1);
				}
			}
			group_stack.pop();
			index_stack.pop();
		}
		return false;
	}
};

	}
}