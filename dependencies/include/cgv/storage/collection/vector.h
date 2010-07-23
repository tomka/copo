#pragma once
#include <algorithm>
#include <iostream>
#include <vector>
#include <cgv/type/size_type.h>
#include <cgv/range/RandomAccessRange.h>
#include <cgv/range/PushBackRange.h>
#include <cgv/range/expression/assign_expression.h>
#include <cgv/range/expression/random_access_expression.h>

namespace cgv {
	namespace storage {
		namespace collection {
			/** extend the standard vector by the range_tag in order to allow usage of range operators and functions and redefine the
			    copy constructor and assignement operator to allow for expression templates and list assignement. */
			template <typename ta_element_type>
			class vector : 
				public std::vector<ta_element_type>, 
				public cgv::type::concepts<cgv::range::RandomAccessRange::tag,
										   cgv::range::PushBackRange::tag>,
				public cgv::range::RandomAccessRange::check<vector<ta_element_type> >,
				public cgv::range::PushBackRange::check<vector<ta_element_type> >
			{
			public:
				/// type of the elements that are stored in the vector
				typedef ta_element_type element_type;
				/// type of iterator with read access only
				typedef typename vector<element_type> this_type;
				///
				typedef cgv::type::size_type size_type;
				/// the standard constructor does not initialize the vector
				vector() {}
				/// initialize to one element
				vector(const element_type& e) 
				{
					push_back(e);
				}
				/// initialize to n copies of one element
				vector(const element_type& e, int count) 
				{
					resize(count);
					std::fill(begin(),end(),e);
				}
				/// pass copy construction to assignment operator
				template <typename _ta_type>
				vector(const _ta_type& value)
				{
					*this = value;
				}
				/// most flexible assignment via expression templates, any argument is converted to a single_value_range if necessary
				template <typename _ta_type>
				typename cgv::range::expression::assign_expression<this_type&, _ta_type> operator = (const _ta_type& value)
				{
					return typename cgv::range::expression::assign_expression<this_type&, _ta_type>(*this, value);
				}
				/// const random access operator
				template <typename _ta_type>
				typename cgv::range::expression::random_access_expression<const this_type&, _ta_type> operator () (const _ta_type& value) const
				{
					return typename cgv::range::expression::random_access_expression<const this_type&, _ta_type>(*this, value);
				}
			};
		}
	}
}
