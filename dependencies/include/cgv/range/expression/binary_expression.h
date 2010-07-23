#pragma once

#include <cgv/utils/pair.h>
#include <cgv/type/func/clean.h>
#include <cgv/type/func/promote_const.h>
#include <cgv/utils/execute_on_destruct.h>
#include <cgv/range/expression/expression.h>

namespace cgv {
	namespace range {
		namespace expression {

	/** use this class to declare an expression that depends on two arguments */
	template <typename T1, typename T2, typename derived_class>
	struct binary_expression :
		public cgv::type::concepts<Expression::tag>,						  // mark as expression
		public cgv::utils::execute_on_destruct<derived_class>,        // delay execution to destructor
		public cgv::utils::pair<typename Expression::traits<T1>::storage_type, typename Expression::traits<T2>::storage_type> // implement range as range pair
	{
		typedef typename cgv::type::ds::list<Expression::tag,cgv::utils::ExecuteOnDestruct::tag>::type concept_list;
		/// publish all types from the two sub-expressions
		typedef typename Expression::traits<T1>::storage_type storage_type_1;
		typedef typename Expression::traits<T1>::iterator iterator_1;
		typedef typename Expression::traits<T1>::construct_argument_type construct_argument_type_1;
		typedef typename Expression::traits<T1>::execute_expression_argument_type execute_expression_argument_type_1;
		typedef typename Expression::traits<T1>::return_type return_type_1;
		typedef typename Expression::traits<T2>::storage_type storage_type_2;
		typedef typename Expression::traits<T2>::iterator iterator_2;
		typedef typename Expression::traits<T2>::construct_argument_type construct_argument_type_2;
		typedef typename Expression::traits<T2>::execute_expression_argument_type execute_expression_argument_type_2;
		typedef typename Expression::traits<T2>::return_type return_type_2;
		/// derive value types from return type
		typedef typename cgv::type::func::clean<return_type_1>::type value_type_1;
		typedef typename cgv::type::func::clean<return_type_2>::type value_type_2;
		/// my own type
		typedef binary_expression<T1, T2, derived_class> this_type;
		/// base class is implemented as a range pair
		typedef cgv::utils::pair<storage_type_1, storage_type_2> pair_base_type;
		/// iterator type used in execute method
		struct iterator : public concepts<ForwardIterator::tag>, public ForwardIterator::check<iterator>
		{
			/// extraction of reference type
			typedef typename cgv::type::func::promote_const<T1, typename cgv::type::Dereferencible::traits<iterator_1>::reference>::type reference_1;
			typedef typename cgv::type::func::promote_const<T1, typename cgv::type::Dereferencible::traits<iterator_2>::reference>::type reference_2;
			/// paired reference type
			typedef typename cgv::utils::pair<reference_1, reference_2> reference;
			typedef typename cgv::utils::pair<typename cgv::type::func::drop_ref<reference_1>::type, 
				typename cgv::type::func::drop_ref<reference_2>::type> value_type;
			/// store both iterators
			iterator_1 first;
			iterator_2 second;
			iterator& operator ++() { ++first; ++second; return *this; }
			reference operator * () const { return reference(*first, *second); }
			iterator(iterator_1 _first, iterator_2 _second) : first(_first), second(_second) {}
			/// compare for equality with the boolean or operation
			bool operator == (const iterator& iter) const 
			{ 
				return first == iter.first || second == iter.second; 
			}
			bool operator != (const iterator& iter) const 
			{
				return !this->operator == (iter); 
			}
		};
		/// access to range
		iterator begin() { return iterator(iterator_1(first.begin()), iterator_2(second.begin())); }
		iterator end()   { return iterator(iterator_1(first.end()), iterator_2(second.end())); }
		cgv::type::uint32 size() const { return std::max(first.size(), second.size()); }

		//typedef typename Expression::traits<derived_class>::iterator expression_iterator;
		/// construct from two types that can be converted to expressions
		binary_expression(construct_argument_type_1 value_1, construct_argument_type_2 value_2) : 
			pair_base_type(Expression::traits<T1>::construct(value_1), Expression::traits<T2>::construct(value_2))
		{
			// deactivate all execute_on_destruct derivates
			cgv::utils::ExecuteOnDestruct::deactivate(value_1);
			cgv::utils::ExecuteOnDestruct::deactivate(value_2);
			// deactivate all value copies of execute_on_destruct in the pair
			cgv::utils::ExecuteOnDestruct::deactivate(first);
			cgv::utils::ExecuteOnDestruct::deactivate(second);
		}
		/// apply expression to the range by iterating over all elements in between begin() and end() and by calling the execute_at method
		void execute()
		{
			for (iterator i = begin(); i != end(); ++i)
				static_cast<derived_class*>(this)->execute_at(i);
		}
	};
		}
	}
}

#include <cgv/range/ForwardRange.h>
#include <cgv/type/func/promote.h>

/** standard implementation for an expression for a binary operator, which should be used inside namespace cgv::range. The
    macro call CGV_RANGE_EXPRESSION_BINARY_OPERATOR_EXPRESSION(add, +) will generate the following code:
\code
namespace Expression {
	/// the add expression delays summation of range expressions. 
	template <typename T1, typename T2>
	struct add_expression : public binary_expression<T1, T2, add_expression<T1, T2> >
	{
		/// return a reference to the values pointed to by the first iterator
		typedef typename cgv::type::func::promote<value_type_1,value_type_2>::type value_type;
		typedef value_type expression_return_type;
		/// define base type
		typedef typename binary_expression<T1, T2, add_expression<T1, T2> > expression_base_type;
		/// perform const_cast to allow usage with add operator
		add_expression(
			typename cgv::type::func::make_const<construct_argument_type_1>::type value_1, 
			typename cgv::type::func::make_const<construct_argument_type_2>::type value_2) : 
		expression_base_type(const_cast<construct_argument_type_1>(value_1), const_cast<construct_argument_type_2>(value_2)) {}
		///
		expression_return_type execute_at(const iterator& iter) const
		{
			return Expression::traits<T1>::execute_at(first, iter.first) +
				        Expression::traits<T2>::execute_at(second, iter.second);
		}
	};
}
\endcode
*/
#define CGV_RANGE_EXPRESSION_BINARY_OPERATOR_EXPRESSION(NAME, OPER) \
	namespace expression { \
		template <typename T1, typename T2> \
		struct NAME##_expression : public binary_expression<T1, T2, NAME##_expression<T1, T2> > \
		{ \
			typedef typename cgv::type::func::promote<value_type_1,value_type_2>::type value_type;\
			typedef value_type expression_return_type;\
			typedef typename binary_expression<T1, T2, NAME##_expression<T1, T2> > expression_base_type;\
			NAME##_expression( \
				typename cgv::type::func::make_const<construct_argument_type_1>::type value_1, \
				typename cgv::type::func::make_const<construct_argument_type_2>::type value_2) : \
			expression_base_type(const_cast<construct_argument_type_1>(value_1), const_cast<construct_argument_type_2>(value_2)) {} \
			expression_return_type execute_at(const iterator& iter) const \
			{ \
				return Expression::traits<T1>::execute_at(first, iter.first) OPER \
							Expression::traits<T2>::execute_at(second, iter.second); \
			} \
		}; \
	}
/// similar to CGV_RANGE_EXPRESSION_BINARY_OPERATOR_EXPRESSION
#define CGV_RANGE_EXPRESSION_BINARY_FUNCTION_EXPRESSION(NAME) \
	namespace expression { \
		template <typename T1, typename T2> \
		struct NAME##_expression : public binary_expression<T1, T2, NAME##_expression<T1, T2> > \
		{ \
			typedef typename cgv::type::func::promote<value_type_1,value_type_2>::type value_type;\
			typedef value_type expression_return_type;\
			typedef typename binary_expression<T1, T2, NAME##_expression<T1, T2> > expression_base_type;\
			NAME##_expression( \
				typename cgv::type::func::make_const<construct_argument_type_1>::type value_1, \
				typename cgv::type::func::make_const<construct_argument_type_2>::type value_2) : \
			expression_base_type(const_cast<construct_argument_type_1>(value_1), const_cast<construct_argument_type_2>(value_2)) {} \
			expression_return_type execute_at(const iterator& iter) const \
			{ \
				return NAME(Expression::traits<T1>::execute_at(first, iter.first),\
							Expression::traits<T2>::execute_at(second, iter.second)); \
			} \
		}; \
	}

/** standard implementation for a binary operator, which should be used inside namespace cgv::range. The
    macro call CGV_RANGE_EXPRESSION_BINARY_OPERATOR_OPERATOR(add, +)
\code
		namespace ForwardRange {
			/// provide add operator for ranges 
			template <typename T1, typename T2>
			expression::add_expression<T1, T2> operator + (const T1& value_1, const T2& value_2)
			{
				return expression::add_expression<T1, T2>(value_1,value_2);
			}
		}
		namespace expression {
			namespace Expression {
				using cgv::range::ForwardRange::operator +;
			}
		}
\endcode
*/
#define CGV_RANGE_EXPRESSION_BINARY_OPERATOR_OPERATOR(NAME, OPER) \
	namespace ForwardRange { \
		template <typename T1, typename T2> \
		expression::NAME##_expression<T1, T2> operator OPER (const T1& value_1, const T2& value_2) \
		{ \
			return expression::NAME##_expression<T1, T2>(value_1,value_2); \
		} \
	} \
	namespace expression { \
		namespace Expression { \
			using cgv::range::ForwardRange::operator OPER; \
		} \
	}
/// similar to CGV_RANGE_EXPRESSION_BINARY_OPERATOR_OPERATOR
#define CGV_RANGE_EXPRESSION_BINARY_FUNCTION_FUNCTION(NAME) \
	namespace ForwardRange { \
		template <typename T1, typename T2> \
		expression::NAME##_expression<T1, T2> NAME(const T1& value_1, const T2& value_2) \
		{ \
			return expression::NAME##_expression<T1, T2>(value_1,value_2); \
		} \
	} \
	namespace expression { \
		namespace Expression { \
			using cgv::range::ForwardRange::NAME; \
		} \
	}

/** implementation of expression and operator for a binary operator, which should be used inside namespace cgv::range. This macro
    invokes CGV_RANGE_EXPRESSION_BINARY_OPERATOR_EXPRESSION and CGV_RANGE_EXPRESSION_BINARY_OPERATOR_OPERATOR. See their documentation
	for sample code that is generated by the macros. */
#define CGV_RANGE_EXPRESSION_BINARY_OPERATOR(NAME, OPER) \
	CGV_RANGE_EXPRESSION_BINARY_OPERATOR_EXPRESSION(NAME, OPER) \
	CGV_RANGE_EXPRESSION_BINARY_OPERATOR_OPERATOR(NAME, OPER)
/// similar to CGV_RANGE_EXPRESSION_BINARY_OPERATOR
#define CGV_RANGE_EXPRESSION_BINARY_FUNCTION(NAME) \
	CGV_RANGE_EXPRESSION_BINARY_FUNCTION_EXPRESSION(NAME) \
	CGV_RANGE_EXPRESSION_BINARY_FUNCTION_FUNCTION(NAME)