#pragma once

#include <algorithm>
#include <cgv/type/size_type.h>
#include <cgv/type/index_type.h>
#include <cgv/range/RandomAccessRange.h>
#include <cgv/range/expression/assign_expression.h>
#include <cgv/range/expression/random_access_expression.h>

namespace cgv {
	namespace storage {
		namespace collection {
			namespace Array {
				using namespace type;
				template <typename T>
				struct runtime_array
				{
					uint32 extent;
					T *elements;
					/// construct from number of elements
					runtime_array(uint32 n = 0) 
					{ 
						extent = 0; 
						elements = 0; 
						if (extent > 0)
							resize(extent); 
					}
					~runtime_array() 
					{ 
						clear(); 
					}
					/// return the size of the vector
					uint32 size() 
					{
						return extent; 
					}
					void clear()
					{
						if (elements)
							delete [] elements;
						elements = 0;
					}
					void resize(uint32 n) 
					{ 
						clear();
						extent   = n;
						elements = new T[n];
					}
				};

				template <typename T, uint32 N>
				struct compile_time_array
				{
					compile_time_array() {}
					T elements[N];
					static uint32 size() { return extent; }
					static const uint32 extent = N;
				};
				template <typename T, type::uint32 N>
				struct traits
				{
					/// nothing to be done for resize
					typedef typename compile_time_array<T,N> base;
					template <typename X>
					static void resize(base& p, const X& n)
					{
					}
					/// nothing has to be done before an assignment
					template <typename X>
					struct prepare
					{
						static void assign(base& b, const X& r) { }
					};
				};

				template <typename T>
				struct traits<T,0>
				{
					typedef typename runtime_array<T> base;
					template <typename X>
					static void resize(base& b, const X& n)
					{
						b.resize((type::uint32)n);
					}
					/// in case of assignments to ranges, resize if smaller than to be assigned range
					template <typename X, bool X_is_range>
					struct prepare_select
					{
						static void assign(base& b, const X& r)
						{
							if (r.size() > 0 && b.extent != r.size())
								b.resize(r.size());
						}
					};
					/// in case of assignments to something else ensure that at least one element is present to hold the value
					template <typename X>
					struct prepare_select<X,false>
					{
						static void assign(base& b, const X& v) 
						{ 
							if (b.extent < 1)
								b.resize(1);
						}
					};
					/// resize the vector for rhs of assignment operator
					template <typename X>
					struct prepare : public prepare_select<X, 
						cond::or<typename range::ForwardRange::traits<X>::implements,
							     typename range::expression::Expression::traits<X>::implements>::value> {	};
				};
			}

/** an array keeps a number of elements that can be fixed at compile time or during 
    runtime, what depends on the given template arguments:
	
	array<double,3> ... three elements fixed at compile-time
	array<double> a(n) ... n elements at runtime

	The array implements the cgv::range::RandomAccessRange concept and therefore 
	sees all operators in cgv/range/operators.h. With that include you can add, 
	subtract, multiply by component and lots more.
	
	The assignment and the () operator is overloaded to support expression
	templates and thus you can write for example:

	a = r*sin(2*Pi*_i);

	or

	s = _i * _i;

	where _i is defined in cgv/range/expression/index_expression.h. Finally,
	the assignment operator is overload to provide also comma assignment:

	a = 1, 7, 3;

	*/
template <typename ta_element_type, cgv::type::uint32 ta_extent = 0>
class array : 
	public Array::traits<ta_element_type, ta_extent>::base,
	public cgv::type::concepts<cgv::range::RandomAccessRange::tag>,
	public cgv::range::RandomAccessRange::check<array<ta_element_type,ta_extent> > // tag the array as a random access range and check concept
{
public:
	/// type of the elements that are stored in the vector
	typedef ta_element_type element_type;
	/// declare value type for range interface
	typedef ta_element_type value_type;
	/// type of iterator with read and write access
	typedef typename element_type* iterator;
	/// type of iterator with read access only
	typedef typename const element_type* const_iterator;
	/// type of a reverse iterator
	typedef std::reverse_iterator<iterator> reverse_iterator;
	/// type of a const reverse iterator
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	/// type of iterator with read access only
	typedef typename array<element_type, ta_extent> this_type;
	/// type of a reference to an element
	typedef typename element_type& reference;
	/// type of a const reference to an element
	typedef typename const element_type& const_reference;
	/// type used to index the array
	typedef type::uint32 size_type;
	/// type for difference between two iterators
	typedef type::index_type difference_type;
	/// compile time: no initialization; runtime: array with 0 elements and no memory allocation
	array() {}
	/// only runtime version: allocate sufficient space for nr_elements elements, call their standard constructor
	array(uint32 nr_elements)
	{
		CGV_DEFINES_ASSERT(ta_extent == 0);
		resize(nr_elements);
//		Array::traits<ta_element_type, ta_extent>::resize(*this, nr_elements);
	}
	/// resize the array if possible
	void resize(uint32 n)
	{
		Array::traits<ta_element_type, ta_extent>::resize(*this, n);
	}
	/// compile time: initialize the first 2 elements to the given values; runtime: resize(2) and set to values
	array(const element_type& e0, const element_type& e1) 
	{
		Array::traits<ta_element_type, ta_extent>::resize(*this, 2);
		if (extent > 0) elements[0] = e0; 
		if (extent > 1) elements[1] = e1; 
	}
	/// compile time: initialize the first 3 elements to the given values; runtime: resize(3) and set to values
	array(const element_type& e0, const element_type& e1, const element_type& e2) 
	{
		Array::traits<ta_element_type, ta_extent>::resize(*this, 3);
		if (extent > 0) elements[0] = e0; 
		if (extent > 1) elements[1] = e1; 
		if (extent > 2) elements[2] = e2; 
	}
	/// compile time: initialize the first 4 elements to the given values; runtime: resize(4) and set to values
	array(const element_type& e0, const element_type& e1, const element_type& e2, const element_type& e3) 
	{
		Array::traits<ta_element_type, ta_extent>::resize(*this, 4);
		if (extent > 0) elements[0] = e0; 
		if (extent > 1) elements[1] = e1; 
		if (extent > 2) elements[2] = e2; 
		if (extent > 3) elements[3] = e3; 
	}
	/// compile time: initialize the first 5 elements to the given values; runtime: resize(5) and set to values
	array(const element_type& e0, const element_type& e1, const element_type& e2, const element_type& e3, const element_type& e4) 
	{
		Array::traits<ta_element_type, ta_extent>::resize(*this, 5);
		if (extent > 0) elements[0] = e0; 
		if (extent > 1) elements[1] = e1; 
		if (extent > 2) elements[2] = e2; 
		if (extent > 3) elements[3] = e3; 
		if (extent > 4) elements[4] = e4; 
	}
	/** assignment from 
		- constant   ... fill all elements with const value
		- range      ... copy values in range
			- compile time version: copy only as many elements as there is space in the array
			- runtime version: always resize to size of the range on the rhs
		- expression ... set values in range to evaluated values in expression. If the expression is a range such es a(_i), follow the rules of assignment from ranges
	*/	
	template <typename _ta_type>
	typename cgv::range::expression::assign_expression<this_type&, _ta_type> operator = (const _ta_type& value)
	{
		Array::traits<ta_element_type, ta_extent>::prepare<_ta_type>::assign(*this, value);
		return typename cgv::range::expression::assign_expression<this_type&, _ta_type>(*this, value);
	}
	/// only const random access operator available
	template <typename _ta_type>
	typename cgv::range::expression::random_access_expression<const this_type&, _ta_type> operator () (const _ta_type& value) const
	{
		return typename cgv::range::expression::random_access_expression<const this_type&, _ta_type>(*this, value);
	}
	/// return iterator to first element
	iterator begin() { return elements; }
	/// return iterator to behind last element
	iterator end() { return elements+extent; }
	/// return iterator to first element
	const_iterator begin() const { return elements; }
	/// return iterator to behind last element
	const_iterator end() const { return elements+extent; }
	/// reverse iterator starts at end
	reverse_iterator rbegin()             { return reverse_iterator(end()); }
	/// reverse iterator starts at end
	const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	/// reverse iterator ends at beginning
	reverse_iterator rend()               { return reverse_iterator(begin()); }
	/// reverse iterator ends at beginning
	const_reverse_iterator rend() const   { return const_reverse_iterator(begin()); }
	/// access to an element without range check
	reference operator[] (uint32 dimension) { return elements[dimension]; }
	/// const access to an element without range check
	const_reference operator[] (uint32 dimension) const { return elements[dimension]; }
	/// access with range check
	reference at(uint32 i) { range_check(i); return elements[i]; }
	/// const access with range check
	const_reference at(uint32 i) const { range_check(i); return elements[i]; }
	/// reference to first element 
	reference front() { return elements[0]; }		        
	/// const reference to first element 
	const_reference front() const { return elements[0]; }
	/// reference to last element 
	reference back() { return elements[extent-1]; }
	/// const reference to last element 
	const_reference back() const { return elements[extent-1]; }
private:
	/// check range
	static void range_check(uint32 i) 
	{
		if (i >= size()) { 
			throw std::range_error("array<>: index out of range");
		}
	}
};
		}
	}
}
