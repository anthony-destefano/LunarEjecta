// Declaration of regolith information

#ifndef LUNAREJECTA_REGOLITH_H
#define LUNAREJECTA_REGOLITH_H

using namespace std;

// HH11_targetMaterial
enum HH11_Table3Params {rock, weaklyCementedBasalt, sand, glassMicroSpheres, sandFlyAsh, perliteSandMixture};

enum regolithDensityType {DSNE, UserDefRegDens};

class lunarEjecta_Regolith
{
public:
	lunarEjecta_Regolith(int HH11_targetMaterial,
						 int regolithDensType,
						 double new_lowDensity,
					     double new_avgDensity,
						 double new_highDensity,
						 double new_radius,
						 double new_escapeSpeed);
	~lunarEjecta_Regolith();
	
	inline double getHH11_porosity();
	double getHH11_mu();
	inline double getHH11_C1();
	inline double getHH11_k();
	double getHH11_C4();
	double getHH11_nu();
	double getlowDensity();
	double getavgDensity();
	double gethighDensity();
	double getlunarRadius();
	double getlunarEscapeSpeed();

private:
	inline double H_calcHH11_C4();

	double HH11_porosity; // percent
	double HH11_mu;
	double HH11_C1;
	double HH11_k;
	double HH11_C4;
	double HH11_nu;

	double lowDensity;  // kg/m^3
	double avgDensity;  // kg/m^3
	double highDensity; // kg/m^3

	double lunarRadius; // m
	double lunarEscapeSpeed; // m/s
};
#endif 