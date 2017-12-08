
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
  double x, y, z;		// ecef coordinate
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

void bihtoecef(const double lat, const double lon, const double alt, 
	       double & x, double & y, double & z)
{
  double slat = sin(lat);
  double clat = cos(lat);
  double slon = sin(lon);
  double clon = cos(lon);
  double N = AE / sqrt(1 - EE2 * slat * slat);
  
  double tmp = (N + alt) * clat;
  x = (double)(tmp * clon);
  y = (double)(tmp * slon);
  z = (double)((N * (1 - EE2) + alt) * slat);
}

void skip_line(ifstream & fin)
{
  char buf[1024];
  fin.getline(buf, 1024);
}

bool load_ais_record(ifstream & fin, aisdata & data)
{
  char buf[1024];
  char * token[128];

  if(fin.eof()){
    return false;
  }

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
  data.rot = (double) atof(token[8]);
  data.sog = (double) atof(token[9]);
  data.lat = (double) atof(token[11]);
  data.lon = (double) atof(token[12]);
  data.cog = (double) atof(token[13]);
  data.hdg = (short) atoi(token[14]);
  
  bihtoecef(
	    (double)(data.lat * (PI / 180.)), 
	    (double)(data.lon * (PI / 180.)),
	    0.0, data.x, data.y, data.z);

  return true;
}


