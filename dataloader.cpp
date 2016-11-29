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
	float x, y, z;		// ecef coordinate
	float cog;			// 13. course over ground 
	short hdg;			// 14. heading
};

#define PI 3.1415926535898			
#define AE 6378137.					// radius at the equator  
#define FE 1/298.257223563			// earth flatness
#define BE (AE*(1.-FE))
#define EE2 (2.*FE-FE*FE) 
#define EE2_A ((AE*AE-BE*BE)/(AE))
#define EE2_B ((AE*AE-BE*BE)/(BE))
#define RE 6371229.3				// radius of the earth
#define TERREF 0.0784				// terrestrial refraction coefficient
#define TERREF2 2.07
#define KNOT 0.514444444			/* in meter/sec*/
#define MILE 1852					/* in meter */

void bihtoecef(const float lat, const float lon, const float alt, 
			   float & x, float & y, float & z)
{
	double slat = sin(lat);
	double clat = cos(lat);
	double slon = sin(lon);
	double clon = cos(lon);
	double N = AE / sqrt(1 - EE2 * slat * slat);

	double tmp = (N + alt) * clat;
	x = (float)(tmp * clon);
	y = (float)(tmp * slon);
	z = (float)((N * (1 - EE2) + alt) * slat);
}

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
		
		bihtoecef(
			(float)(data.lat * (PI / 180.)), 
			(float)(data.lon * (PI / 180.)),
			0.0, data.x, data.y, data.z);
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