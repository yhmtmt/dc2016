#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
using namespace std;
#include "dclib.h"

struct shiplog{
  unsigned int mmsi;
  unsigned short year_s, year_e;
  unsigned short mon_s, mon_e;
  unsigned short day_s, day_e;
  unsigned int t_s, t_e;
  double lat_s, lat_e;
  double lon_s, lon_e;
  float x_s, y_s, z_s, x_e, y_e, z_e;
  double dist;
  
  shiplog():mmsi(0), year_s(0), year_e(0),
	    mon_s(0), mon_e(0), day_s(0), day_e(0),
	    t_s(0), t_e(0), lat_s(0), lat_e(0), lon_s(0),
	    lon_e(0), dist(0), x_s(0), y_s(0), z_s(0), 
	    x_e(0), y_e(0), z_e(0)
  {}
};


int main(int argc, char ** argv)
{
  if(argc != 2){
    cerr << "dataloader <file path>" << endl;
    return 1;
  }
  
  ifstream fin(argv[1]);
  if(!fin.is_open()){
    cerr << "Failed to open file " << argv[1] << "." << endl;
    return 1;
  }
  
  
  char * token[128];
  aisdata data; // data object
  shiplog shiplogs[2000];
  
  // skip first line
  skip_line(fin);
  
  while(load_ais_record(fin, data)){
    int iship = 0;
    for(iship = 0; iship < 2000; iship++){
      if(shiplogs[iship].mmsi == 0){
	cout << iship << "th ship found. (MMSI:" << data.mmsi << ")" << endl;
	shiplog & log = shiplogs[iship];
	log.mmsi = data.mmsi;
	log.year_e = log.year_s = data.year;
	log.mon_e = log.mon_s = data.mon;
	log.day_e = log.day_s = data.day;
	log.t_e = log.t_s = data.t;
	log.lon_e = log.lon_s = data.lon;
	log.lat_e = log.lat_s = data.lat;
	log.x_e = log.x_s = data.x;
	log.y_e = log.y_s = data.y;
	log.z_e = log.z_s = data.z;
	break;
      }
      if(shiplogs[iship].mmsi == data.mmsi){
	shiplog & log = shiplogs[iship];
	log.year_e = data.year;
	log.mon_e = data.mon;
	log.day_e = data.day;
	log.t_e = data.t;
	log.lon_e = data.lon;
	log.lat_e = data.lat;
	double dx = data.x - log.x_e;
	double dy = data.y - log.y_e;
	double dz = data.z - log.z_e;
	double ddist = sqrt(dx * dx + dy * dy + dz * dz);
	log.x_e = data.x;
	log.y_e = data.y;
	log.z_e = data.z;
	log.dist += ddist;	
	break;
      }
    }    
  }
  
  cout << "Calculating total .... " << endl;
  double D = 0;
  int iship;
  for (iship = 0; iship < 2000; iship++){
    if(shiplogs[iship].mmsi == 0)
      break;
    cout << "MMSI: " << shiplogs[iship].mmsi << " Dist: " << shiplogs[iship].dist << "m" << endl;
    D += shiplogs[iship].dist;
  }
  cout << "done." << endl << endl;
  cout << "Total number of ships: " << iship << endl;
  cout << "Total travel distance: " << D << "m" << endl;
  return 0;
}
