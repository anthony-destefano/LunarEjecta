#include "lunarEjecta_GeneralExpressions.h"
#include "lunarEjecta_FractalIntegration.h"

using namespace std;

#include <vector>
#include <cmath>
#include <iostream>

lunarEjecta_FractalIntegration::lunarEjecta_FractalIntegration
		(double new_xMin,
		 double new_xMax,
		 double new_yMin,
		 double new_yMax,
		 double new_epsError)
{
	xMin = new_xMin;
	xMax = new_xMax;
	yMin = new_yMin;
	yMax = new_yMax;

	epsError = new_epsError;

	levelMin = 4;
	levelMax = 10;
	levelCur = -1;

	// init size of quarry set and reduced sum, including the last level
	quarrySet.resize(levelMax+1);
	reducedSum.resize(levelMax+1, 0.0);
	
	// to level 0
	h_increaseLevel();
	h_evalLevel_reduce(levelCur);

	evalIntegral();

	//h_evalLevel_reduce(levelCur);

	// h_increaseLevel();
	// h_increaseLevel();
	// h_increaseLevel();
	// h_increaseLevel();
	// h_increaseLevel();
	// h_increaseLevel();
	// h_increaseLevel();
	// h_increaseLevel();
	// h_increaseLevel();
	// h_increaseLevel();
	// h_increaseLevel();
	//h_increaseLevel();
	//h_increaseLevel();
}


lunarEjecta_FractalIntegration::~lunarEjecta_FractalIntegration() {}

// Note, we can probably apply Richardson extrapolation here,
//  but I'm not sure how the error goes... so maybe later
double lunarEjecta_FractalIntegration::evalIntegral()
{
	double curError = 10.*epsError;
	double richarsonSum;


	while(levelCur <= levelMin || (curError > epsError && levelCur <= levelMax))
	{
		// increase level and eval the points
		h_increaseLevel();
		h_evalLevel_reduce(levelCur);

		// compute relative error
		curError = fabs((reducedSum[levelCur] - reducedSum[levelCur-1]) / reducedSum[levelCur]);
	
		richarsonSum = reducedSum[levelCur] + (reducedSum[levelCur] - reducedSum[levelCur-1])
			         / ( pow(sqrt((pow(4., levelCur+1)-1.)/(pow(4., levelCur)-1.)), 2.) - 1.);

		cout << "***level = " << levelCur << endl;
		cout << "   sum = " << reducedSum[levelCur] << endl;
		cout << "   richarsonSum = " << richarsonSum << endl;
		cout << "   relative error = " << curError << " | " << epsError << endl;
		//cout << "   true error = " << abs((reducedSum[levelCur] - sin(1.))/(sin(1.))) << endl;
		cout << "   true error = " << abs((reducedSum[levelCur] - PI/4.)/(PI/4.)) << endl;
	}

	return reducedSum[levelCur];
}


void lunarEjecta_FractalIntegration::printQuarryPoints()
{
	int ilev, jpoint;
	int Npoints;

	for (ilev = 0; ilev <= levelCur; ++ilev)
	{
		Npoints = hh_getNumQuarryPointsAtLevel(ilev);

		for (jpoint = 0; jpoint < Npoints; ++jpoint)
		{
			hh_printSetPoint(&quarrySet[ilev][jpoint]); 
		}
	}
}

void lunarEjecta_FractalIntegration::printQuarryPointsIfEval() {
	int ilev, jpoint;
	int Npoints;

	for (ilev = 0; ilev <= levelCur; ++ilev)
	{
		Npoints = hh_getNumQuarryPointsAtLevel(ilev);

		for (jpoint = 0; jpoint < Npoints; ++jpoint)
		{
			if(quarrySet[ilev][jpoint].isEval)
				hh_printSetPoint(&quarrySet[ilev][jpoint]); 
		}
	}
}





void lunarEjecta_FractalIntegration::h_increaseLevel()
{

	if(levelCur == levelMax) {
		cout << " Increase level failed... reached max level = " << levelMax << endl;
		return;
	}

	levelCur++;
	cout << "---level increased to: " << levelCur << endl;

	int Npoints = hh_getNumQuarryPointsAtLevel(levelCur);
	int NpointsPrev;
	double dx = (xMax + xMin) / double(2 << levelCur);
	double dy = (yMax + yMin) / double(2 << levelCur);
	double x, y;
	cout << "   dx | dy = " << dx << ' ' << dy << endl;
	cout << "   Point in level = " << Npoints << endl;
	cout << "   Point total = " << hh_getNumQuarryPointsTotal(levelCur) << endl;

	quarrySet[levelCur].resize(Npoints);

	if (levelCur == 0) // base case
	{
		x = dx;
		y = dy;

		hh_initSet(&quarrySet[0][0], x, y);
	}
	else // all other cases > level 0
	{
		NpointsPrev = hh_getNumQuarryPointsAtLevel(levelCur-1);

		// loop through locations of all previous points
		//  adding 4 new points arround each
		for (int i = 0; i < NpointsPrev; ++i)
		{
			// get position of previous point
			x = quarrySet[levelCur-1][i].loc.x;
			y = quarrySet[levelCur-1][i].loc.y;

			// add 4 new points around previous point
			hh_initSet(&quarrySet[levelCur][4*i    ], x - dx, y - dy);
			hh_initSet(&quarrySet[levelCur][4*i + 1], x + dx, y - dy);
			hh_initSet(&quarrySet[levelCur][4*i + 2], x - dx, y + dy);
			hh_initSet(&quarrySet[levelCur][4*i + 3], x + dx, y + dy);
		}
	}
}


void lunarEjecta_FractalIntegration::h_evalLevel_reduce(int lev)
{
	int i;
	int Npoints = hh_getNumQuarryPointsAtLevel(levelCur);
	int NpTotal = hh_getNumQuarryPointsTotal(levelCur);
	vector<double> v;

	double x, y;
	double dx = (xMax - xMin) / sqrt(double(NpTotal));
	double dy = (yMax - yMin) / sqrt(double(NpTotal));

	// init sum to zero
	reducedSum[levelCur] = 0.0;

	// loop through points at current level
	for (i = 0; i < Npoints; ++i)
	{
		x = quarrySet[levelCur][i].loc.x;
		y = quarrySet[levelCur][i].loc.y;

		quarrySet[levelCur][i].evalVal = integrand(x, y, dx, dy, v);

		// Check if we need to evaluate function
		// this in temporary
		if(y > sqrt(1.-sqr(x)) && y < cos(x)) {
		//if( y < cos(x)) {
			quarrySet[levelCur][i].isEval = 1;
			reducedSum[levelCur] += quarrySet[levelCur][i].evalVal;
		}
		else
		{
			quarrySet[levelCur][i].isEval = 0;
		}
	}

	// renormalize and reduce previous levels
		reducedSum[levelCur] += h_renormReduce(levelCur-1, dx, dy);
	
}

double lunarEjecta_FractalIntegration::h_renormReduce(int lev, double dx, double dy)
{
	int NpTotal = hh_getNumQuarryPointsTotal(lev);
	double dxlev = (xMax - xMin) / sqrt(double(NpTotal));
	double dylev = (yMax - yMin) / sqrt(double(NpTotal));

	return reducedSum[lev] * ((dx * dy) / (dxlev * dylev));
}


void lunarEjecta_FractalIntegration::hh_initSet(set* s, double x, double y)
{
	s->loc.x = x;
	s->loc.y = y;

	s->isEval = 0;
	s->evalVal = 0.0;

	s->dist = y; // this is just for a test
	//hh_printSetPoint(s);
	//HH_calcDist(x, v);
}

double lunarEjecta_FractalIntegration::integrand
                (double x, // location of center
		         double y, //  of eval
		         double Dx, // width of eval in x
		         double Dy, // width of eval in y
		         vector<double>& vv) // other params needed (should pack beforehand)
{
	return Dx * Dy;
}


int lunarEjecta_FractalIntegration::hh_getNumQuarryPointsTotal(int lev)
{
	return (double(4 << (2*lev)) - 1.) / 3.;
}

int lunarEjecta_FractalIntegration::hh_getNumQuarryPointsAtLevel(int lev)
{
	return 1 << 2*lev;
}


void lunarEjecta_FractalIntegration::hh_printSetPoint(set* s)
{
	cout << s->loc.x << " " <<  s->loc.y << endl;
}

double lunarEjecta_FractalIntegration::HH_calcDist(double x, double v) {
	return 1./PI * atan2(2.*sqr(v) * (1.-x) * sqrt(x*(2.-x)) , 1. - 2.*sqr(v) * x*(2.-x));
}