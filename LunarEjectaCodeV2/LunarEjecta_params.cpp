#include "LunarEjecta_params.h"


#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;


void linspace(vector<double>& x, double xmin, double xmax, int Nx) {
	x.resize(Nx);
	for (int i = 0; i < Nx; ++i)
		x[i] = xmin + (xmax - xmin) * double(i) / double(Nx-1.);
}

void logspace(vector<double>& x, double xmin, double xmax, int Nx, int i0, int im) {
	x.resize(im-i0);
	for (int i = i0; i < im; ++i) {
		x[i-i0] = xmin * pow(xmax/xmin, double(i)/double(Nx-1.));
		//cout << x[i-i0] << endl;
	}
}

// input mass in units of grams, equation uses mass in kg
double NEO_integral_flux(double m)
{
	return 2.89E-11 * pow(m/1000., -0.9); // units of #/m^2/yr
}

double H_calcH11_C4(input* p)
{
	return 3.*p->HH11_k/(4.*PI) * pow(p->HH11_C1, 3.*p->HH11_mu);
}

input* init_input(string param_fn, int N_proc, int i_proc)
{
	string temp_s;
	input* p = new input;

	p->N_proc = N_proc;
	p->i_proc = i_proc;

	//////////////////////
	// Default parameters
	p->MEM_massMin = 1.E-6; // g
	p->MEM_massMax = 10.; // g
	p->NEO_massMin = 10.; // g
	p->NEO_massMax = 1.5708E15; // g, 1000 m diameter at 3 g/cc

	p->lunar_radius = 1737.4; // km
	p->lunar_escape_speed = 2.38; // km/s

	p->MEM_hiDens_mu    = 8.241; // log_e (not log_10)
	p->MEM_hiDens_sigma = 0.214;
	p->MEM_loDens_mu    = 6.753;
	p->MEM_loDens_sigma = 0.292;

	p->NEO_dens = 3000.; // kg/m^3
	//////////////////////

	//////////////////////
	// Required parameters

	getParam(param_fn, "Nlat", p->Nlat, 0);
	getParam(param_fn, "Nlon", p->Nlon, 0);

	p->Nlatlon_tot = p->Nlat * p->Nlon;
	p->N_loc       = ceil(p->Nlatlon_tot / double(p->N_proc));

	p->latlon_idx_min = p->i_proc * p->N_loc;
	p->latlon_idx_max = fmin((p->i_proc + 1) * p->N_loc - 1, p->Nlatlon_tot - 1);

	p->N_loc = p->latlon_idx_max - p->latlon_idx_min + 1; // fix number of locations (mostly for the last process)

	cout << "min and max lat-lon index (absolute) = " << p->latlon_idx_min << ", " << p->latlon_idx_max << " | for proc = " << p->N_loc << " | total = " << p->Nlatlon_tot << endl;

	p->lon_directory.resize(4);
	for (int i = 0; i < p->Nlon; ++i)
		getParam(param_fn, "MEMDirectoryName_" + to_string(i * 360/p->Nlon), p->lon_directory[i], 0);

	getParam(param_fn, "NEO_velDist_fn", p->NEO_velDist_fn, 0);
	getParam(param_fn, "readNEO_files", p->readNEO_files, 0);
	getParam(param_fn, "saveNEO_files", p->saveNEO_files, 0);

	getParam(param_fn, "ROI_radius", p->ROI_radius, 0); // km
	getParam(param_fn, "ROI_lat", p->ROI_lat, 0); // read in degrees
	p->ROI_lat *= PI/180.;
	getParam(param_fn, "ROI_lon", p->ROI_lon, 0); // read in degrees
	p->ROI_lon *= PI/180.;

	getParam(param_fn, "regolith_dens", p->regolith_dens, 0);
	getParam(param_fn, "regolith_type", temp_s, 0);


	p->HH11_nu = 0.4; // see footnote 5 of Housen Holsapple 2011

	if (temp_s == "rock")
	{
		p->HH11_porosity = 0.0;
		p->HH11_mu       = 0.55;
		p->HH11_C1       = 1.5;
		p->HH11_k        = 0.3;
	}
	else if (temp_s == "weaklyCementedBasalt")
	{
		p->HH11_porosity = 0.20;
		p->HH11_mu       = 0.46;
		p->HH11_C1       = 0.18;
		p->HH11_k        = 0.3;
	}
	else if (temp_s == "sand")
	{
		p->HH11_porosity = 0.35;
		p->HH11_mu       = 0.41;
		p->HH11_C1       = 0.55;
		p->HH11_k        = 0.3;
	}
	else if (temp_s == "glassMicroSpheres")
	{
		p->HH11_porosity = 0.36;
		p->HH11_mu       = 0.45;
		p->HH11_C1       = 1.0;
		p->HH11_k        = 0.5;
	}
	else if (temp_s == "sandFlyAsh")
	{
		p->HH11_porosity = 0.45;
		p->HH11_mu       = 0.4;
		p->HH11_C1       = 0.55;
		p->HH11_k        = 0.3;
	}
	else if (temp_s == "perliteSandMixture")
	{
		p->HH11_porosity = 0.6;
		p->HH11_mu       = 0.35;
		p->HH11_C1       = 0.6;
		p->HH11_k        = 0.32;
	}
	else
	{
		cout << "ERROR: invalid reolith type in init_input\n\n";
	}

	p->HH11_C4       = H_calcH11_C4(p);
	cout << "HH11 C4 = " << p->HH11_C4 << endl;

	//////////////////////
	return p;
}