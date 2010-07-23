#pragma once

#include <cgv/type/concept.h>
#include <cgv/type/ctrl/if_.h>
#include <cgv/type/cond/is_const.h>
#include <cgv/type/func/make_const.h>
#include <cgv/type/func/make_ref.h>
#include <cgv/type/func/drop_ref.h>
#include <cgv/range/ForwardIterator.h>
#include <cgv/range/ForwardRange.h>
#include <cgv/utils/execute_on_destruct.h>

namespace cgv {
	namespace range {
		namespace expression {
			namespace Expression {
				/** An expression is derived from this tag and must implement the following concepts:
\code
typedef ... iterator;                 // iterator type used to specify elements
iterator begin() [const]              // method that generates an iterator that points to the first element
iterator end()   [const]	          // method that generates another iterator that points to the last element
typedef ... expression_return_type;   // the type returned by the execute_at method
expression_return_type 
execute_at(const iterator&) const; // method that applies expression to the element pointed to by the iterator
\endcode
				*/
				struct tag {};
			}
			/** the const_expression implements an expression that keeps a copy of a constant value which is returned by the
				execute method, independent of the passed iterator. It is not derived from execute_on_destruct because it is
				no range. */
			template <typename T>
			struct const_expression : public cgv::type::concepts<Expression::tag>
			{
				typedef const T& expression_return_type;
				/// construct from constant
				const_expression(const T& _value) : value(_value) {}
				/// minimal implementation of iterator that does nothing and always compares to false
				struct iterator : public concepts<ForwardIterator::tag>, public ForwardIterator::check<iterator>
				{
					typedef T value_type;
					typedef const T& reference;
					iterator& operator ++() { return *this; }
					bool operator == (const iterator&) const { return false; }
					bool operator != (const iterator&) const { return true; }
				};
				typedef iterator const_iterator;
				iterator begin() const { return iterator(); }
				iterator end() const { return iterator(); }
				static cgv::type::uint32 size() { return 0; }
				/// the execute_at method returns const reference to the stored value
				expression_return_type execute_at(const iterator&) const { return value; }
				/// store copy of value
				T value;
			};
			namespace Expression {
				namespace Traits {
					/// not specialized implementation 
					template <typename T>
					struct traits : public cgv::type::not_specialized 
					{
						typedef typename cgv::type::cond::false_type implements;

						typedef typename cgv::type::func::clean<T>::type clean_type;
						typedef const_expression<clean_type> storage_type;
						typedef const clean_type& construct_argument_type;
						typedef typename storage_type::expression_return_type return_type;
						typedef const storage_type& execute_expression_argument_type;
						typedef typename storage_type::iterator iterator;
						static storage_type construct(construct_argument_type value) 
						{
							return storage_type(value); 
						}
						static return_type execute_at(execute_expression_argument_type expression, const iterator& iter)
						{
							return expression.execute_at(iter);
						}
						static cgv::type::uint32 size(const T&)
						{
							return 1;
						}
					};
					/// implementation for forward ranges 
					template <typename T>
					struct traits<cgv::type::concept<ForwardRange::tag,T> >
					{
						typedef typename cgv::type::cond::false_type implements;
						typedef typename ForwardRange::traits<T>::promote_const_reference 
							return_type;
						typedef typename cgv::type::func::make_ref<
							typename cgv::type::func::make_const<T>::type>::type 
							execute_expression_argument_type;
						typedef typename ForwardRange::traits<T>::promote_const_iterator 
							iterator;
						typedef typename cgv::type::func::make_ref<T>::type 
							storage_type;
						typedef storage_type construct_argument_type;

						static construct_argument_type construct(construct_argument_type range) 
						{
							return range; 
						}
						/// standard implementation calls operator *() of the iterator
						static return_type execute_at(execute_expression_argument_type, const iterator& iter) 
						{
							return *iter;
						}
						static cgv::type::uint32 size(const T& range)
						{
							return range.size();
						}
					};
					/// implementation for expressions
					template <typename T>
					struct traits<cgv::type::concept<tag,T> >
					{
						typedef typename cgv::type::cond::true_type implements;
						/// remove modifiers to extract storage type
						typedef typename cgv::type::func::clean<T>::type storage_type;
						/// use return type defined in expression
						typedef typename storage_type::expression_return_type return_type;
						/// pass expression by const reference
						typedef typename cgv::type::func::make_ref<
							typename cgv::type::func::make_const<T>::type>::type 
							execute_expression_argument_type;
						/// use iterator type defined in expression
						typedef typename storage_type::iterator iterator;
						/// query ExecuteOnDestruct traits for the argument type to be used in the constructor
						typedef typename cgv::utils::ExecuteOnDestruct::traits<T>::argument_type 
							construct_argument_type;

						/// simply pass on reference to expression
						static construct_argument_type construct(construct_argument_type expression) 
						{
							return expression; 
						}
						/// call the execute_at method of the expression
						static return_type execute_at(execute_expression_argument_type expression, const iterator& iter)
						{
							return expression.execute_at(iter);
						}
						static cgv::type::uint32 size(const T& expression)
						{
							return expression.size();
						}
					};
				}
				/** gives access to implements, return_type, argument_type, iterator_type and 
				    execute_at for ranges and expressions. */
				template <typename T>
				struct traits : public cgv::type::pick<Traits::traits, typename cgv::type::func::drop_ref<T>::type>::type {};
			}
		}
	}
}
