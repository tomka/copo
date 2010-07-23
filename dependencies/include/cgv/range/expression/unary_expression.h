#pragma once

#include <cgv/type/func/clean.h>
#include <cgv/utils/execute_on_destruct.h>
#include <cgv/range/expression/expression.h>

namespace cgv {
	namespace range {
		namespace expression {

	/** use this class to declare an expression that depends on one argument */
	template <typename T, typename derived_class>
	struct unary_expression :
		public cgv::type::concepts<Expression::tag>,						  // mark as expression
		public cgv::utils::execute_on_destruct<derived_class>        // delay execution to destructor
	{
		typedef typename cgv::type::ds::list<Expression::tag,cgv::utils::ExecuteOnDestruct::tag>::type concept_list;
		/// publish all types from the two sub-expressions
		typedef typename Expression::traits<T>::storage_type storage_type;
		typedef typename Expression::traits<T>::iterator iterator;
		typedef typename Expression::traits<T>::construct_argument_type construct_argument_type;
		typedef typename Expression::traits<T>::execute_expression_argument_type execute_expression_argument_type;
		/// derive value types from return type
		typedef typename cgv::type::func::clean<execute_expression_argument_type>::type value_type;
		/// my own type
		typedef unary_expression<T, derived_class> this_type;
		/// 
		storage_type dependent;
		/// access to iterators
		iterator begin() { return dependent.begin(); }
		iterator end()   { return dependent.end(); }
		cgv::type::uint32 size() const { return dependent.size(); }

		//typedef typename Expression::traits<derived_class>::iterator expression_iterator;
		/// construct from two types that can be converted to expressions
		unary_expression(construct_argument_type value) : dependent(value)
		{
			cgv::utils::ExecuteOnDestruct::deactivate(value);
			cgv::utils::ExecuteOnDestruct::deactivate(dependent);
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

/** standard implementation for an expression for a unary operator, which should be used inside namespace cgv::range. The
    macro call CGV_RANGE_EXPRESSION_UNARY_OPERATOR_EXPRESSION(negate, -) will generate the following code:

\code
namespace expression {

	/// the negate expression delays summation of range expressions.
	template <typename T>
	struct negate_expression : public unary_expression<T, negate_expression<T> >
	{
		/// define base type
		typedef typename unary_expression<T, negate_expression<T> > expression_base_type;
		typedef typename cgv::type::func::clean<typename Expression::traits<T>::return_type>::type expression_return_type;
		/// perform const_cast to allow usage with plus operator
		negate_expression(typename cgv::type::func::make_const<construct_argument_type>::type value) :
		   expression_base_type(const_cast<construct_argument_type>(value)) {}
		///
		expression_return_type execute_at(const iterator& iter) const
		{
			return -Expression::traits<T>::execute_at(dependent, iter);
		}
	};
}
\endcode
*/
#define CGV_RANGE_EXPRESSION_UNARY_OPERATOR_EXPRESSION(NAME, OPER) \
	namespace expression {\
	template <typename T>\
	struct NAME##_expression : public unary_expression<T, NAME##_expression<T> >\
	{\
		typedef typename unary_expression<T, NAME##_expression<T> > expression_base_type;\
		typedef typename cgv::type::func::clean<typename Expression::traits<T>::return_type>::type expression_return_type;\
		NAME##_expression(typename cgv::type::func::make_const<construct_argument_type>::type value) :\
		   expression_base_type(const_cast<construct_argument_type>(value)) {}\
		expression_return_type execute_at(const iterator& iter) const\
		{\
			return OPER Expression::traits<T>::execute_at(dependent, iter);\
		}\
	};\
		}

/// similar to CGV_RANGE_EXPRESSION_UNARY_OPERATOR_EXPRESSION
#define CGV_RANGE_EXPRESSION_UNARY_FUNCTION_EXPRESSION(NAME) \
	namespace expression {\
	template <typename T>\
	struct NAME##_expression : public unary_expression<T, NAME##_expression<T> >\
	{\
		typedef typename unary_expression<T, NAME##_expression<T> > expression_base_type;\
		typedef typename cgv::type::func::clean<typename Expression::traits<T>::return_type>::type expression_return_type;\
		NAME##_expression(typename cgv::type::func::make_const<construct_argument_type>::type value) :\
		   expression_base_type(const_cast<construct_argument_type>(value)) {}\
		expression_return_type execute_at(const iterator& iter) const\
		{\
			return NAME(Expression::traits<T>::execute_at(dependent, iter));\
		}\
	};\
		}

/** standard implementation for a unary operator, which should be used inside namespace cgv::range. The 
    macro call CGV_RANGE_EXPRESSION_UNARY_OPERATOR_OPERATOR(negate, -) will generate the following code:
\code
namespace ForwardRange {
	// provide unary negate operator for ranges
	template <typename T>
	expression::negate_expression<T> operator - (const T& value)
	{
		return expression::negate_expression<T>(value);
	}
}
namespace expression {
	namespace Expression {
		// and expressions
		using cgv::range::ForwardRange::operator -;
	}
}
\endcode
*/
#define CGV_RANGE_EXPRESSION_UNARY_OPERATOR_OPERATOR(NAME, OPER) \
		namespace ForwardRange {\
			template <typename T>\
			expression::NAME##_expression<T> operator OPER (const T& value)\
			{\
				return expression::NAME##_expression<T>(value);\
			}\
		}\
		namespace expression {\
			namespace Expression {\
				using cgv::range::ForwardRange::operator OPER;\
			}\
		}

/// similar to CGV_RANGE_EXPRESSION_UNARY_OPERATOR_OPERATOR
#define CGV_RANGE_EXPRESSION_UNARY_FUNCTION_FUNCTION(NAME) \
		namespace ForwardRange {\
			template <typename T>\
			expression::NAME##_expression<T> NAME(const T& value)\
			{\
				return expression::NAME##_expression<T>(value);\
			}\
		}\
		namespace expression {\
			namespace Expression {\
				using cgv::range::ForwardRange::NAME;\
			}\
		}

/** implementation of expression and operator for a unary operator, which should be used inside namespace cgv::range. This macro
    invokes CGV_RANGE_EXPRESSION_UNARY_OPERATOR_EXPRESSION and CGV_RANGE_EXPRESSION_UNARY_OPERATOR_OPERATOR. See their documentation
	for sample code that is generated by the macros. */
#define CGV_RANGE_EXPRESSION_UNARY_OPERATOR(NAME, OPER) \
	CGV_RANGE_EXPRESSION_UNARY_OPERATOR_EXPRESSION(NAME, OPER) \
	CGV_RANGE_EXPRESSION_UNARY_OPERATOR_OPERATOR(NAME, OPER)
/// similar to CGV_RANGE_EXPRESSION_UNARY_OPERATOR
#define CGV_RANGE_EXPRESSION_UNARY_FUNCTION(NAME) \
	CGV_RANGE_EXPRESSION_UNARY_FUNCTION_EXPRESSION(NAME) \
	CGV_RANGE_EXPRESSION_UNARY_FUNCTION_FUNCTION(NAME)