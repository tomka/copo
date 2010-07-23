#pragma once

#include <cgv/storage/collection/array.h>
#include <cgv/range/operators.h>
#include <cgv/range/sub.h>

namespace cgv {
	namespace math {
		namespace func {
			/** The func class is the base class of all mathematical functions that support evaluation and 
			    differentiation. The class is templated over source and target type as well as source and target
				dimension. For different special cases specializations exist. The function class provides an implementation
				of the differentiation function which is based on central differences. The step size \c epsilon for the central 
				differences is stored in a static member and can be set only once per source type. The eval and diff methods
				as well as the destructor are kept virtual in order to provide maximum flexibility. */
			template <typename ta_source_value, typename ta_target_value, unsigned int ta_source_dimensionality = 1, unsigned int ta_target_dimensionality = 1>
			class func
			{
			public:
				///
				typedef unsigned int size_type;
				typedef ta_source_value source_value_type;
				typedef ta_target_value target_value_type;
				static const size_type source_dimensionality = ta_source_dimensionality;
				static const size_type target_dimensionality = ta_target_dimensionality;
				typedef typename cgv::storage::collection::array<source_value_type, ta_source_dimensionality> source_type;
				typedef typename cgv::storage::collection::array<target_value_type, ta_target_dimensionality> target_type;
				typedef typename cgv::storage::collection::array<target_value_type, ta_source_dimensionality> gradient_component_type;
				typedef typename cgv::storage::collection::array<target_value_type, ta_source_dimensionality*ta_target_dimensionality> gradient_type;
				/// standard constructor
				function() {}
				/// virtual destructor
				virtual ~function() {}
				/// evaluate the function at the given location only of the given target dimension 
				virtual target_value_type eval(const source_type& location, unsigned int target_dimension) const { return target_value_type(0); }
				/// evaluate the function at the given location 
				virtual target_type eval(const source_type& location) const { return target_type(0); }
				/// compute the derivative of the function at the given location in the given source dimension only of the given target dimension, the standard implementation uses central differences with an epsilon of 1e-8
				virtual target_value_type diff(const source_type& location, unsigned int source_dimension, unsigned int target_dimension) const
				{
					source_type loc(location);
					loc[source_dimension] += (source_value_type) 1e-8;
					target_value_type result = eval(loc, target_dimension);
					loc[source_dimension] = location[source_value_type] - (source_value_type) 1e-8;
					result -= eval(loc, target_dimension);
					result /= (target_value_type) 2e-8;
				}
				/// compute the derivative of the function at the given location in the given source dimension, the standard implementation uses central differences with an epsilon of 1e-8
				virtual target_type diff(const source_type& location, unsigned int source_dimension) const
				{
					source_type loc(location);
					loc[source_dimension] += (source_value_type) 1e-8;
					target_type result = eval(loc);
					loc[source_dimension] = location[source_value_type] - (source_value_type) 1e-8;
					result -= eval(loc);
					result /= (target_value_type) 2e-8;
					return result;
				}
			};

			/** specialization to one dimensional target space. */
			template <typename ta_source_value, typename ta_target_value, unsigned int ta_source_dimensionality = 1>
			class function_base<ta_function_implementation,ta_source_value,ta_target_value,ta_source_dimensionality,1>
			{
			public:
				///
				typedef unsigned int size_type;
				typedef ta_source_value source_value_type;
				typedef ta_target_value target_value_type;
				static const size_type source_dimensionality = ta_source_dimensionality;
				static const size_type target_dimensionality = 1;
				typedef typename cgv::storage::collection::array<source_value_type, ta_source_dimensionality> source_type;
				typedef typename ta_target_value target_type;
				typedef typename cgv::storage::collection::array<target_value_type, ta_source_dimensionality> gradient_component_type;
				typedef typename cgv::storage::collection::array<target_value_type, ta_source_dimensionality> gradient_type;
				/// evaluate the function at the given location 
				target_type eval(const source_type& location) const { return target_type(0); }
				/// compute the derivative of the function at the given location in the given source dimension, the standard implementation uses central differences with an epsilon of 1e-8
				target_type diff(const source_type& location, unsigned int source_dimension) const
				{
					source_type loc(location);
					loc[source_dimension] += (source_value_type) 1e-8;
					target_type result = eval(loc);
					loc[source_dimension] = location[source_value_type] - (source_value_type) 1e-8;
					result -= eval(loc);
					result /= (target_value_type) 2e-8;
					return result;
				}
			};


			/** specialization to one dimensional source and target space. */
			template <typename ta_source_value, typename ta_target_value>
			class function_base<ta_function_implementation,ta_source_value,1,1>
			{
			public:
				///
				typedef unsigned int size_type;
				typedef ta_source_value source_value_type;
				typedef ta_target_value target_value_type;
				static const size_type source_dimensionality = 1;
				static const size_type target_dimensionality = 1;
				typedef typename ta_source_value source_type;
				typedef typename ta_target_value target_type;
				typedef typename ta_target_value gradient_component_type;
				typedef typename ta_target_value gradient_type;
				/// evaluate the function at the given location 
				target_type eval(const source_type& location) const { return target_type(0); }
				/// compute the derivative of the function at the given location, the standard implementation uses central differences with an epsilon of 1e-8
				target_type diff(const source_type& location) const
				{
					source_type loc = location + (source_value_type) 1e-8;
					target_type result = eval(loc);
					loc = location - (source_value_type) 1e-8;
					result -= eval(loc);
					result /= (target_value_type) 2e-8;
					return result;
				}
			};

			/** this class extends an implementation of a potentially multidimensional function with further access methods. 
			    The function implementation is given as template argument and has to implement the methods 
				eval(const source_type&) and diff(unsigned int, const source_type&). It should also publicly define 
				the source and target types which can be achieved by using function_base as base class of the function
				implementation. There exist specializations for a one dimensional target and source space respectively. */
			template <class ta_function_implementation, typename ta_source_value, typename ta_target_value, unsigned int ta_source_dimensionality = 1, unsigned int ta_target_dimensionality = 1>
			class function : public ta_function_implementation
			{
			public:
				/// compute the gradient of the function at the given location of the given target dimension in the given target dimension only
				gradient_component_type grad(const source_type& location, size_type target_dimension) const
				{
					gradient_component_type result;
					for (size_type source_dimension = 0; source_dimension < source_dimensionality; ++source_dimension)
						result[source_dimension] = diff(location, source_dimension);
					return result;
				}
				/// compute the gradient of the function at the given location
				gradient_type grad(const source_type& location) const
				{
					gradient_type result;
					size_type start = 0;
					for (size_type target_dimension = 0; target_dimension < target_dimensionality; ++target_dimension) {
						sub(range, start) = grad(location, target_dimension);
						start += source_dimensionality;
					}
					return result;
				}
			};
			/** specialization of function for two source dimensions. */
			template <class ta_function_implementation, typename ta_source_value, typename ta_target_value, unsigned int ta_target_dimensionality>
			class function<ta_function_implementation,ta_source_value,ta_target_value,2,ta_target_dimensionality> : public ta_function_implementation
			{
			public:
				/// evaluate the function at the location given with two coordinates 
				target_value_type eval(source_value_type x, source_value_type y) const { return eval(source_type(x,y)); }
				/// evaluate the function at the location given with two coordinates in the given target dimension only
				target_value_type eval(source_value_type x, source_value_type y, unsigned int target_dimension) const { return eval(source_type(x,y), target_dimension); }
				/// compute the derivative of the function at the location given with two coordinates
				target_value_type diff(source_value_type x, source_value_type y, unsigned int source_dimension) const { return diff(source_dimension, source_type(x,y)); }
				/// compute the derivative of the function at the location given with two coordinates in the given target dimension only
				target_value_type diff(source_value_type x, source_value_type y, unsigned int source_dimension, unsigned int target_dimension) const { return diff(source_type(x,y), source_dimension, target_dimension); }
				/// compute the gradient of the function at the given location of the given target dimension in the given target dimension only
				gradient_component_type grad(const source_type& location, size_type target_dimension) const
				{
					gradient_component_type result;
					for (size_type source_dimension = 0; source_dimension < source_dimensionality; ++source_dimension)
						result[source_dimension] = diff(location, source_dimension);
					return result;
				}
				/// compute the gradient of the function at the given location
				gradient_type grad(const source_type& location) const
				{
					gradient_type result;
					size_type start = 0;
					for (size_type target_dimension = 0; target_dimension < target_dimensionality; ++target_dimension) {
						sub(range, start) = grad(location, target_dimension);
						start += source_dimensionality;
					}
					return result;
				}
				/// compute the gradient of the function at the location given with two coordinates
				gradient_component_type grad(source_value_type x, source_value_type y, unsigned int target_dimension) const { return grad(source_type(x,y),target_dimension); }
				/// compute the gradient of the function at the location given with two coordinates
				gradient_type grad(source_value_type x, source_value_type y) const { return grad(source_type(x,y)); }
			};
			/** specialization of function for three source dimensions. */
			template <class ta_function_implementation, typename ta_source_value, typename ta_target_value, unsigned int ta_target_dimensionality>
			class function<ta_function_implementation,ta_source_value,ta_target_value,3,ta_target_dimensionality> : public ta_function_implementation
			{
			public:
				/// evaluate the function at the location given with two coordinates 
				target_value_type eval(source_value_type x, source_value_type y, source_value_type z) const { return eval(source_type(x,y,z)); }
				/// compute the derivative of the function at the location given with two coordinates
				target_value_type diff(source_value_type x, source_value_type y, source_value_type z, unsigned int source_dimension) const { return diff(source_type(x,y,z), source_dimension); }
				/// evaluate the function at the location given with two coordinates in the given target dimension only
				target_value_type eval(source_value_type x, source_value_type y, source_value_type z, unsigned int target_dimension) const { return eval(source_type(x,y,z), target_dimension); }
				/// compute the derivative of the function at the location given with two coordinates in the given target dimension only
				target_value_type diff(source_value_type x, source_value_type y, source_value_type z, unsigned int source_dimension, unsigned int target_dimension) const { return diff(source_type(x,y,z), source_dimension, target_dimension); }
				/// compute the gradient of the function at the given location of the given target dimension in the given target dimension only
				gradient_component_type grad(const source_type& location, size_type target_dimension) const
				{
					gradient_component_type result;
					for (size_type source_dimension = 0; source_dimension < source_dimensionality; ++source_dimension)
						result[source_dimension] = diff(location, source_dimension);
					return result;
				}
				/// compute the gradient of the function at the given location
				gradient_type grad(const source_type& location) const
				{
					gradient_type result;
					size_type start = 0;
					for (size_type target_dimension = 0; target_dimension < target_dimensionality; ++target_dimension) {
						sub(range, start) = grad(location, target_dimension);
						start += source_dimensionality;
					}
					return result;
				}
				/// compute the gradient of the function at the location given with three coordinates
				gradient_component_type grad(source_value_type x, source_value_type y, source_value_type z, unsigned int target_dimension) const { return grad(source_type(x,y,z),target_dimension); }
				/// compute the gradient of the function at the location given with three coordinates
				gradient_type grad(source_value_type x, source_value_type y, source_value_type z) const { return grad(source_type(x,y,z)); }
			};
			/** specialization of function for two source dimensions and one target dimension. */
			template <class ta_function_implementation, typename ta_source_value, typename ta_target_value>
			class function<ta_function_implementation,ta_source_value,ta_target_value,2,1> : public ta_function_implementation
			{
			public:
				/// evaluate the function at the location given with two coordinates 
				target_value_type eval(source_value_type x, source_value_type y) const { return eval(source_type(x,y)); }
				/// compute the derivative of the function at the location given with two coordinates
				target_value_type diff(source_value_type x, source_value_type y, unsigned int source_dimension) const { return diff(source_dimension, source_type(x,y)); }
				/// compute the gradient of the function at the given location
				gradient_type grad(const source_type& location) const
				{
					gradient_type result;
					for (size_type source_dimension = 0; source_dimension < source_dimensionality; ++source_dimension)
						result[source_dimension] = diff(location, source_dimension);
					return result;
				}
				/// compute the gradient of the function at the location given with two coordinates
				gradient_type grad(source_value_type x, source_value_type y) const { return grad(source_type(x,y)); }
			};
			/** specialization of function for three source dimensions and one target dimension. */
			template <class ta_function_implementation, typename ta_source_value, typename ta_target_value>
			class function<ta_function_implementation,ta_source_value,ta_target_value,3,1> : public ta_function_implementation
			{
			public:
				/// evaluate the function at the location given with two coordinates 
				target_value_type eval(source_value_type x, source_value_type y, source_value_type z) const { return eval(source_type(x,y,z)); }
				/// compute the derivative of the function at the location given with two coordinates
				target_value_type diff(source_value_type x, source_value_type y, source_value_type z, unsigned int source_dimension) const { return diff(source_type(x,y,z), source_dimension); }
				/// compute the gradient of the function at the given location
				gradient_type grad(const source_type& location) const
				{
					gradient_type result;
					for (size_type source_dimension = 0; source_dimension < source_dimensionality; ++source_dimension)
						result[source_dimension] = diff(location, source_dimension);
					return result;
				}
				/// compute the gradient of the function at the location given with three coordinates
				gradient_type grad(source_value_type x, source_value_type y, source_value_type z) const { return grad(source_type(x,y,z)); }
			};
		}
	}
}