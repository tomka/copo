#pragma once

namespace cgv {
	namespace type {
		/// enum that represents different kind of types. No long is supported, use INT32 or INT64 instead. TYPENAME is used as type placeholder.
		enum kind {
			TYPENAME, 
			CHAR, INT16, INT32, INT64, 
			UCHAR, UINT16, UINT32, UINT64,
			FLOAT, DOUBLE, 
			BOOL, WCHAR, 
			ENUM, 
			STRING, 
			FUNCTION_POINTER,
			POINTER, REFERENCE,
			STRUCT, CLASS, UNION,
			MEMBER_POINTER, METHOD_POINTER
		};
		/// query if a kind is an unsigned integral type
		inline bool is_unsigned_integral(kind k) { return k>=UCHAR && k<=UINT64; }
		inline bool is_signed_integral(kind k)   { return k>=CHAR && k<=INT64; }
		inline bool is_integral(kind k)          { return k>=CHAR && k<=UINT64; }
		inline bool is_floating(kind k)          { return k>=FLOAT && k<=DOUBLE; }
		inline bool is_number(kind k)            { return k>=CHAR && k<=DOUBLE; }
		inline bool is_fundamental(kind k)       { return k>=CHAR && k<=ENUM; }
		/// return a const char* with the name for a given kind
		inline const char* get_name_of_kind(kind k)
		{
			static const char* names[] = { 
				"typename", 
				"char", "int16", "int32", "int64", 
				"uchar", "uint16", "uint32", "uint64",
				"float", "double", 
				"bool", "wchar", 
				"enum", 
				"string", 
				"function_pointer",
				"pointer", "reference",
				"struct", "class", "union",
				"member_pointer", "method_pointer" 
			};
			return names[k];
		}
	}
}
