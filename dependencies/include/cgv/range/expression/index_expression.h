#pragma once

#include <cgv/type/index_type.h>
#include <cgv/utils/execute_on_destruct.h>
#include <cgv/range/ForwardIterator.h>
#include <cgv/range/expression/expression.h>

namespace cgv {
	namespace range {
		namespace expression {

	/** this class returns an index starting at 0 */
	struct index_expression :
		public cgv::type::concepts<Expression::tag>  // mark as expression
	{
		typedef int expression_return_type;
		/// iterator type used in execute method
		struct iterator : public concepts<ForwardIterator::tag>, public ForwardIterator::check<iterator>
		{
			typedef expression_return_type reference;
			typedef cgv::type::func::drop_const<reference>::type value_type;
			expression_return_type i;
			iterator(int _i = 0) : i(_i) {}
			iterator& operator ++() { ++i; return *this; }
			reference operator * () const { return i; }
			/// compare for equality with the boolean or operation
			bool operator == (const iterator& iter) const 
			{ 
				return i == iter.i; 
			}
			bool operator != (const iterator& iter) const 
			{
				return !this->operator == (iter); 
			}
		};
		/// access to range
		iterator begin() { return iterator(); }
		iterator end()   { return iterator(-1); }
		cgv::type::uint32 size() const { return 0; }

		//typedef typename expression_traits<derived_class>::iterator expression_iterator;
		/// construct from two types that can be converted to expressions
		index_expression() {}
		expression_return_type execute_at(const iterator& iter) const
		{
			return *iter;
		}
	};
	index_expression _i;
		}
	}
}
