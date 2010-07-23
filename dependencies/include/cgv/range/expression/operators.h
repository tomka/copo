#pragma once

#include <cgv/range/expression/unary_expression.h>
#include <cgv/range/expression/binary_expression.h>

namespace cgv {
	namespace range {
		CGV_RANGE_EXPRESSION_UNARY_OPERATOR(minus, -)
		CGV_RANGE_EXPRESSION_UNARY_OPERATOR(plus, +)
		CGV_RANGE_EXPRESSION_UNARY_OPERATOR(not, !)
		CGV_RANGE_EXPRESSION_UNARY_OPERATOR(one_complement, ~)
		CGV_RANGE_EXPRESSION_BINARY_OPERATOR(add, +)
		CGV_RANGE_EXPRESSION_BINARY_OPERATOR(sub, -)
		CGV_RANGE_EXPRESSION_BINARY_OPERATOR(mul, *)
		CGV_RANGE_EXPRESSION_BINARY_OPERATOR(div, /)
		CGV_RANGE_EXPRESSION_BINARY_OPERATOR(mod, %)
		CGV_RANGE_EXPRESSION_BINARY_OPERATOR(xor, ^)
		CGV_RANGE_EXPRESSION_BINARY_OPERATOR(and, &)
		CGV_RANGE_EXPRESSION_BINARY_OPERATOR(or, |)
		CGV_RANGE_EXPRESSION_BINARY_OPERATOR(land, &&)
		CGV_RANGE_EXPRESSION_BINARY_OPERATOR(lor, ||)
	}
}
