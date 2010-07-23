#include <boost/test/unit_test.hpp>
#include <types.h>
#include <utilities.h>
#include <utilities.cpp>
#include <vector>

// Tests for the 'b' class.
BOOST_AUTO_TEST_SUITE(utilities_tests)

BOOST_AUTO_TEST_CASE(isLeft_left_handed_2d_TestCase_1)
{
	point3f p1(0.0, 0.0, 0.0);
	point3f p2(0.0, 0.0, 1.0);
	point3f left(1.0, 0.0, 0.0);
	point3f right(-1.0, 0.0, 0.0);
	point3f on1(0.0, 0.0, -1.0);
	point3f on2(0.0, 0.0, 2.0);

	float result = isLeft_right_handed_2d(p1, p2, left);
	BOOST_CHECK_GT(result, 0);

	result = isLeft_right_handed_2d(p1, p2, right);
	BOOST_CHECK_LT(result, 0);

	result = isLeft_right_handed_2d(p1, p2, on1);
	BOOST_CHECK_EQUAL(result, 0);

	result = isLeft_right_handed_2d(p1, p2, on2);
	BOOST_CHECK_EQUAL(result, 0);

	point3f p3(-3.0, 0.0, 1.0);
	point3f p4(1.0, 0.0, 1.0);
	point3f above(1.0, 0.0, -1.0);
	result = isLeft_right_handed_2d(p3, p4, above);
	BOOST_CHECK_GT(result,0);

	point3f below(-4.0, 0.0, 11.0);
	result = isLeft_right_handed_2d(p3, p4, below);
	BOOST_CHECK_LT(result,0);
}

BOOST_AUTO_TEST_CASE(isLeft_left_handed_2d_TestCase_2)
{
	point3f tri1_p1(-0.5, 0.0,  0.5);
	point3f tri1_p2( 0.5, 0.0,  0.5);
	point3f tri1_p3(-0.5, 0.0, -0.5);


	point3f p1(0.140893, 0.0, 0.248);
	float result = isLeft_right_handed_2d(tri1_p1, tri1_p2, p1);
	BOOST_CHECK_GT(result, 0.0f);

	point3f p2(-0.7214, 0.0, 0.660589);
	result = isLeft_right_handed_2d(tri1_p1, tri1_p2, p2);
	BOOST_CHECK_LT(result, 0.0f);
}

BOOST_AUTO_TEST_CASE(directs_to_left_2d_TestCase)
{
	point3f line1_p1(-1.0, 0.0, 5.0);
	point3f line1_p2(10.0, 0.0, 5.0);
	point3f line2_p1(1.0, 0.0, -2.0);
	point3f line2_p2(1.0, 0.0, 11.0);

	float result = directs_to_left_2d(line1_p1, line1_p2, line2_p1, line2_p2);
	BOOST_CHECK( result < 0.001f);

	result = directs_to_left_2d(line1_p1, line1_p2, line2_p2, line2_p1);
	BOOST_CHECK( result > 0.001f);

	result = directs_to_left_2d(line1_p1, line1_p2, line1_p1, line1_p2);
	BOOST_CHECK( abs(result)  < 0.001f);
}

BOOST_AUTO_TEST_CASE(segment_segment_intersect_TestCase)
{
	point3f line1_p1(-1.0, 0.0, 5.0);
	point3f line1_p2(10.0, 0.0, 5.0);
	point3f line2_p1(1.0, 0.0, -2.0);
	point3f line2_p2(1.0, 0.0, 11.0);
	point3f intersection;

	bool result = segment_segment_intersect(line1_p1, line1_p2, line2_p1, line2_p2, intersection);
	BOOST_CHECK( result == true );
	BOOST_CHECK_CLOSE(intersection.x(), 1.0f, 0.0001f);
	BOOST_CHECK_CLOSE(intersection.z(), 5.0f, 0.0001f);

	point3f line3_p1(-1.0, 0.0, 3.0);
	point3f line3_p2(5.0, 0.0, 1.0);

	result = segment_segment_intersect(line1_p1, line1_p2, line3_p1, line3_p2, intersection);
	BOOST_CHECK( result == false );
	BOOST_CHECK_CLOSE(intersection.x(), 1.0f, 0.0001f);
	BOOST_CHECK_CLOSE(intersection.z(), 5.0f, 0.0001f);
}

BOOST_AUTO_TEST_CASE(ray_line_intersect_TestCase)
{
	point3f line1_p1(-1.0, 0.0, 5.0);
	point3f line1_p2(10.0, 0.0, 5.0);
	point3f line2_p1(1.0, 0.0, -2.0);
	point3f line2_p2(1.0, 0.0, -1.0);
	point3f intersection;

	bool result = ray_line_intersect(line1_p1, line1_p2, line2_p1, line2_p2, false, intersection);
	BOOST_CHECK( result == true );
	BOOST_CHECK_CLOSE(intersection.x(), 1.0f, 0.0001f);
	BOOST_CHECK_CLOSE(intersection.z(), 5.0f, 0.0001f);

	result = ray_line_intersect(line1_p1, line1_p2, line2_p1, line2_p2, true, intersection);
	BOOST_CHECK( result == true );
	BOOST_CHECK_CLOSE(intersection.x(), 1.0f, 0.0001f);
	BOOST_CHECK_CLOSE(intersection.z(), 5.0f, 0.0001f);

	result = ray_line_intersect(line1_p1, line1_p2, line2_p2, line2_p1, true, intersection);
	BOOST_CHECK( result == true );
	BOOST_CHECK_CLOSE(intersection.x(), 1.0f, 0.0001f);
	BOOST_CHECK_CLOSE(intersection.z(), 5.0f, 0.0001f);

	result = ray_line_intersect(line1_p1, line1_p2, line2_p2, line2_p1, false, intersection);
	BOOST_CHECK( result == false );
	BOOST_CHECK_CLOSE(intersection.x(), 1.0f, 0.0001f);
	BOOST_CHECK_CLOSE(intersection.z(), 5.0f, 0.0001f);
}

BOOST_AUTO_TEST_CASE(point_in_triangle_2d_TestCase)
{
	point3f tri1_p1(1.0, 0.0, 4.5);
	point3f tri1_p2(1.0, 0.0, 7.0);
	point3f tri1_p3(3.5, 0.0, 7.0);

	std::vector<point3f> triangle;
	triangle.push_back(tri1_p1);
	triangle.push_back(tri1_p2);
	triangle.push_back(tri1_p3);

	point3f p1_in(1.5,0.0,6);
	bool result = point_in_triangle_2d(triangle, p1_in);
	BOOST_CHECK( result == true );

	point3f p2_out(6.5,0.0,6);
	result = point_in_triangle_2d(triangle, p2_out);
	BOOST_CHECK( result == false );
}

BOOST_AUTO_TEST_CASE(clip_ray_to_triangle_TestCase)
{
	point3f tri1_p1(1.0, 0.0, 4.5);
	point3f tri1_p2(1.0, 0.0, 7.0);
	point3f tri1_p3(3.5, 0.0, 7.0);

	std::vector<point3f> triangle;
	triangle.push_back(tri1_p1);
	triangle.push_back(tri1_p2);
	triangle.push_back(tri1_p3);

	point3f p1_in(1.5,0.0,6.0);
	point3f p1_down(1.5,0.0,6.1);
	bool result = clip_ray_to_polygon(triangle, p1_in, p1_down);
	BOOST_CHECK( result == true );
	BOOST_CHECK_CLOSE(p1_down.x(), 1.5f, 0.0001f);
	BOOST_CHECK_CLOSE(p1_down.z(), 7.0f, 0.0001f);

	point3f p2_in(1.5,0.0,6.0);
	point3f p2_left(0.0,0.0,6.0);
	result = clip_ray_to_polygon(triangle, p2_in, p2_left);
	BOOST_CHECK( result == true );
	BOOST_CHECK_CLOSE(p2_left.x(), 1.0f, 0.0001f);
	BOOST_CHECK_CLOSE(p2_left.z(), 6.0f, 0.0001f);

	point3f p3_out(-1.5,0.0,6.0);
	point3f p3_left(-2.5,0.0,6.0);
	result = clip_ray_to_polygon(triangle, p3_out, p3_left);
	BOOST_CHECK( result == false );

	point3f quad_p1(1.0, 0.0, 4.5);
	point3f quad_p2(1.0, 0.0, 7.0);
	point3f quad_p3(3.5, 0.0, 7.0);
	point3f quad_p4(3.5, 0.0, 4.5);

	std::vector<point3f> quad;
	quad.push_back(quad_p1);
	quad.push_back(quad_p2);
	quad.push_back(quad_p3);
	quad.push_back(quad_p4);

	point3f p4_in(1.5,0.0,6.0);
	point3f p4_left(0.0,0.0,6.0);
	result = clip_ray_to_polygon(quad, p4_in, p4_left);
	BOOST_CHECK( result == true );
	BOOST_CHECK_CLOSE(p4_left.x(), 1.0f, 0.0001f);
	BOOST_CHECK_CLOSE(p4_left.z(), 6.0f, 0.0001f);
}

BOOST_AUTO_TEST_CASE(intersect_ray_with_polygon_TestCase)
{
	point3f quad_p1(1.0, 0.0, 4.5);
	point3f quad_p2(1.0, 0.0, 7.0);
	point3f quad_p3(3.5, 0.0, 7.0);
	point3f quad_p4(3.5, 0.0, 4.5);

	std::vector<point3f> quad;
	quad.push_back(quad_p1);
	quad.push_back(quad_p2);
	quad.push_back(quad_p3);
	quad.push_back(quad_p4);

	int side_hit;

	point3f p1_in(1.5,0.0,6.0);
	point3f p1_left(0.0,0.0,6.0);
	bool result = intersect_ray_with_polygon(quad, p1_in, p1_left, side_hit);
	BOOST_CHECK( result == true );
	BOOST_CHECK_CLOSE(p1_left.x(), 1.0f, 0.0001f);
	BOOST_CHECK_CLOSE(p1_left.z(), 6.0f, 0.0001f);

	point3f p2_in(-1.5,0.0,6.0);
	point3f p2_left(-10,0.0,6.0);
	result = intersect_ray_with_polygon(quad, p2_in, p2_left, side_hit);
	BOOST_CHECK( result == true );
	BOOST_CHECK_CLOSE(p2_left.x(), 1.0f, 0.0001f);
	BOOST_CHECK_CLOSE(p2_left.z(), 6.0f, 0.0001f);
}

//BOOST_AUTO_TEST_CASE(clip_polygon_to_triangle_TestCase)
//{
//	// define triangle
//	point3f tri1_p1(1.0, 0.0, 4.5);
//	point3f tri1_p2(1.0, 0.0, 7.0);
//	point3f tri1_p3(3.5, 0.0, 7.0);
//
//	std::vector<point3f> triangle;
//	triangle.push_back(tri1_p1);
//	triangle.push_back(tri1_p2);
//	triangle.push_back(tri1_p3);
//
//	// Test 1, two overlapping triangles
//	// define polygon to clip
//	point3f tri2_p1(1.0, 0.0, 4.5);
//	point3f tri2_p2(1.0, 0.0, 7.0);
//	point3f tri2_p3(3.5, 0.0, 7.0);
//
//	std::vector<point3f> triangle2;
//	triangle.push_back(tri2_p1);
//	triangle.push_back(tri2_p2);
//	triangle.push_back(tri2_p3);
//
//	bool result = clip_polygon_to_triangle(triangle, triangle2);
//	BOOST_CHECK( result == true );
//	BOOST_CHECK_CLOSE(tri2_p1.x(), tri1_p1.x(), 0.0001f);
//	BOOST_CHECK_CLOSE(tri2_p1.z(), tri1_p1.z(), 0.0001f);
//	BOOST_CHECK_CLOSE(tri2_p2.x(), tri1_p2.x(), 0.0001f);
//	BOOST_CHECK_CLOSE(tri2_p2.z(), tri1_p2.z(), 0.0001f);
//	BOOST_CHECK_CLOSE(tri2_p3.x(), tri1_p3.x(), 0.0001f);
//	BOOST_CHECK_CLOSE(tri2_p3.z(), tri1_p3.z(), 0.0001f);
//
//	// Test 2, purely outside polygon
//	// define polygon to clip (convex)
//	point3f poly1_p1(15.0, 0.0, 4.5);
//	point3f poly1_p2(16.0, 0.0, 7.0);
//	point3f poly1_p3(22.5, 0.0, 7.0);
//	point3f poly1_p4(20.5, 0.0, 5.0);
//
//	std::vector<point3f> polygon1;
//	triangle.push_back(poly1_p1);
//	triangle.push_back(poly1_p2);
//	triangle.push_back(poly1_p3);
//	triangle.push_back(poly1_p4);
//
//	bool result = clip_polygon_to_triangle(triangle, polygon1);
//	BOOST_CHECK( result == false );
//}

BOOST_AUTO_TEST_CASE(clip_line_to_triangle_TestCase_1)
{
	point3f tri1_p1(1.0, 0.0, 4.5);
	point3f tri1_p2(1.0, 0.0, 7.0);
	point3f tri1_p3(3.5, 0.0, 7.0);

	std::vector<point3f> triangle;
	triangle.push_back(tri1_p1);
	triangle.push_back(tri1_p2);
	triangle.push_back(tri1_p3);

	// left to right
	point3f line1_p1(0.0, 0.0, 6.0);
	point3f line1_p2(1.5, 0.0, 6.0);

	bool result = clip_line_to_triangle(triangle, 3, line1_p1, line1_p2);
	BOOST_CHECK( result == true );
	BOOST_CHECK_CLOSE(line1_p1.x(), 1.0f, 0.0001f);
	BOOST_CHECK_CLOSE(line1_p1.z(), 6.0f, 0.0001f);
	BOOST_CHECK_CLOSE(line1_p2.x(), 1.5f, 0.0001f);
	BOOST_CHECK_CLOSE(line1_p2.z(), 6.0f, 0.0001f);

	// right to left
	point3f line2_p1(1.5, 0.0, 6.0);
	point3f line2_p2(0.0, 0.0, 6.0);

	result = clip_line_to_triangle(triangle, 3, line2_p1, line2_p2);
	BOOST_CHECK( result == true );
	BOOST_CHECK_CLOSE(line2_p1.x(), 1.5f, 0.0001f);
	BOOST_CHECK_CLOSE(line2_p1.z(), 6.0f, 0.0001f);
	BOOST_CHECK_CLOSE(line2_p2.x(), 1.0f, 0.0001f);
	BOOST_CHECK_CLOSE(line2_p2.z(), 6.0f, 0.0001f);

	// downwards
	point3f line3_p1(1.5, 0.0, 6.0);
	point3f line3_p2(1.5, 0.0, 12.5);

	result = clip_line_to_triangle(triangle, 3, line3_p1, line3_p2);
	BOOST_CHECK( result == true );
	BOOST_CHECK_CLOSE(line3_p1.x(), 1.5f, 0.0001f);
	BOOST_CHECK_CLOSE(line3_p1.z(), 6.0f, 0.0001f);
	BOOST_CHECK_CLOSE(line3_p2.x(), 1.5f, 0.0001f);
	BOOST_CHECK_CLOSE(line3_p2.z(), 7.0f, 0.0001f);

	// upwards
	point3f line4_p1(1.5, 0.0, 12.5);
	point3f line4_p2(1.5, 0.0, 6.0);

	result = clip_line_to_triangle(triangle, 3, line4_p1, line4_p2);
	BOOST_CHECK( result == true );
	BOOST_CHECK_CLOSE(line4_p1.x(), 1.5f, 0.0001f);
	BOOST_CHECK_CLOSE(line4_p1.z(), 7.0f, 0.0001f);
	BOOST_CHECK_CLOSE(line4_p2.x(), 1.5f, 0.0001f);
	BOOST_CHECK_CLOSE(line4_p2.z(), 6.0f, 0.0001f);

	// outside
	point3f line5_p1(1.5, 0.0, 12.5);
	point3f line5_p2(17.5, 0.0, 60.0);

	result = clip_line_to_triangle(triangle, 3, line5_p1, line5_p2);
	BOOST_CHECK( result == false );
	BOOST_CHECK_CLOSE(line5_p1.x(), 1.5f, 0.0001f);
	BOOST_CHECK_CLOSE(line5_p1.z(), 12.5f, 0.0001f);
	BOOST_CHECK_CLOSE(line5_p2.x(), 17.5f, 0.0001f);
	BOOST_CHECK_CLOSE(line5_p2.z(), 60.0f, 0.0001f);
}

BOOST_AUTO_TEST_CASE(clip_line_to_triangle_TestCase_2)
{
	point3f tri1_p1(-0.5, 0.0,  0.5);
	point3f tri1_p2( 0.5, 0.0,  0.5);
	point3f tri1_p3(-0.5, 0.0, -0.5);

	std::vector<point3f> triangle;
	triangle.push_back(tri1_p1);
	triangle.push_back(tri1_p2);
	triangle.push_back(tri1_p3);

	// right down
	point3f line1_p1(-0.332907, 0.0, 0.170831);
	point3f line1_p2(0.140873, 0.0, 0.248);

	bool result = clip_line_to_triangle(triangle, 3, line1_p1, line1_p2);
	BOOST_CHECK( result == true );
	BOOST_CHECK_CLOSE(line1_p1.x(), -0.332907f, 0.0001f);
	BOOST_CHECK_CLOSE(line1_p1.z(), 0.170831f, 0.0001f);
	BOOST_CHECK_CLOSE(line1_p2.x(), 0.140873f, 0.0001f);
	BOOST_CHECK_CLOSE(line1_p2.z(), 0.248f, 0.0001f);

	// left down
	point3f line2_p1(0.140873, 0.0, 0.248);
	point3f line2_p2(-0.7214, 0.0, 0.660589);

	result = clip_line_to_triangle(triangle, 3, line2_p1, line2_p2);
	BOOST_CHECK( result == true );
	BOOST_CHECK_CLOSE(line2_p1.x(), 0.140873f, 0.0001f);
	BOOST_CHECK_CLOSE(line2_p1.z(), 0.248f, 0.0001f);
	BOOST_CHECK_CLOSE(line2_p2.x(), -0.385783851f, 0.0001f);
	BOOST_CHECK_CLOSE(line2_p2.z(), 0.5f, 0.0001f);

	// right up
	point3f line3_p1(-0.7214, 0.0, 0.660589);
	point3f line3_p2(-0.332907, 0.0, 0.170831);

	result = clip_line_to_triangle(triangle, 3, line3_p1, line3_p2);
	BOOST_CHECK( result == true );
	BOOST_CHECK_CLOSE(line3_p1.x(), -0.5f, 0.0001f);
	BOOST_CHECK_CLOSE(line3_p1.z(), 0.381478608f, 0.0001f);
	BOOST_CHECK_CLOSE(line3_p2.x(), -0.332907f, 0.0001f);
	BOOST_CHECK_CLOSE(line3_p2.z(), 0.170831f, 0.0001f);
}

BOOST_AUTO_TEST_SUITE_END()
