#ifndef LUNAREJECTA_AZM_DIST_MAP_H
#define LUNAREJECTA_AZM_DIST_MAP_H

#include "LunarEjecta_params.h"

using namespace std;

struct hist2DSet
{
	int N_D_perRegion;
	int N_azm_perRegion;

	vector<vector<double>> D_bin_edge;
	vector<vector<double>> D_bin_center;
	vector<vector<double>> azm_bin_edge;
	vector<vector<double>> azm_bin_center;
	vector<vector<double>> bin_weight;
};


struct hist3DSet
{
	// number of bins in each direction
	int N_D;
	int N_bearing_POI;
	int N_latlon;
	int N_horz_ang;

	// min and max for each lat-lon, bearing_ROI always spans 0 to 2*PI
	vector<double> D_min;
	vector<double> D_max;
	vector<double> bearing_POI_min;
	vector<double> bearing_POI_max;
	vector<int> N_bearing_ROI; // [horizon angle] 
	vector<int> N_bearing_ROI_cum; // cummulative sum of N_bearing_ROI [horizon angle] 

	vector<double> bin;
	//vector<vector<vector<vector<vector<double>>>>> bin; // bin[lat-lon][horizon angle][distance][bearing POI][bearing ROI]
};



double get_lat_from_pole(double D, double bearing);
double get_lon_from_pole(double D, double bearing);
double get_lat_from_ROI(double D, double bearing, double lat_R, double lon_R);
double get_lon_from_ROI(double D, double bearing, double lat_R, double lon_R);

void get_lat_min_max(double cur_lat, double d_lat, double& lat_min, double& lat_max);

hist2DSet* init_azm_dist_map(input* params);


#endif 