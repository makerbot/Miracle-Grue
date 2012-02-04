#include <fstream>
#include <cmath>

#include <omp.h>

#include <cppunit/config/SourcePrefix.h>
#include "RegionerTestCase.h"

#include "mgl/abstractable.h"
#include "mgl/configuration.h"

//#include "../PathData.h"
//#include "../RegionerOperation.h"

#include "ctime"

using namespace mgl;
using namespace std;


CPPUNIT_TEST_SUITE_REGISTRATION( RegionerTestCase );

#define SINGLE_EXTRUDER_FILE_NAME "v29_single_xtruder_warmup.gcode"
#define DUAL_EXTRUDER_FILE_NAME "v29_dual_xtruder_warmup.gcode"


class Timer
{
	unsigned int t0;


public:
	std::vector<double> times;

	Timer()
	{
		reset();
	}

	void reset()
	{
		this->t0 = clock();
		//cout << "clock " << this->t0 << endl;
		times.clear();
	}

	double time()
	{
		//cout << "x " << t0 << endl;
		unsigned int t = clock()-t0;
		double s =  t *0.000001;
		times.push_back(s);
		return s;
	}
};

void RegionerTestCase::setUp()
{

}

void RegionerTestCase::example()
{
	cout << endl <<__FUNCTION__ << endl;
/*
	RegionerOperation operation;
	PathData *inputData = new PathData(0.2);
	RegionData *outputData = new RegionData(0.2, 0.4);

//  CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, 1.1, 0.05 );
//  CPPUNIT_ASSERT( 1 == 0 );
  CPPUNIT_ASSERT( 1 == 1 );

  long* l1 = new long(12);
  long* l2 = new long(12);

  CPPUNIT_ASSERT_EQUAL( 12, 12 );
  CPPUNIT_ASSERT_EQUAL( 12L, 12L );
  CPPUNIT_ASSERT_EQUAL( *l1, *l2 );

  delete l1;
  delete l2;
  inputData->release();
  outputData->release();

  CPPUNIT_ASSERT( 12L == 12L );
//  CPPUNIT_ASSERT_EQUAL( 12, 13 );
//  CPPUNIT_ASSERT_DOUBLES_EQUAL( 12.0, 11.99, 0.5 );
*/
}

void stall()
{
	Timer t;
	cout << " * start " << omp_get_thread_num() << endl;
	int j=0;
	for(int i=0; i < 200000000; i++)
	{
		j += i * 10 - sin(i*4);
	}
	cout << "stalled for " << t.time() << " in thread " << omp_get_thread_num() << endl;
}

double single()
{
    long result = 0;
    long i;


    for (i = 0; i < 10; i++) {
        stall();
    	result++;
    }
    cout << "result " << result << endl;
    return result;
}

double multi()
{

    long result = 0;
    long i;

    long stuff = 1;

    int nthreads = omp_get_num_threads();
    printf("Number of threads before = %d\n", nthreads);
	#pragma omp parallel for// reduction(+: result)
    for (i = 0; i < 10; i++) {
    	stall();
    	result++;
    	if (i==0) nthreads = omp_get_num_threads();
    }
    printf("Number of threads during = %d\n", nthreads);
    return result;
}



void RegionerTestCase::testMp()
{
	cout << endl <<__FUNCTION__ << endl;

	printf("### Number of host CPUs:\t%d\n", omp_get_num_procs());
	int nthreads = omp_get_num_threads();
//	printf("Number of threads = %d\n", nthreads);

	MyComputer theMatrix;
	cout << endl << theMatrix.clock.now() << endl;
	cout << "Turbo!" << endl;
	Timer t2;
	cout << "result =" << multi()<< endl;
	cout << t2.time() << " seconds" << endl;
	cout << "********\n*************\ndone" << endl;
	sleep(1);

	cout << endl << theMatrix.clock.now() << endl;
	cout << endl;
	cout << "Single thread" << endl;
	Timer t;
	cout << "result =" << single()<< endl;
	cout << t.time() << " seconds" << endl;
	cout << endl;
	cout << endl << theMatrix.clock.now() << endl;
	cout << endl;
}


