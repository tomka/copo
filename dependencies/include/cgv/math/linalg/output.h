#pragma once
#include <iostream>
#include <cgv/math/linalg/col_vec.h>
#include <cgv/math/linalg/row_vec.h>
#include <cgv/math/linalg/mat.h>
#include <cgv/math/linalg/diag_mat.h>
#include <cgv/math/linalg/sqr_mat.h>
#include <cgv/math/linalg/sym_mat.h>
#include <cgv/math/linalg/lo_tri_mat.h>
#include <cgv/math/linalg/up_tri_mat.h>



template<class T>
std::ostream& operator<<(std::ostream& out,const cgv::math::linalg::row_vec<T>& v)
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


template<class T>
std::ostream& operator<<(std::ostream& out,const cgv::math::linalg::col_vec<T>& v)
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

template<class T>
std::ostream& operator<<(std::ostream& out,const cgv::math::linalg::mat<T>& m)
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
			out << ","; 
	}
	out << ")";
	return out;
}

template<class T>
std::ostream& operator<<(std::ostream& out,const cgv::math::linalg::sqr_mat<T>& m)
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
			//out <<",";
			out <<"\n";
	}
	out << ")";
	return out;
}


template<class T>
std::ostream& operator<<(std::ostream& out,const cgv::math::linalg::sym_mat<T>& m)
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

template<class T>
std::ostream& operator<<(std::ostream& out,const cgv::math::linalg::lo_tri_mat<T>& m)
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



template<class T>
std::ostream& operator<<(std::ostream& out,const cgv::math::linalg::up_tri_mat<T>& m)
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



template<class T>
std::ostream& operator<<(std::ostream& out,const cgv::math::linalg::diag_mat<T>& m)
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


	
