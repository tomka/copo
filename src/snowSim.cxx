#pragma comment(lib, "glew32.lib")

#include <GUI/window.h>
#include <fltk/run.h>
#include <MainControler.h>

#define VORONOI_TEST 0

// tell FLTK that we want to use GLEW
//#define USE_GLEW 1


#if VORONOI_TEST
#include <ios>      // Required for streamsize
#include <iostream>
#include <istream>
#include <limits>   // Required for numeric_limits
#include <MeshRepresentation/Voronoi/IncDelaunay.h>
#include <MeshRepresentation/Voronoi/geom2d.h>
#include <Utilities/Random.h>
#include <Utilities/Timer.h>
#endif

#if VORONOI_TEST
void myflush ( std::istream& in )
{
  //in.ignore ( std::numeric_limits<std::streamsize>::max(), '\n' );
  in.clear();
}

void mypause()
{
  std::cout<<"Press [Enter] to continue . . .";
  std::cin.get();
}
#endif


int main(void) {

#if VORONOI_TEST

	std::cout << "Testing Voronoi Insertion Time" << std::endl;

	Utilities::Random ran = Utilities::Random();
	Utilities::Timer timer = Utilities::Timer();

	/* Last Test:
			Testing Voronoi Insertion Time
			generating 1000000 points...ready
			Time: 146.369 msec
			inserting...ready
			Time: 724831 msec
			Total: 724978 msec
	*/

	double vertices[8];
	vertices[0] = 0.0;
	vertices[1] = 0.0;

	vertices[2] = 1000.0;
	vertices[3] = 0.0;

	vertices[4] = 1000.0;
	vertices[5] = 1000.0;

	vertices[6] = 0.0;
	vertices[7] = 1000.0;

	int count = 1000000;

	std::vector<Point2d> pointlist = std::vector<Point2d>(count);
	std::cout << "\tgenerating "<< count << " points...";

	int i = count;
	timer.start();
	while(i--) {
		Real x = ran.randomDouble(1.0, 999.0);
		Real y = ran.randomDouble(1.0, 999.0);
		//std::cout << "x: " << x << " y: " << y << std::endl;
		pointlist[i] = Point2d(x, y);
	}
	timer.stop();
	double genTime = timer.diffTimeInMSec();

	std::cout << "ready" << std::endl << "\tTime: " << genTime << " msec" << std::endl;
	std::cout << "\tinserting...";
	IncDelaunay testDT = IncDelaunay(4, vertices);

	i = count;
	timer.start();
	while(i--) {
		//std::cout << i << ": (" << pointlist[i][0] << ", " << pointlist[i][1] << ")" << std::endl;
		testDT.InsertSite(pointlist[i]);
	}
	timer.stop();

	std::cout << "ready" << std::endl << "\tTime: " << timer.diffTimeInMSec() << " msec" << std::endl << "\tTotal: " << genTime + timer.diffTimeInMSec() << " msec" << std::endl;

	mypause();

#else
	// init fltk multi thread support
	fltk::lock();

	//create window
	SnowSim::GUI::window sw(fltk::USEDEFAULT,fltk::USEDEFAULT,800,624);

	//SnowSim::MainControler::Init(&sw);

	//show window
	sw.show();
	//run
#endif

	return fltk::run();
}
