#pragma once

#include <vector>

namespace cgv {
	namespace storage {
		namespace collection {
			/** the type free stack allows to push and pop elements of an arbitrary
			    type. The top most element can be queried with the top() method. */
			class type_free_stack
			{
			public:
				/// push a value of arbitrary type onto the stack
				template <typename T>
				void push(const T& v)
				{
					stack.resize(stack.size()+sizeof(T));
					(T&)*(stack.end()-sizeof(T)) = v;
				}
				/// query the top value of an arbitrary type as const reference
				template <typename T>
				const T& top() const { return (const T&)*(stack.end()-sizeof(T)); }
				/// query the top value of an arbitrary type as reference
				template <typename T>
				T& top() { return (T&)*(stack.end()-sizeof(T)); }
				/// pop the top value of an arbitrary type
				template <typename T>
				void pop() { stack.erase(stack.end()-sizeof(T), stack.end()); }
				/// return the size of the stack
				std::size_t size() const { return stack.size(); }
				/// return whether the stack is empty
				bool empty() const { return stack.empty(); }
			protected:
				/// store the values in a vector of char
				std::vector<char> stack;
			};
		}
	}
}