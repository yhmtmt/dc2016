#include <iostream>
#include <fstream>

using namespace std;

struct aisdata{
	unsigned int mmsi;	// 1. mmsi number 
	unsigned short year;// 2. year (0-3)
	unsigned short mon; // 2. month (4-5)
	unsigned short day; // 2. day (6-7)
	unsigned int t;		// 3. time in second
	float rot;			// 8. rate of turn
	float sog;			// 9. speed over ground
	double lat;			// 11. latitude
	double lon;			// 12. longitude
	float cog;			// 13. course over ground 
	short hdg;			// 14. heading
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

	char buf[1024];
	char * token[128];
	aisdata data; // data object

	// skip first line
	fin.getline(buf, 1024);

	while(!fin.eof()){
		fin.getline(buf, sizeof(buf));

		// tokenize
		char * p = buf;
		int ntoks = 0;
		bool newtok = true;
		while(*p != '\0'){
			if(newtok){
				token[ntoks] = p;
				ntoks++;
				newtok = false;
			}

			if(*p == ','){
				*p = '\0';
				newtok = true;
			}

			p++;
		}
		*p = '\0';

		// converting characters into values
		data.mmsi = strtoul(token[1], (char**)NULL, 10);

		p = token[2];
		data.year = (p[0] - '0') * 1000 + (p[1] - '0') * 100 + (p[2] - '0') * 10 + p[3] - '0';
		data.mon = (p[4] - '0') * 10 + (p[5] - '0');
		data.day = (p[6] - '0') * 10 + (p[7] - '0');

		data.t = strtoul(token[3], (char**)NULL, 10);
		data.rot = (float) atof(token[8]);
		data.sog = (float) atof(token[9]);
		data.lat = (float) atof(token[11]);
		data.lon = (float) atof(token[12]);
		data.cog = (float) atof(token[13]);
		data.hdg = (short) atoi(token[14]);
		
		// Printing out
		cout << "MMSI: " << data.mmsi 
			<< " Date: " << data.year << "-" << data.mon << "-" << data.day << "-" << data.t
			<< " ROT: " << data.rot << " SOG: " 
			<< data.sog << " LAT: " << data.lat << " LON: " << data.lon 
			<< " COG: " << data.cog << " HDG: " << data.hdg << endl;			
	}
	return 0;
}