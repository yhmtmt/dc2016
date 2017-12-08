#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
using namespace std;

#include "dclib.h"



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
  
  // skip first line
  aisdata data; // data object
  skip_line(fin);
  while(load_ais_record(fin, data)){
    // Printing out
    cout << "MMSI: " << data.mmsi 
	 << " Date: " << data.year << "-" << data.mon << "-" << data.day << "-" << data.t
	 << " ROT: " << data.rot << " SOG: " 
	 << data.sog << " LAT: " << data.lat << " LON: " << data.lon 
	 << " COG: " << data.cog << " HDG: " << data.hdg 
	 << " ECEF: ("<< data.x << "," << data.y << "," << data.z << ")" << endl;						
  }
  return 0;
}
