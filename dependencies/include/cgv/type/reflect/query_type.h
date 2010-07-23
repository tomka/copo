#pragma once

#include <cgv/type/not_specialized.h>
#include <cgv/type/index_type.h>
#include <cgv/type/reflect/type.h>
#include <cgv/type/reflect/descriptors.h>
#include <cgv/type/reflect/type_interface.h>

namespace cgv {
	namespace type {
		namespace reflect {
			namespace detail {
				template <kind k, class T>
				struct number_type_interface : type_interface_base<T,true,true,false>
				{
					number_descriptor nd;
					kind get_kind() const             { return k; }
					/// return the number descriptor if get_kind() returns a fundamental number type
					number_descriptor*            N() { return &nd; }
					/// return the number descriptor if get_kind() returns a fundamental number type
					const number_descriptor*    get_number_descriptor() const { return &nd; }
				};
				template <kind k, class T>
				struct fundamental_type_interface : type_interface_base<T>
				{
					kind get_kind() const            { return k; }
				};
				/// access to the static_get_type method of the type interface
				template <typename T, bool ta_is_singleton>
				struct type_query
				{
					static const type_interface* get_type() { return type<T>::static_get_type(); }
				};
				template <typename T>
				struct type_query<T,false>
				{
					static const type_interface* get_type() { return 0; }
				};
				template <typename T, typename P, bool ta_is_const, bool ta_is_ref>
				struct pointer_type_interface_base : public type_interface_base<P,!ta_is_ref,true,false>
				{
					pointer_descriptor ptr;
					pointer_type_interface_base() : ptr(cgv::type::reflect::query_type<T>(),ta_is_const)  { }
					kind get_kind() const          { return ta_is_ref ? REFERENCE : POINTER; }
					const char* get_name() const   { return 0; }
					pointer_descriptor* P() { return &ptr; }
					/// return the array descriptor if get_kind() returns ARRAY
					const pointer_descriptor*     get_pointer_descriptor() const 
					{ 
						return &ptr; 
					}
				};
			}

template <> struct type<bool> : detail::fundamental_type_interface<BOOL,bool>
{
	const char* get_name() const { return "bool"; }
};

template <> struct type<char> : detail::number_type_interface<CHAR,char>
{
	const char* get_name() const { return "char"; }
};

template <> struct type<wchar_t> : detail::number_type_interface<WCHAR,wchar_t>
{
	const char* get_name() const { return "wchar"; }
};

template <> struct type<int16> : detail::number_type_interface<INT16,int16>
{
	const char* get_name() const { return "int16"; }
};
template <> struct type<int32> : detail::number_type_interface<INT32,int32>
{
	const char* get_name() const { return "int32"; }
};
template <> struct type<int64> : detail::number_type_interface<INT64,int64>
{
	const char* get_name() const { return "int64"; }
};
template <> struct type<unsigned char> : detail::fundamental_type_interface<UCHAR,unsigned char>
{
	const char* get_name() const { return "unsigned char"; }
};

template <> struct type<uint16> : detail::number_type_interface<UINT16,uint16>
{
	const char* get_name() const { return "uint16"; }
};
template <> struct type<uint32> : detail::number_type_interface<UINT32,uint32>
{
	const char* get_name() const { return "uint32"; }
};
template <> struct type<uint64> : detail::number_type_interface<UINT64,uint64>
{
	const char* get_name() const { return "uint64"; }
};

template <> struct type<float> : detail::number_type_interface<FLOAT,float>
{
	const char* get_name() const { return "float"; }
};
template <> struct type<double> : detail::number_type_interface<DOUBLE,double>
{
	const char* get_name() const { return "double"; }
};
template <> struct type<std::string> : detail::fundamental_type_interface<STRING,std::string>
{
	/// access to list of namespaces. The default implementation returns 0.
	const name_range* get_namespaces() const 
	{ 
		static name_range nr("std");
		return &nr;
	}
	const char* get_name() const { return "string"; }
};


/** return a const pointer to the type interface of the type specified
    as template argument or function argument.
*/
template <typename T>
unions::pointer_union<type_interface> query_type()
{
	return type<T>::static_query_type();
};

template <typename T>
struct type<T*> : public detail::pointer_type_interface_base<T,T*,false,false> {};
template <typename T>
struct type<const T*> : public detail::pointer_type_interface_base<T,const T*,true,false> {};
template <typename T>
struct type<T&> : public detail::pointer_type_interface_base<T,T&,false,true> {};
template <typename T>
struct type<const T&> : public detail::pointer_type_interface_base<T,const T&,true,true> {};

		}
	}
}