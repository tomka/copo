#pragma once
#include <iostream>
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_row_vec.h>
#include <cgv/math/linalg/fix_mat.h>
#include <cgv/math/linalg/fix_diag_mat.h>
#include <cgv/math/linalg/fix_sqr_mat.h>
#include <cgv/math/linalg/fix_sym_mat.h>
#include <cgv/math/linalg/fix_up_tri_mat.h>
#include <cgv/math/linalg/fix_lo_tri_mat.h>
#include <cgv/math/linalg/fix_quaternion.h>



template <typename T, cgv::type::uint32 N>
std::ostream& operator<<(std::ostream& out,const cgv::math::linalg::fix_row_vec<T,N>& v)
{
	out << "(";
	for(int i=0;i < v.size();i++)
	{
		out << v(i);
		if(i < v.size()-1)
			out <<", ";	
	}
	out << ")";
	return out;

}

template <typename T>
std::ostream& operator<<(std::ostream& out,const cgv::math::linalg::fix_quaternion<T>& q)
{
	out << q.w() << " + " << q.x() << "i + " << q.y()<<"j + "<<q.z()<<"k";
	return out;

}



template <typename T, cgv::type::uint32 N>
std::ostream& operator<<(std::ostream& out,const cgv::math::linalg::fix_col_vec<T,N>& v)
{
	out << "(";
	for(int i=0;i < v.size();i++)
	{
		out << v(i);
		if(i < v.size()-1)
			out <<", ";	
	}
	out << ")^T";
	return out;

}


template <typename T, cgv::type::uint32 N,cgv::type::uint32 M>
std::ostream& operator<<(std::ostream& out,const cgv::math::linalg::fix_mat<T,N,M>& m)
{
	out << "(";
	for(int i = 0; i < m.nrows(); i++)
	{
		out << "(";
		for(int j = 0; j < m.ncols();j++)
		{
			out << m(i,j);
			if(j < m.ncols()-1)
				out <<", ";	
		}
		out <<")";
		if(i <m.nrows()-1)
			out <<",";
	}
	out << ")";
	return out;
}

template <typename T, cgv::type::uint32 N>
std::ostream& operator<<(std::ostream& out,const cgv::math::linalg::fix_sym_mat<T,N>& m)
{
	out << "(";
	for(int i = 0; i < m.nrows(); i++)
	{
		out << "(";
		for(int j = 0; j < m.ncols();j++)
		{
			out << m(i,j);
			if(j < m.ncols()-1)
				out <<", ";	
		}
		out <<")";
		if(i <m.nrows()-1)
			out <<",";
	}
	out << ")";
	return out;
}


template <typename T, cgv::type::uint32 N>
std::ostream& operator<<(std::ostream& out,const cgv::math::linalg::fix_lo_tri_mat<T,N>& m)
{
	out << "(";
	for(int i = 0; i < m.nrows(); i++)
	{
		out << "(";
		for(int j = 0; j < m.ncols();j++)
		{
			out << m(i,j);
			if(j < m.ncols()-1)
				out <<", ";	
		}
		out <<")";
		if(i <m.nrows()-1)
			out <<",";
	}
	out << ")";
	return out;
}



template <typename T, cgv::type::uint32 N>
std::ostream& operator<<(std::ostream& out,const cgv::math::linalg::fix_up_tri_mat<T,N>& m)
{
	out << "(";
	for(int i = 0; i < m.nrows(); i++)
	{
		out << "(";
		for(int j = 0; j < m.ncols();j++)
		{
			out << m(i,j);
			if(j < m.ncols()-1)
				out <<", ";	
		}
		out <<")";
		if(i <m.nrows()-1)
			out <<",";
	}
	out << ")";
	return out;
}



template <typename T, cgv::type::uint32 N>
std::ostream& operator<<(std::ostream& out,const cgv::math::linalg::fix_sqr_mat<T,N>& m)
{
	out << "(";
	for(int i = 0; i < m.nrows(); i++)
	{
		out << "(";
		for(int j = 0; j < m.ncols();j++)
		{
			out << m(i,j);
			if(j < m.ncols()-1)
				out <<", ";	
		}
		out <<")";
		if(i <m.nrows()-1)
			out <<",";
	}
	out << ")";
	return out;
}


template <typename T, cgv::type::uint32 N>
std::ostream& operator<<(std::ostream& out,const cgv::math::linalg::fix_diag_mat<T,N>& m)
{
	out << "(";
	for(int i = 0; i < m.nrows(); i++)
	{
		out << "(";
		for(int j = 0; j < m.ncols();j++)
		{
			if(i!=j)
				out <<"0";
			else
				out << m(i);
			if(j < m.ncols()-1)
				out <<", ";	
		}
		out <<")";
		if(i <m.nrows()-1)
			out <<",";
	}
	out << ")";
	return out;
}


	
