#pragma once


//matrix types
#include <cgv/math/linalg/col_vec.h>
#include <cgv/math/linalg/row_vec.h>
#include <cgv/math/linalg/mat.h>
#include <cgv/math/linalg/sqr_mat.h>
#include <cgv/math/linalg/diag_mat.h>
#include <cgv/math/linalg/sym_mat.h>
#include <cgv/math/linalg/lo_tri_mat.h>
#include <cgv/math/linalg/up_tri_mat.h>


#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_row_vec.h>
#include <cgv/math/linalg/fix_mat.h>
#include <cgv/math/linalg/fix_sqr_mat.h>
#include <cgv/math/linalg/fix_diag_mat.h>
#include <cgv/math/linalg/fix_sym_mat.h>
#include <cgv/math/linalg/fix_lo_tri_mat.h>
#include <cgv/math/linalg/fix_up_tri_mat.h>
#include <cgv/math/linalg/fix_quaternion.h>

//translate(1.0,0.0,2.0),rotate_x(1.0), rotate_y(2.0),rotate_z(3.0), rotate(0.0,1.0,0.0,1.4),scale(1.0),
//scale(1.0,2.0,1.0),...
#include <cgv/math/linalg/fix_mat_construct.h>

//std::cout << a;
#include <cgv/math/linalg/output.h>
#include <cgv/math/linalg/fix_output.h>

//transpose(a)
#include <cgv/math/linalg/transpose.h>
#include <cgv/math/linalg/fix_transpose.h>
//a+b
#include <cgv/math/linalg/add.h>
#include <cgv/math/linalg/fix_add.h>
//a-b, -a
#include <cgv/math/linalg/sub.h>
#include <cgv/math/linalg/fix_sub.h>
//3*a
#include <cgv/math/linalg/scl_mult.h>
#include <cgv/math/linalg/fix_scl_mult.h>
//a*b
#include <cgv/math/linalg/mult.h>
#include <cgv/math/linalg/fix_mult.h>
//dot(a,b),cross(a,b), dbl_cross(a,b,c), spat(a,b,c)
#include <cgv/math/linalg/vec_prod.h>
#include <cgv/math/linalg/fix_vec_prod.h>
//trace(a)
#include <cgv/math/linalg/trace.h>
#include <cgv/math/linalg/fix_trace.h>
//invert(a)
#include <cgv/math/linalg/invert.h>
#include <cgv/math/linalg/fix_invert.h>
//length(a)
#include <cgv/math/linalg/length.h>
#include <cgv/math/linalg/fix_length.h>
//normalize(a)
#include <cgv/math/linalg/normalize.h>
#include <cgv/math/linalg/fix_normalize.h>
//det(a)
#include <cgv/math/linalg/det.h>
#include <cgv/math/linalg/fix_det.h>
//trace(a)
#include <cgv/math/linalg/trace.h>
#include <cgv/math/linalg/fix_trace.h>
//homogenize(a)
#include <cgv/math/linalg/homogenize.h>
#include <cgv/math/linalg/fix_homogenize.h>
//svd(a,b,c)
#include <cgv/math/linalg/svd.h>
#include <cgv/math/linalg/fix_svd.h>

#include <cgv/math/linalg/cholesky_factorization.h>
#include <cgv/math/linalg/fix_cholesky_factorization.h>
//direct LES solver
#include <cgv/math/linalg/gauss_LES_solver.h>
#include <cgv/math/linalg/fix_gauss_LES_solver.h>
#include <cgv/math/linalg/cholesky_LES_solver.h>
#include <cgv/math/linalg/fix_cholesky_LES_solver.h>
///iterative LES solver
#include <cgv/math/linalg/gauss_seidel_LES_solver.h>
#include <cgv/math/linalg/fix_gauss_seidel_LES_solver.h>
#include <cgv/math/linalg/jacobi_LES_solver.h>
#include <cgv/math/linalg/fix_jacobi_LES_solver.h>
#include <cgv/math/linalg/cg_LES_solver.h>
#include <cgv/math/linalg/fix_cg_LES_solver.h>

#include <cgv/math/linalg/fix_reflect.h>
#include <cgv/math/linalg/fix_refract.h>


