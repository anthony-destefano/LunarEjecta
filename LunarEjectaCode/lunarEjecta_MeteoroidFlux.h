// Declaration of meteoroid flux information

// Have the option to read in cube_avg, flux_avg, and igloo_avg

#ifndef LUNAREJECTA_METEOROIDFLUX_H
#define LUNAREJECTA_METEOROIDFLUX_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "vector2d.h"

using namespace std;

// This is an abstract base class and cannot be used directly
class MEM_data
{
public:
	MEM_data(string fn);
	~MEM_data();

	inline double getFlux(int row, int col);
	inline double getRVar(int row, int col);
	inline double getCVar(int row, int col);

	inline double getNrows();
	inline double getNcols();
	inline double getNrowVars();
	inline double getNcolVars();

	
protected:
	virtual void H_readFile(void) = 0;
	void H_setHeaderLength(int h);
	void H_getRowCol_FromFile();
	void H_readInt_FromFile(ifstream& file, int& firstInt);
	inline int H_idxFlux(int row, int col);
	inline int H_idxRVar(int row, int col);
	inline int H_idxCVar(int row, int col);
	inline void H_storeFlux(int row, int col, double flux);
	inline void H_storeRVar(int row, int col, double RVar);
	inline void H_storeCVar(int row, int col, double RCar);

	// don't use these now
	inline void H_pushBackFlux(double flux); 
	inline void H_pushBackRVar(double RVar);
	inline void H_pushBackCVar(double CVar); 

	string fileName;
	int headerLength;

	int Nrows; // of data only
	int Ncols; // of data only
	int NrowVars;
	int NcolVars;

	vector<double> fluxData; // total size = Nrows * Ncols
	vector<double> rowVars;  // total size = Nrows * NrowVars
	vector<double> colVars;  // total size = Ncols * NcolVars

	string fluxUnits;            
	vector<string> rowVarsUnits; 
	vector<string> colVarsUnits; 
};

/////////////////////////////////////////////////////////////
class MEM_cubeAvg : public MEM_data
{
public:
	MEM_cubeAvg(string dn); // dn = directory name
	~MEM_cubeAvg();
private:
	void H_readFile();


};
/////////////////////////////////////////////////////////////
class MEM_fluxAvg : public MEM_data
{
public:
	MEM_fluxAvg(string dn);
	~MEM_fluxAvg();
private:
	void H_readFile();

};
/////////////////////////////////////////////////////////////
class MEM_iglooAvg : public MEM_data
{
public:
	MEM_iglooAvg(string dn);
	~MEM_iglooAvg();
private:
	void H_readFile();

};
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
class MEM_HiDensityCubeAvg : public MEM_cubeAvg
{
public:
	MEM_HiDensityCubeAvg(string dn);
	~MEM_HiDensityCubeAvg();
	
};
/////////////////////////////////////////////////////////////
class MEM_HiDensityFluxAvg : public MEM_fluxAvg
{
public:
	MEM_HiDensityFluxAvg(string dn);
	~MEM_HiDensityFluxAvg();
	
};
/////////////////////////////////////////////////////////////
class MEM_HiDensityIglooAvg : public MEM_iglooAvg
{
public:
	MEM_HiDensityIglooAvg(string dn);
	~MEM_HiDensityIglooAvg();
	
};
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
class MEM_LoDensityCubeAvg : public MEM_cubeAvg
{
public:
	MEM_LoDensityCubeAvg(string dn);
	~MEM_LoDensityCubeAvg();
	
};
/////////////////////////////////////////////////////////////
class MEM_LoDensityFluxAvg : public MEM_fluxAvg
{
public:
	MEM_LoDensityFluxAvg(string dn);
	~MEM_LoDensityFluxAvg();
	
};
/////////////////////////////////////////////////////////////
class MEM_LoDensityIglooAvg : public MEM_iglooAvg
{
public:
	MEM_LoDensityIglooAvg(string dn);
	~MEM_LoDensityIglooAvg();
	
};
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

// Note: a template class must be definied in the header file
template <class genMEMdata> 
class MEM_LatData 
{
public:
	MEM_LatData(string dn, double lMin, double lMax, int NL) // string dn, double lMin, double lMax, int NL
	{
		directoryName = dn;
		latMin        = lMin;
		latMax        = lMax;
		NLat          = NL;
		cout << "template class init \n";

		dataSet.resize(NL);
		string latDirectoryName;

		for (int i = 0; i < NLat; ++i)
		{
			latDirectoryName = "/lat" + to_string(int(latMin + (latMax-latMin)*i/(NL-1.0)));
			//cout << latDirectoryName << endl;
			dataSet[i] = new genMEMdata(dn + latDirectoryName);
		}
	}

	~MEM_LatData()
	{
		for (int i = 0; i < NLat; ++i)
		{
			delete dataSet[i];
			//cout << "delete " << i << endl;
		}
	}

	//void print();

private:
	string directoryName;
	vector<genMEMdata*> dataSet;

	double latMin;
	double latMax;
	int NLat;
	
};




#endif 