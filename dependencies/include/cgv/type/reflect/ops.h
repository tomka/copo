#pragma once

#include <cgv/type/invalid_type.h>
#include <cgv/type/consts.h>

namespace cgv {
	namespace type {
		/// binary type operator implementing the plus operator
		template <typename T1, typename T2> struct plus : public invalid_type {};
		template <char i1, char i2> struct plus<char_const<i1>, char_const<i2> >     { typedef char_const<i1+i2> type; };
		template <short i1, short i2> struct plus<short_const<i1>, short_const<i2> > { typedef short_const<i1+i2> type; };
		template <int i1, int i2> struct plus<int_const<i1>, int_const<i2> >         { typedef int_const<i1+i2> type; };
		template <unsigned char i1, unsigned char i2> struct plus<unsigned_char_const<i1>, unsigned_char_const<i2> >     { typedef unsigned_char_const<i1+i2> type; };
		template <unsigned short i1, unsigned short i2> struct plus<unsigned_short_const<i1>, unsigned_short_const<i2> > { typedef unsigned_short_const<i1+i2> type; };
		template <unsigned int i1, unsigned int i2> struct plus<unsigned_int_const<i1>, unsigned_int_const<i2> >         { typedef unsigned_int_const<i1+i2> type; };
		/// binary type expression implementing the plus operator
		struct plus_expr 
		{
			template <typename T1, typename T2> 
			struct apply
			{
				typedef typename plus<T1,T2>::type type;
			};
		};
		/// unary type operator implementing the sizeof operator
		template <typename T> struct size_of { typedef int_const<sizeof(T)> type; };
		/// unary type expression implementing the sizeof operator
		struct size_of_expr 
		{ 
			template <typename T> 
			struct apply
			{
				typedef typename size_of<T>::type type;
			};
		};
		/// concatenate two expressions
	}
}
