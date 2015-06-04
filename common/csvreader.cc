#include "csvreader.hh"
#include "csvparser.hh"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

static const char* csv_header_topo_height = "X,Y,QUELLE_FC,GRUND_AEND,ART_AENDER,ART_AEND_1,DATUM_AEND,INKREMENTA,KARTOGRAF,HINWEIS_AE,BORDER_STA,SICHTBARKE,OBJEKTART,HOEHE,HOEHE_DM,RID1";
static const char* csv_header_topo_morph = "X,Y,QUELLE_FC,GRUND_AEND,ART_AENDER,ART_AEND_1,DATUM_AEND,INKREMENTA,KARTOGRAF,HINWEIS_AE,BORDER_STA,SICHTBARKE,OBJEKTART,NAME,BESCHRIFTE,BLOCKTYP,AUSRICHTUN,RID1";
static const char* csv_header_topo_named = "X,Y,QUELLE_FC,GRUND_AEND,ART_AENDER,ART_AEND_1,DATUM_AEND,INKREMENTA,KARTOGRAF,HINWEIS_AE,BORDER_STA,SICHTBARKE,OBJEKTART,NAME,BESCHRIFTE,SG,HOEHE,RID1";
static const char* csv_header_topo_shop = "X,Y,OBJECTID,OBJECTVAL,OBJECTORIG,YEAROFCHAN";
static const char* csv_header_scalespace = "X,Y,Z,LIFE,STRENGTH,CLASSIFICATION,SWISSTOPO,DATABASE";
static const char* csv_header_generic = "X,Y,Z,IMPORTANCE,CLASSIFICATION";

static const char* other_s = "OTHER";
static const char* peak_s = "PEAK";
static const char* pit_s = "PIT";
static const char* saddle_s = "SADDLE";
static const char* human_s = "HUMAN";

// GG WARN should correct here: string without _S
// -- bewware this will make files produced before modification uncorrect.
static const char* named_alpine_summit_100_s = "NAMED_ALPINE_SUMMIT_100_S";
static const char* named_main_summit_200_s = "NAMED_MAIN_SUMMIT_200";
static const char* named_summit_300_s = "NAMED_SUMMIT_300";

static const char* named_main_hill_400_s = "NAMED_MAIN_HILL_400";
static const char* named_hill_500_s = "NAMED_HILL_500";
static const char* named_rock_head_600_s = "NAMED_ROCK_HEAD_600";

static const char* named_pass_700_s = "NAMED_PASS_700";
static const char* named_road_pass_800_s = "NAMED_ROAD_PASS_800";

static const char* height_lfp1_100_s = "HEIGHT_LFP1_100";
static const char* height_lv95_200_s = "HEIGHT_LV95_200";
static const char* height_agnes_300_s = "HEIGHT_AGNES_300";

static const char* height_natural_400_s = "HEIGHT_NATURAL_400";
static const char* height_human_500_s = "HEIGHT_HUMAN_500";
static const char* height_lake_600_s = "HEIGHT_LAKE_600";

static const char* morph_doline_100_s = "MORPH_DOLINE_100";
static const char* morph_erratic_200_s = "MORPH_ERRATIC_200";
static const char* morph_block_300_s = "MORPH_BLOCK_300";
static const char* morph_sink_400_s = "MORPH_SINK_400";
static const char* undefined_s = "UNDEFINED";


SwisstopoType encode_swisstopo (DatabaseType dbt, int code)
{
    if (dbt == DB_TOPO_NAMED)
    {
	if (code == 100)
	    return NAMED_ALPINE_SUMMIT_100;
	else if (code == 200)
	    return NAMED_MAIN_SUMMIT_200;
	else if (code == 300)
	    return NAMED_SUMMIT_300;

	else if (code == 400)
	    return NAMED_MAIN_HILL_400;
	else if (code == 500)
	    return NAMED_HILL_500;
	else if (code == 600)
	    return NAMED_ROCK_HEAD_600;

	else if (code == 700)
	    return NAMED_PASS_700;
	else if (code == 800)
	    return NAMED_ROAD_PASS_800;
    }
    else if (dbt == DB_TOPO_HEIGHT)
    {
	if (code == 100)
	    return HEIGHT_LFP1_100;
	else if (code == 200)
	    return HEIGHT_LV95_200;
	else if (code == 300)
	    return HEIGHT_AGNES_300;

	else if (code == 400)
	    return HEIGHT_NATURAL_400;
	else if (code == 500)
	    return HEIGHT_HUMAN_500;
	else if (code == 600)
	    return HEIGHT_LAKE_600;
    }
    else if (dbt == DB_TOPO_MORPH)
    {
	if (code == 100)
	    return MORPH_DOLINE_100;
	else if (code == 200)
	    return MORPH_ERRATIC_200;
	else if (code == 300)
	    return MORPH_BLOCK_300;
	else if (code == 400)
	    return MORPH_SINK_400;    
    }
    
    eprint ("wrong type: dbt:%d, code:%d\n", dbt, code);
    return UNDEFINED;
}


SwisstopoType string2swisstopo (std::string s)
{
    if ( s == named_alpine_summit_100_s)
	return NAMED_ALPINE_SUMMIT_100;
    else if (s == named_main_summit_200_s)
	return NAMED_MAIN_SUMMIT_200;
    else if (s == named_summit_300_s)
	return NAMED_SUMMIT_300;

    else if (s == named_main_hill_400_s)
	return NAMED_MAIN_HILL_400;
    else if (s == named_hill_500_s)
	return NAMED_HILL_500;
    else if (s == named_rock_head_600_s)
	return NAMED_ROCK_HEAD_600;

    else if (s == named_pass_700_s)
	return NAMED_PASS_700;
    else if (s == named_road_pass_800_s)
	return NAMED_ROAD_PASS_800;

    else if (s == height_lfp1_100_s)
	return HEIGHT_LFP1_100;
    else if (s == height_lv95_200_s)
	return HEIGHT_LV95_200;
    else if (s == height_agnes_300_s)
	return HEIGHT_AGNES_300;

    else if (s == height_natural_400_s)
	return HEIGHT_NATURAL_400;
    else if (s == height_human_500_s)
	return HEIGHT_HUMAN_500;
    else if (s == height_lake_600_s)
	return HEIGHT_LAKE_600;

    else if (s == morph_doline_100_s)
	return MORPH_DOLINE_100;
    else if (s == morph_erratic_200_s)
	return MORPH_ERRATIC_200;
    else if (s == morph_block_300_s)
	return MORPH_BLOCK_300;
    else if (s == morph_sink_400_s)
	return MORPH_SINK_400;
    
    else if (s == undefined_s)
	return UNDEFINED; 

    
    eprint ("wrong type: %s\n", s.c_str());
    return UNDEFINED;
}

std::string swisstopo2string (SwisstopoType t)
{

    if (t == NAMED_ALPINE_SUMMIT_100)
	return named_alpine_summit_100_s;
    else if (t == NAMED_MAIN_SUMMIT_200)
	return named_main_summit_200_s;
    else if (t == NAMED_SUMMIT_300)
	return named_summit_300_s;

    else if (t == NAMED_MAIN_HILL_400)
	return named_main_hill_400_s;
    else if (t == NAMED_HILL_500)
	return named_hill_500_s;
    else if (t == NAMED_ROCK_HEAD_600)
	return named_rock_head_600_s;

    else if (t == NAMED_PASS_700)
	return named_pass_700_s;
    else if (t == NAMED_ROAD_PASS_800)
	return named_road_pass_800_s;

    else if (t == HEIGHT_LFP1_100)
	return height_lfp1_100_s;
    else if (t == HEIGHT_LV95_200)
	return height_lv95_200_s;
    else if (t == HEIGHT_AGNES_300)
	return height_agnes_300_s;

    else if (t == HEIGHT_NATURAL_400)
	return height_natural_400_s;
    else if (t == HEIGHT_HUMAN_500)
	return height_human_500_s;
    else if (t == HEIGHT_LAKE_600)
	return height_lake_600_s;

    else if (t == MORPH_DOLINE_100)
	return morph_doline_100_s;
    else if (t == MORPH_ERRATIC_200)
	return morph_erratic_200_s;
    else if (t == MORPH_BLOCK_300)
	return morph_block_300_s;
    else if (t == MORPH_SINK_400)
	return morph_sink_400_s;
    else if (t == UNDEFINED)
	return undefined_s;
    
    eprint ("wrong type: %d\n", t);
    return "UNDEFINED";
}




// .named_alpine_summit_100- NAMED_ALPINE_SUMMIT_100,
// .named_main_summit_200- NAMED_MAIN_SUMMIT_200,
// .named_summit_300- NAMED_SUMMIT_300,

// .named_main_hill_400- NAMED_MAIN_HILL_400,
// .named_hill_500- NAMED_HILL_500,
// .named_rock_head_600- NAMED_ROCK_HEAD_600,

// .named_pass_700- NAMED_PASS_700,
// .named_road_pass_800- NAMED_ROAD_PASS_800,

// .height_lfp1_100- HEIGHT_LFP1_100,
// .height_lv95_200- HEIGHT_LV95_200,
// .height_agnes_300- HEIGHT_AGNES_300,

// .height_natural_400- HEIGHT_NATURAL_400,
// .height_human_500- HEIGHT_HUMAN_500,
// .height_lake_600- HEIGHT_LAKE_600,

// .morph_doline_100- MORPH_DOLINE_100,
// .morph_erratic_200- MORPH_ERRATIC_200,
// .morph_block_300- MORPH_BLOCK_300,
// .morph_sink_400- MORPH_SINK_400,


DatabaseType header2databasetype (char* header)
{
    DatabaseType dbt;
    
    if ((std::string(header)) == csv_header_topo_shop)
	db_set (dbt, DB_TOPO_SHOP);
    else if ((std::string(header)) == csv_header_topo_morph)
	db_set (dbt, DB_TOPO_MORPH);
    else if ((std::string(header)) == csv_header_topo_named)
	db_set (dbt, DB_TOPO_NAMED);
    else if ((std::string(header)) == csv_header_topo_height)
	db_set (dbt, DB_TOPO_HEIGHT);
    else if ((std::string(header)) == csv_header_scalespace)
	db_set (dbt, DB_SCALESPACE);
    else if ((std::string(header)) == csv_header_generic)
	db_set (dbt, DB_GENERIC);
    else
	eprintx (-1, "unrecognized swiss csv format. header:\n\t%s\n", header);

    return dbt;
}


    
bool is_sametype (CriticalType ct, ClassifiedType st)
{
    if (ct == MAX && st == PEAK)
	return true;
    if (ct == MIN && st == PIT)
	return true;
    if ((ct == SA2 || ct == SA3) && st == SADDLE)
	return true;
    if ((ct == SA2 || ct == SA3 || ct == MAX || ct == MIN) && st == ALL)
	return true;
    
    return false;
}


ClassifiedType string2classified (std::string s)
{
    if (s == other_s)
	return OTHER;
    else if (s == peak_s)
	return PEAK;
    else if (s == pit_s)
	return PIT;
    else if (s == saddle_s)
	return SADDLE;
    else if (s == human_s)
	return HUMAN;

    eprint ("wrong type: `%s'\n", s.c_str());
    return OTHER;
}

std::string classified2string (ClassifiedType t)
{
    if (t == OTHER)
	return other_s;
    else if (t == PEAK)
	return peak_s;
    else if (t == PIT)
	return pit_s;
    else if (t == SADDLE)
	return saddle_s;
    else if (t == HUMAN)
	return human_s;

    eprint ("wrong type: %d\n", t);
    return "";
}

ClassifiedType critical2classified (CriticalType c)
{
    switch (c)
    {
    case REG:
	return OTHER;
    case MAX:
	return PEAK;
    case MIN:
	return PIT;
    case SA2:
    case SA3:
	return SADDLE;
    case EQU:
    default:
	;
    }
    eprint ("Wrong type %d\n", c);
    return OTHER;
}

CriticalType classified2critical (ClassifiedType c)
{
    switch (c)
    {
    case HUMAN:
    case OTHER:
	return REG;
    case PEAK:
	return MAX;
    case PIT:
	return MIN;
    case SADDLE:
	return SA2;
    default:
	;
    }
    eprint ("Wrong type %d\n", c);
    return REG;
}


CSVReader::CSVReader () : asch() {}

CSVReader::CSVReader (ASCReader& ascr) : asch (ascr) {}

CSVReader::CSVReader (ASCHeader& asch) : asch (asch) {}

// CSVReader::CSVReader () :
//     width (0), height (0), cellsize (0),
//     xllcorner (0), yllcorner (0) {}

// CSVReader::CSVReader (ASCReader& ascr) :
//     width (ascr.width), height (ascr.height), cellsize (ascr.cellsize),
//     xllcorner (ascr.xllcorner), yllcorner (ascr.yllcorner) {}


    
// CSVReader::CSVReader (int width, int height,
// 		      double cellsize, double xllcorner, double yllcorner) :
//     width (width), height (height), cellsize (cellsize),
//     xllcorner (xllcorner), yllcorner (yllcorner) {}

void CSVReader::asc2img (Point a, Point* i)
{
    asch.asc2img (a, i);
}

void CSVReader::img2asc (Point i, Point* a)
{
    asch.img2asc (i, a);
}

Point CSVReader::asc2img (Point a)
{
    return asch.asc2img (a);
}

Point CSVReader::img2asc (Point i)
{
    return asch.img2asc (i);
}


void CSVReader::load (const char* filename, std::vector<SwissSpotHeight>& points)
{
    load (filename, points, 0.0);
}

void CSVReader::load (const char* filename, std::vector<SwissSpotHeight>& points, double cut)
{
    points.clear ();
    
    FILE *fp = fopen (filename, "r");
    if (fp == NULL)
	eprintx (2, "Could not open file `%s'. %s\n", filename, strerror (errno));

    // header
    CSVParser csvp (fp);
    char* header = csvp.parse_header ();
    if (header == NULL)	
	eprintx (2, "No header in file `%s'. %s\n", filename, strerror (errno));

    DatabaseType dbt = header2databasetype (header);

    int points_read = 0;
    // for every line
    while (true)
    {
	Point pa;

	// fill sh depending on whatever
	SwissSpotHeight sh;

	switch (dbt)
	{
	case DB_TOPO_SHOP:
	    {
		// X,Y,OBJECTID,OBJECTVAL,OBJECTORIG,YEAROFCHAN
		if (csvp.parse_next() != 6)
		    goto outta_loop;
		pa.x = atof (csvp.field (0));
		pa.y = atof (csvp.field (1));
		
		sh.dbt = dbt;
	    }
	    break;

	case DB_SCALESPACE:
	    {
		//"X,Y,Z,LIFE,STRENGTH,CLASSIFICATION,SWISSTOPO,DATABASE";
		if (csvp.parse_next() != 8)
		    goto outta_loop;
		pa.x = atof (csvp.field (0));
		pa.y = atof (csvp.field (1));
		sh.z = atof (csvp.field (2));
		sh.life = atof (csvp.field (3));
		sh.strength = atof (csvp.field (4));
		sh.ct = string2classified (csvp.field (5));
		sh.st = string2swisstopo (csvp.field (6));
		sh.dbt = atoi (csvp.field (7));
	    }
	    break;
	    
	case DB_GENERIC:
	    {
		//"X,Y,Z,IMPORTANCE,CLASSIFICATION";
		if (csvp.parse_next() != 5)
		    goto outta_loop;
		pa.x = atof (csvp.field (0));
		pa.y = atof (csvp.field (1));
		sh.z = atof (csvp.field (2));
		sh.life = atof (csvp.field (3));
		sh.strength = atof (csvp.field (3));
		sh.ct = string2classified (csvp.field (4));
		sh.st = UNDEFINED;
		sh.dbt = dbt;
	    }
	    break;
	    
	case DB_TOPO_NAMED:
	    {
		//"X,Y,QUELLE_FC,GRUND_AEND,ART_AENDER,ART_AEND_1,DATUM_AEND,INKREMENTA,KARTOGRAF,HINWEIS_AE,BORDER_STA,SICHTBARKE,OBJEKTART,NAME,BESCHRIFTE,SG,HOEHE,RID1";
		if (csvp.parse_next() != 18)
		    goto outta_loop;
		pa.x = atof (csvp.field (0));
		pa.y = atof (csvp.field (1));
		sh.life = sh.strength = -1.0;
		sh.ct = OTHER;
		sh.st = encode_swisstopo (dbt, atoi (csvp.field (12)));
		sh.dbt = dbt;
	    }
	    break;
	    
	case DB_TOPO_HEIGHT:
	    {
		//"X,Y,QUELLE_FC,GRUND_AEND,ART_AENDER,ART_AEND_1,DATUM_AEND,INKREMENTA,KARTOGRAF,HINWEIS_AE,BORDER_STA,SICHTBARKE,OBJEKTART,HOEHE,HOEHE_DM,RID1";
		if (csvp.parse_next() != 16)
		    goto outta_loop;
		pa.x = atof (csvp.field (0));
		pa.y = atof (csvp.field (1));
		sh.life = sh.strength = -1.0;
		sh.ct = OTHER;
		sh.st = encode_swisstopo (dbt, atoi (csvp.field (12)));
		
		sh.dbt = dbt;
	    }
	    break;
	    
	case DB_TOPO_MORPH:
	    {
		//"X,Y,QUELLE_FC,GRUND_AEND,ART_AENDER,ART_AEND_1,DATUM_AEND,INKREMENTA,KARTOGRAF,HINWEIS_AE,BORDER_STA,SICHTBARKE,OBJEKTART,NAME,BESCHRIFTE,BLOCKTYP,AUSRICHTUN,RID1";
		if (csvp.parse_next() != 18)
		    goto outta_loop;
		pa.x = atof (csvp.field (0));
		pa.y = atof (csvp.field (1));
		sh.life = sh.strength = -1.0;
		sh.ct = OTHER;
		sh.st = encode_swisstopo (dbt, atoi (csvp.field (12)));
		
		sh.dbt = dbt;
	    }
	    break;
	    
	default:
	    eprintx (-1, "Impossible db type %d\n", dbt);
	    break;
	}

	asc2img (pa, &sh.p);
	// sh.oid = id;
	if (sh.p.is_inside ((double) asch.width, (double) asch.height, cut))
	    points.push_back (sh);
	// else
	//     printf ("[%d], p(%lf,%lf) dim[%d,%d]\n", points_read, sh.p.x, sh.p.y, asch.width, asch.height);
	
	points_read++;
    }

 outta_loop:
    tprints (SCOPE_CSVREADER, "loaded %zu/%d points from file %s\n",
	   points.size (), points_read, filename);
    fclose (fp);
}

void CSVReader::save (const char* filename, std::vector<SwissSpotHeight>& points)
{
    // save (filename, points, false);
    FILE *fp = fopen (filename, "w");
    if (fp == NULL)
	eprintx (2, "Could not open file `%s'. %s\n", filename, strerror (errno));

    fprintf (fp, "X,Y,Z,LIFE,STRENGTH,CLASSIFICATION,SWISSTOPO,DATABASE\n");
    
    for (unsigned i = 0; i < points.size(); i++)
    {
	Point pa;
	img2asc (points[i].p, &pa);

	fprintf (fp, "%lf,%lf,%lf,%lf,%lf,%s,%s,%d\n",
		 pa.x, pa.y, points[i].z, points[i].life, points[i].strength,
		 (classified2string (points[i].ct)).c_str (),
		 (swisstopo2string (points[i].st)).c_str (),
		 points[i].dbt);
    }

    fclose (fp);
}

// void CSVReader::save (const char* filename, std::vector<SwissSpotHeight>& points,
// 		      bool save_types)
// {
//     FILE *fp = fopen (filename, "w");
//     if (fp == NULL)
// 	eprintx (2, "Could not open file `%s'. %s\n", filename, strerror (errno));

//     fprintf (fp, "X,Y,OBJECTID,OBJECTVAL,OBJECTORIG,YEAROFCHAN%s%s\n",
// 	     save_types? "," : "",
// 	     save_types? "TYPE" : "");
    
//     for (unsigned i = 0; i < points.size(); i++)
//     {
// 	Point pa;
// 	img2asc (points[i].p, &pa);

// 	fprintf (fp, "%lf,%lf,%d,Hoehenkote,LK25,1986%s%s\n",
// 		 pa.x, pa.y, points[i].oid, save_types? "," : "",
// 		 save_types? (classified2string (points[i].t)).c_str() : "");
//     }

//     fclose (fp);
// }

void CSVReader::save (const char* filename, std::vector<int>& spots,
		      Track* track, ScaleSpace* ssp)
{
    std::vector<SwissSpotHeight> vsh;
    
    for (unsigned i = 0; i < spots.size(); i++)
    {
    	int idx = spots[i];
    	// coords:      track->lines[idx].entries[0].c,
    	// type:        track->original_type (idx)
    	// importance:  track->lifetime_elixir (idx)

    	Point pi (track->lines[idx].entries[0].c.x + 0.5,
    		  track->lines[idx].entries[0].c.y + 0.5);
    	double vv = (*(ssp->dem[0]))(track->lines[idx].entries[0].c.x,
    					 track->lines[idx].entries[0].c.y);
	
	SwissSpotHeight sh (pi);
	sh.z = vv;
	sh.ct = critical2classified (track->original_type (idx));
	sh.st = UNDEFINED;
	sh.dbt = DB_TRACK;
	sh.life = track->lifetime_elixir (idx);
	sh.strength = track->strength (idx);
	vsh.push_back (sh);
    }

    save (filename, vsh);
}    

void CSVReader::save (const char* filename, std::vector<int>& spots,
		      std::vector<double>& rendersize, Track* track, ScaleSpace* ssp)
{
    if (rendersize.size () != spots.size ())
	return save (filename, spots, track, ssp);
	
    std::vector<SwissSpotHeight> vsh;
    
    for (unsigned i = 0; i < spots.size(); i++)
    {
    	int idx = spots[i];
    	// coords:      track->lines[idx].entries[0].c,
    	// type:        track->original_type (idx)
    	// importance:  track->lifetime_elixir (idx)

    	Point pi (track->lines[idx].entries[0].c.x + 0.5,
    		  track->lines[idx].entries[0].c.y + 0.5);
    	double vv = (*(ssp->dem[0]))(track->lines[idx].entries[0].c.x,
				     track->lines[idx].entries[0].c.y);
	
	SwissSpotHeight sh (pi);
	sh.z = vv;
	sh.ct = critical2classified (track->original_type (idx));
	sh.st = UNDEFINED;
	sh.dbt = DB_TRACK;
	sh.life = track->lifetime_elixir (idx);
	sh.strength = track->strength (idx);
	vsh.push_back (sh);
    }
    
    // save (filename, points, false);
    FILE *fp = fopen (filename, "w");
    if (fp == NULL)
	eprintx (2, "Could not open file `%s'. %s\n", filename, strerror (errno));

    fprintf (fp, "X,Y,Z,LIFE,STRENGTH,RENDERSIZE,CLASSIFICATION,SWISSTOPO,DATABASE\n");
    
    for (unsigned i = 0; i < vsh.size(); i++)
    {
	Point pa;
	img2asc (vsh[i].p, &pa);

	fprintf (fp, "%lf,%lf,%lf,%lf,%lf,%lf,%s,%s,%d\n",
		 pa.x, pa.y, vsh[i].z, vsh[i].life, vsh[i].strength,
		 rendersize[i],
		 (classified2string (vsh[i].ct)).c_str (),
		 (swisstopo2string (vsh[i].st)).c_str (),
		 vsh[i].dbt);
    }

    fclose (fp);
}

void CSVReader::save (const char* filename, ScaleSpace* ssp)
{
    FILE* fp = fopen (filename, "w");
	
    if (fp == NULL)
	eprint ("Could not write file %s\n", filename);	
	
    fprintf (fp, "X,Y,IMG_X,IMG_Y,TYPE, LEVEL,IDX\n");
    for (int i = 0; i < ssp->levels; i++)
	for (unsigned j = 0; j < ssp->critical[i].size(); j++)
	{
	    Coord c = ssp->critical[i][j].c;
	    Point pi = coord2point (c);
	    Point pa = img2asc (pi);
	    fprintf (fp, "%lf,%lf,%d,%d,%s,%d,%d\n", 
		     pa.x, pa.y, c.x, c.y,
		     (classified2string
		      (critical2classified
		       (ssp->critical[i][j].t))).c_str (), i, j
		     );
	}
	
    fclose (fp);
}


    // FILE *fp = fopen (filename, "w");
    // if (fp == NULL)
    // 	eprintx (2, "Could not open file `%s'. %s\n", filename, strerror (errno));
    
    // fprintf (fp, "X,Y,Z,TYPE,STRENGTH\n");
    
    // for (unsigned i = 0; i < spots.size(); i++)
    // {
    // 	int idx = spots[i];
    // 	// coords:      track->lines[idx].entries[0].c,
    // 	// type:        track->original_type (idx)
    // 	// importance:  track->lifetime_elixir (idx)

    // 	Point pi (track->lines[idx].entries[0].c.x + 0.5,
    // 		  track->lines[idx].entries[0].c.y + 0.5);
    // 	double height = (*(ssp->dem[0]))(track->lines[idx].entries[0].c.x,
    // 					 track->lines[idx].entries[0].c.y);
    // 	Point pa;
    // 	img2asc (pi, &pa);
	    
    // 	fprintf (fp, "%lf,%lf,%lf,%s,%lf,\n", pa.x, pa.y, height,
    // 		 critical2string (track->original_type (idx)),
    // 		 track->lifetime_elixir (idx));
    // }

    // fclose (fp);
