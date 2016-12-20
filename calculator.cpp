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
	shiplog shiplogs[2000];

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

		// Printing out
		/*
		cout << "MMSI: " << data.mmsi 
			<< " Date: " << data.year << "-" << data.mon << "-" << data.day << "-" << data.t
			<< " ROT: " << data.rot << " SOG: " 
			<< data.sog << " LAT: " << data.lat << " LON: " << data.lon 
			<< " COG: " << data.cog << " HDG: " << data.hdg 
			<< " ECEF: ("<< data.x << "," << data.y << "," << data.z << ")" << endl;						
			*/
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