#include "demreader.hh"
#include "csvreader.hh"
#include "track.hh"

#include <vector>
#include <algorithm>

#include <Eigen/Sparse>
#include <Eigen/Cholesky>
#include <Eigen/Dense>
#include <Eigen/Sparse>


int window = 2;
char *input_swiss_file = 0;
char *asc_file = 0;
char *output_swiss_file = "swissclassifier_output.csv";
char *track_file = 0;
double strength_threshold = 2.0;
bool correction_pre = false;
bool correction_pre_saddles = false;
bool perform_final_report_swiss = false;
bool perform_final_report_generic = false;

void print_help (FILE* f)
{
    fprintf (f, "Usage: swissclassifier\n"
	     // "[-s scalespacefile.ssp]\n"
	     "-i inputname : csv input file\n"
	     "-a asc file : for georegistration\n"
	     "[-t trk file] : activate optional refining step using tracking\n"
	     "[-S strength_threshold] : strength threshold for refining step\n"
	     "[-o outputname] : csv output file\n"
	     "[-W WINDOW] : pixel window for fit classification. default 2 (5x5)\n"
	     "[-c] : Correct unclassified points if a maximum or minumum is present\n"
	     "[-s] : Correct saddles too\n"
	     "[-r] : Print quality assesment report\n"
	     "\n"
	     );
}

void app_init(int argc, char *argv[])
{
    for (argc--, argv++; argc--; argv++)
    {
        if( (*argv)[0] == '-')
        {
            switch( (*argv)[1] )
            {
            case 'a':
                asc_file = (*++argv);
                argc--;
                break;

            case 'i':
                input_swiss_file = (*++argv);
                argc--;
                break;

            case 'c':
                correction_pre = true;
                break;

            case 's':
                correction_pre_saddles = true;
                break;

            case 't':
                track_file = (*++argv);
                argc--;
                break;

	    case 'o':
                output_swiss_file = (*++argv);
                argc--;
                break;

            case 'W':
                window = atoi (*++argv);
                argc--;
                break;

            case 'S':
                strength_threshold = atof (*++argv);
                argc--;
                break;

	    case 'R':
		perform_final_report_swiss = true;
		break;

	    case 'r':
		perform_final_report_generic = true;
		break;

            default:
                goto die;
            }
        }
    }

    if (input_swiss_file == 0 || asc_file == 0)
    {
	eprintx (-1, "%s\n", "No input file given.");
	goto die;
    }
    
    return;
	
 die:
    print_help(stderr);
    exit(1);
}


class Quadric
{
public:
    
    Quadric ()
    {
	a() = b() = c() = d() = e() = f() = 1.0;
    }
    
    Quadric(double av, double bv, double cv, double dv, double ev, double ef)
    {
	a() = av;
	b() = bv;
	c() = cv;
	d() = dv;
	e() = ev;
	f() = ef;
    }
    
    double& a() { return data[0];}
    double& b() { return data[1];}
    double& c() { return data[2];}
    double& d() { return data[3];}
    double& e() { return data[4];}
    double& f() { return data[5];}
    
    double data[6];
    
    double evaluate(double x, double y)
    {
	return a()*x*x + b()*x*y + c()*y*y + d()*x + e()*y + f();
    }
    
    double dx(double x, double y)
    {
	return 2.0*a()*x + b()*y + d();
    }
    
    double dy(double x, double y)
    {
	return 2.0*c()*y + b()*x + e();
    }
    
    double dxy(double x, double y)
    {
	(void) x, (void) y;
 	return b();
    }
    
    double dxx(double x, double y)
    {
	(void) x, (void) y;
	return 2.0*a();
    }
    
    double dyy(double x, double y)
    {
	(void) x, (void) y;
	return 2.0*c();
    }
    
    static Quadric fit(std::vector<Eigen::Vector3d> &VV)
    {
	using namespace std;
	assert(VV.size() >= 6);
	if (VV.size() < 6)
	{
	    cerr << "ASSERT FAILED!" << endl;
	    exit(0);
	}

	Eigen::MatrixXd AA(VV.size(),6);
	Eigen::MatrixXd bb(VV.size(),1);
	Eigen::MatrixXd sol(6,1);
      
	for(unsigned int i = 0; i < VV.size(); i++)
	{
	    double x = VV[i][0];
	    double y = VV[i][1];
	    double z = VV[i][2];
        
	    AA(i,0) = x*x;
	    AA(i,1) = x*y;
	    AA(i,2) = y*y;
	    AA(i,3) = x;
	    AA(i,4) = y;
	    AA(i,5) = 1;
        
	    bb(i) = z;
	}
      
	sol=AA.jacobiSvd (Eigen::ComputeThinU | Eigen::ComputeThinV).solve(bb);
	return Quadric (sol(0),sol(1),sol(2),sol(3),sol(4),sol(5));
    }

    // bool isZeroInWindow (double w, Point* zero)
    bool isZeroInWindow (double w)
    {
	// solve this
	// 2a  b | x = | -d
	//  b 2c | y   | -e
	Eigen::Matrix2d AA;
	Eigen::Vector2d bb;
	Eigen::Vector2d xx;

	AA(0,0) = 2.0 * a();
	AA(0,1) = b();
	AA(1,0) = b();
	AA(1,1) = 2.0 * c();

	bb(0) = -d();
	bb(1) = -e();
	xx = AA.fullPivHouseholderQr ().solve (bb);

	// if x,y is inside (-w,-w)(w,w) return true
	// zero->x = xx(0);
	// zero->y = xx(1);
	return
	    xx(0) > -w &&
	    xx(0) <  w &&
	    xx(1) > -w &&
	    xx(1) <  w;
    }

    // ClassifiedType studyHessian (Point zero)
    ClassifiedType studyHessian ()
    {
	// TBD
	// study sign of determinant of 
	//  2a  b
	//   b 2c

	double det = (2.0 * a() * 2.0 * c()) - (b() * b());
	double first = 2.0 * a();
	
	if (det > 0.0 && first < 0.0)
	    return PEAK;
	if (det > 0.0 && first > 0.0)
	    return PIT;
	if (det < 0.0)
	    return SADDLE;

	// if (det == 0.0 || first == 0.0)
	//     eprintx (-1,"Null determinant (%lf) or first term (%lf).\n", det, first);
	// eprintx (-1,"%s\n","Impossible combination.");

	if (det == 0.0 || first == 0.0)
	{
	    eprint ("Null determinant (%lf) or first term (%lf).\n", det, first);
	    return OTHER;
	}
	eprint ("%s\n","Impossible combination.");
	return OTHER;
    }
    
    ClassifiedType getClassifiedType (double w)
    {
	// Point z;
	// if (isZeroInWindow (w, &z))
	//     return studyHessian (z);
	if (isZeroInWindow (w))
	    return studyHessian ();
	else
	    return OTHER;
    }

    void print ()
    {
	fprintf (stdout, "a*x^2 + b*x*y + c*y^2 + d*x + e*y + f: ");
	for (int i = 0; i < 6; i++)
	    fprintf (stdout, "%lf%c", data[i], i == 5? '\n':' ');
    }
};


// ax2+ by2+cxy+dx+ey+f = z

// dfdx 2ax+cy+d=0
// dfdy 2by+cx+e=0

// 2a  c | x = | -d
//  c 2b | y   | -e

// H= 2a c \ c 2b

// d2fx2 2a
// d2fy2 2b
// d2fdxdy c

int width, height;
ASCHeader asch;
// double cellsize, xllcorner, yllcorner;
std::vector <SwissSpotHeight> swiss;
std::vector <SwissSpotHeight> swiss_class;

void check_and_print_generic (std::vector <SwissSpotHeight>& sc)
{
    int peak, pit, saddle, other;
    peak = pit = saddle = other = 0;
    
    for (int i = 0; i < sc.size(); i++)
    {
	if (sc[i].ct == PEAK)
	    peak++;

	if (sc[i].ct == PIT)
	    pit++;

	if (sc[i].ct == SADDLE)
	    saddle++;

	if (sc[i].ct == OTHER)
	    other++;
    }
    printf ("\n");
    printf ("REPORT total %d\n", sc.size() );
    printf ("REPORT peak %d\n", peak );
    printf ("REPORT saddle %d\n", saddle );
    printf ("REPORT pit %d\n", pit );
    printf ("REPORT other %d\n", other );
    printf ("\n");
}

void check_and_print_swiss (std::vector <SwissSpotHeight>& sc)
{
    int peak_total, peak_peak, peak_pit, peak_saddle, peak_other, peak_wrong;
    peak_total = peak_peak = peak_pit = peak_saddle = peak_other = peak_wrong = 0;
    int saddle_total, saddle_peak, saddle_pit, saddle_saddle, saddle_other, saddle_wrong;
    saddle_total = saddle_peak = saddle_pit = saddle_saddle = saddle_other = saddle_wrong = 0;
    int pit_total, pit_peak, pit_pit, pit_saddle, pit_other, pit_wrong;
    pit_total = pit_peak = pit_pit = pit_saddle = pit_other = pit_wrong = 0;
    int natural_total, natural_peak, natural_pit, natural_saddle, natural_other;
    natural_total = natural_peak = natural_pit = natural_saddle = natural_other = 0;
    int human_total, human_peak, human_pit, human_saddle, human_other;
    human_total = human_peak = human_pit = human_saddle = human_other = 0;
    int other_total, other_peak, other_pit, other_saddle, other_other, other_wrong;
    other_total = other_peak = other_pit = other_saddle = other_other = other_wrong = 0;
    
    for (int i = 0; i < sc.size(); i++)
    {
	if (is_peak (sc[i].st))
	{
	    peak_total++;
	    if (sc[i].ct == PEAK)
		peak_peak++;

	    if (sc[i].ct == PIT)
		peak_pit++;

	    if (sc[i].ct == SADDLE)
		peak_saddle++;

	    if (sc[i].ct == OTHER)
		peak_other++;
	}
	else if (is_saddle (sc[i].st))
	{
	    saddle_total++;
	    if (sc[i].ct == PEAK)
		saddle_peak++;

	    if (sc[i].ct == PIT)
		saddle_pit++;

	    if (sc[i].ct == SADDLE)
		saddle_saddle++;

	    if (sc[i].ct == OTHER)
		saddle_other++;
	}
	else if (is_pit (sc[i].st))
	{
	    pit_total++;
	    if (sc[i].ct == PEAK)
		pit_peak++;

	    if (sc[i].ct == PIT)
		pit_pit++;

	    if (sc[i].ct == SADDLE)
		pit_saddle++;

	    if (sc[i].ct == OTHER)
		pit_other++;
	}
	else if (is_natural (sc[i].st))
	{
	    natural_total++;
	    if (sc[i].ct == PEAK)
		natural_peak++;

	    if (sc[i].ct == PIT)
		natural_pit++;

	    if (sc[i].ct == SADDLE)
		natural_saddle++;

	    if (sc[i].ct == OTHER)
		natural_other++;
	}
	else if (is_human (sc[i].st))
	{
	    human_total++;
	    if (sc[i].ct == PEAK)
		human_peak++;

	    if (sc[i].ct == PIT)
		human_pit++;

	    if (sc[i].ct == SADDLE)
		human_saddle++;

	    if (sc[i].ct == OTHER)
		human_other++;
	}
	else if (is_pit (sc[i].st))
	{
	    pit_total++;
	    if (sc[i].ct == PEAK)
		pit_peak++;

	    if (sc[i].ct == PIT)
		pit_pit++;

	    if (sc[i].ct == SADDLE)
		pit_saddle++;

	    if (sc[i].ct == OTHER)
		pit_other++;
	}
	else
	{
	    other_total++;
	    if (sc[i].ct == PEAK)
		other_peak++;

	    if (sc[i].ct == PIT)
		other_pit++;

	    if (sc[i].ct == SADDLE)
		other_saddle++;

	    if (sc[i].ct == OTHER)
		other_other++;
	}
    }

    peak_wrong = peak_pit + peak_saddle;// + peak_other;
    saddle_wrong = saddle_pit + saddle_peak;// + saddle_other;
    pit_wrong = pit_peak + pit_saddle;// + pit_other;
    other_wrong = other_pit + other_saddle + other_peak;

    printf ("\n");
    printf ("REPORT peak_total %d\n", peak_total );
    printf ("REPORT peak_peak %d\n", peak_peak );
    printf ("REPORT peak_saddle %d\n", peak_saddle );
    printf ("REPORT peak_pit %d\n", peak_pit );
    printf ("REPORT peak_other %d\n", peak_other );
    printf ("REPORT peak_mismatch %d\n", peak_wrong );

    printf ("\n");
    printf ("REPORT saddle_total %d\n", saddle_total );
    printf ("REPORT saddle_peak %d\n", saddle_peak );
    printf ("REPORT saddle_saddle %d\n", saddle_saddle );
    printf ("REPORT saddle_pit %d\n", saddle_pit );
    printf ("REPORT saddle_other %d\n", saddle_other );
    printf ("REPORT saddle_mismatch %d\n", saddle_wrong );

    printf ("\n");
    printf ("REPORT pit_total %d\n", pit_total );
    printf ("REPORT pit_peak %d\n", pit_peak );
    printf ("REPORT pit_saddle %d\n", pit_saddle );
    printf ("REPORT pit_pit %d\n", pit_pit );
    printf ("REPORT pit_other %d\n", pit_other );
    printf ("REPORT pit_mismatch %d\n", pit_wrong );

    printf ("\n");
    printf ("REPORT other_total %d\n", other_total );
    printf ("REPORT other_peak %d\n", other_peak );
    printf ("REPORT other_saddle %d\n", other_saddle );
    printf ("REPORT other_pit %d\n", other_pit );
    printf ("REPORT other_other %d\n", other_other );
    printf ("REPORT other_mismatch %d\n", other_wrong );

    printf ("\n");
    printf ("REPORT natural_total %d\n", natural_total );
    printf ("REPORT natural_peak %d\n", natural_peak );
    printf ("REPORT natural_saddle %d\n", natural_saddle );
    printf ("REPORT natural_pit %d\n", natural_pit );
    printf ("REPORT natural_other %d\n", natural_other );

    printf ("\n");
    printf ("REPORT human_total %d\n", human_total );
    printf ("REPORT human_peak %d\n", human_peak );
    printf ("REPORT human_saddle %d\n", human_saddle );
    printf ("REPORT human_pit %d\n", human_pit );
    printf ("REPORT human_other %d\n", human_other );

    printf ("\n");
}


void adjacent_pixels (Point p, std::vector<Coord>& nb)
{
    double dbx;
    double mx = modf (p.x, &dbx);
    double dby;
    double my = modf (p.y, &dby);
    int bx = (int) dbx;
    int by = (int) dby;

    // X+ Y+
    if (mx >= 0.5 && my >= 0.5 && mx >= my)
    {
	nb.push_back (Coord (bx, by));
	nb.push_back (Coord (bx+1, by));
	nb.push_back (Coord (bx, by+1));
	nb.push_back (Coord (bx+1, by+1));
    }
    else if (mx >= 0.5 && my >= 0.5 && mx < my)
    {
	nb.push_back (Coord (bx, by));
	nb.push_back (Coord (bx, by+1));
	nb.push_back (Coord (bx+1, by));
	nb.push_back (Coord (bx+1, by+1));
    }
    // X+ Y-
    else if (mx >= 0.5 && my < 0.5 && ((mx-0.5) + my) >= 0.5)
    {
	nb.push_back (Coord (bx, by));
	nb.push_back (Coord (bx+1, by));
	nb.push_back (Coord (bx, by-1));
	nb.push_back (Coord (bx+1, by-1));
    }
    else if (mx >= 0.5 && my < 0.5 && ((mx-0.5) + my) < 0.5)
    {
	nb.push_back (Coord (bx, by));
	nb.push_back (Coord (bx, by-1));
	nb.push_back (Coord (bx+1, by));
	nb.push_back (Coord (bx+1, by-1));
    }
    // X- Y-
    else if (mx < 0.5 && my < 0.5 && mx >= my)
    {
	nb.push_back (Coord (bx, by));
	nb.push_back (Coord (bx, by-1));
	nb.push_back (Coord (bx-1, by));
	nb.push_back (Coord (bx-1, by-1));
    }
    else if (mx < 0.5 && my < 0.5 && mx < my)
    {
	nb.push_back (Coord (bx, by));
	nb.push_back (Coord (bx-1, by));
	nb.push_back (Coord (bx, by-1));
	nb.push_back (Coord (bx-1, by-1));
    }
    // X- Y+
    else if (mx < 0.5 && my >= 0.5 && (mx + (my-0.5)) >= 0.5)
    {
	nb.push_back (Coord (bx, by));
	nb.push_back (Coord (bx, by+1));
	nb.push_back (Coord (bx-1, by));
	nb.push_back (Coord (bx-1, by+1));
    }
    else if (mx < 0.5 && my >= 0.5 && (mx + (my-0.5)) < 0.5)
    {
	nb.push_back (Coord (bx, by));
	nb.push_back (Coord (bx-1, by));
	nb.push_back (Coord (bx, by+1));
	nb.push_back (Coord (bx-1, by+1));
    }    
}

void fill_window_coords (std::vector<Coord>& nb, int window)
{
    int w = window;
    for (int i = -w; i <= w; i++)
	for (int j = -w; j <= w; j++)
	{
	    // fprintf (stderr, "  NBW: %d,%d\n", i, j);
	    nb.push_back (Coord (i,j));
	}
}

void fill_window_coords (Coord s, std::vector<Coord>& nb, int window)
{
    int w = window;
    for (int i = s.x-w; i <= s.x+w; i++)
	for (int j = s.y-w; j <= s.y+w; j++)
	{
	    // fprintf (stderr, "  nbc: %d,%d\n", i, j);
	    nb.push_back (Coord (i,j));
	}
}

bool highest_in_window (Coord s, std::vector<Coord>& nb, ASCReader& ascr)
{
    double hv = -DBL_MAX;
    //Coord hc;
    double sv = ascr.get_pixel (s.x, s.y);
    for (unsigned j = 0; j < nb.size(); j++)
    {
	double cv = ascr.get_pixel (nb[j].x, nb[j].y);
	if (cv > hv)
	    hv = cv;
    }

    return hv == sv;
}

bool lowest_in_window (Coord s, std::vector<Coord>& nb, ASCReader& ascr)
{
    double lv = DBL_MAX;
    // Coord lc;
    double sv = ascr.get_pixel (s.x, s.y);
    for (unsigned j = 0; j < nb.size(); j++)
    {
	double cv = ascr.get_pixel (nb[j].x, nb[j].y);
	if (cv < lv)
	{
	    lv = cv;
	    // lc = nb[j];
	}
    }

    return lv == sv;
    // return lc == s;
}

void print_swiss (std::vector<SwissSpotHeight> points);

int main (int argc, char *argv[])
{
    app_init (argc, argv);

    ASCReader ascr (asc_file);
    asch = ASCHeader (ascr);
    width = asch.width;
    height = asch.height;
    // xllcorner = ascr.xllcorner;
    // yllcorner = ascr.yllcorner;
    // cellsize = ascr.cellsize;
    
    CSVReader csvio (asch);
    csvio.load (input_swiss_file, swiss);
    
    std::vector<Coord> nbw;
    fill_window_coords (nbw, window);

    printf ("basic classification step. Pre-correction: %s\n\n",
	    correction_pre? "on" : "off");

    int pre_peak, pre_peak_saddle, pre_pit, pre_pit_saddle;
    pre_peak = pre_peak_saddle = pre_pit = pre_pit_saddle = 0;
    
    // classification
    // per ogni punto svizzero che giace su un pixel distante almeno W dai bordi
    for (unsigned i = 0; i < swiss.size(); i++)
    {
	Coord c = point2coord (swiss[i].p);
	if (c.is_inside (width, height, window))
	{
	    // impacchetta tutto nei vector3D eigen aggiungendo le z
	    std::vector<Coord> nbc;
	    std::vector<Eigen::Vector3d> VV;
	    fill_window_coords (c, nbc, window);
	    VV.resize (nbc.size());
	    for (unsigned j = 0; j < nbc.size(); j++)
	    {
		VV[j][0] = nbw[j].x;
		VV[j][1] = nbw[j].y;
		VV[j][2] = ascr.get_pixel (nbc[j].x, nbc[j].y);
	    }
	    
	    // fai la quadrica, fatti dare il tipo, salvalo
	    Quadric quadpol = Quadric::fit (VV);
	    
	    // fprintf (stderr, "point %d ", i);
	    // quadpol.print ();
	    // fprintf (stderr, "\n", quadpol.a(), quadpol.b(), quadpol.c(), quadpol.d(),
	    // 	     quadpol.e(), quadpol.f());

	    swiss[i].ct = quadpol.getClassifiedType (window);
	    swiss[i].z = ascr.get_pixel (c.x, c.y);
	    db_add (swiss[i].dbt, DB_CLASSIFICATION);

	    // GG TODO print ho many points added by precondition
	    if (correction_pre &&
		(swiss[i].ct == OTHER || swiss[i].ct == SADDLE))
	    {
		if (swiss[i].ct == SADDLE && !correction_pre_saddles)
		    goto hell_yeah;
		
		// if (swiss[i].st == NAMED_MAIN_SUMMIT_200 &&
		//     swiss[i].ct != PEAK)
		// {
		//     printf ("PRE - swiss[%d]\n", i);
		//     swiss[i].print ();
		//     quadpol.print ();
		//     Point ppp;
		//     csvio.img2asc (swiss[i].p, &ppp);
		//     printf ("geo: %lf,%lf\n", ppp.x, ppp.y);
		//     printf ("\n\n");
		// } 
		

		
		// get one to four pixel depending on distance 0.5, ordered on their distance
		std::vector<Coord> ap;
		adjacent_pixels (swiss[i].p, ap);
		// find the first HIGHEST or LOWEST pixel; mark the point accordingly as PEAK or PIT
		for (unsigned j = 0; j < ap.size(); j++)
		    if (highest_in_window (ap[j], nbc, ascr))
		    {
			pre_peak++;
			if (swiss[i].ct == SADDLE)
			    pre_peak_saddle++;
			swiss[i].ct = PEAK;
			break;
		    }
		    else if (lowest_in_window (ap[j], nbc, ascr))
		    {
			pre_pit++;
			if (swiss[i].ct == SADDLE)
			    pre_pit_saddle++;
			swiss[i].ct = PIT;
			break;
		    }
		
		
	    }
	    
	hell_yeah:

	    // if (swiss[i].st == NAMED_MAIN_SUMMIT_200 &&
	    // 	swiss[i].ct != PEAK)
	    // {
	    // 	printf ("POST - swiss[%d]\n", i);
	    // 	swiss[i].print ();
	    // 	quadpol.print ();
	    // 	Point ppp;
	    // 	csvio.img2asc (swiss[i].p, &ppp);
	    // 	printf ("geo: %lf,%lf\n", ppp.x, ppp.y);
	    // 	printf ("\n\n");
	    // } 
	    
	    swiss_class.push_back (swiss[i]);
	}
    }

    if (correction_pre)
	printf ("Optional fit correction (saddles correction(%d)):\n"
		"     peaks/s (%d/%d), pits/s(%d/%d )\n\n", correction_pre_saddles,
		pre_peak, pre_peak_saddle, pre_pit, pre_pit_saddle);
    
    print_swiss (swiss_class);

    printf ("Refining step using tracking.\n");

    int deleted_not_found = 0;
    int deleted_below_threshold = 0;

    if (track_file != NULL)
    {
	Track *track;
	TrackOrdering *track_order;
    
	track_reader (track_file, &track, &track_order);

	TrackMap trackmap (*track);
    
	for (unsigned i = 0; i < swiss_class.size(); i++)
	{
	    ClassifiedType ct = swiss_class[i].ct;
	    if (ct == OTHER)
		continue;
	
	    // get all pixel coords in window
	    Coord c = point2coord (swiss_class[i].p);
	    std::vector<Coord> nbc;
	    fill_window_coords (c, nbc, window);

	    // for every pixel in window search tracking to see if it's there
	    // keep the candidate pixel with most strength
	    double s_max = -DBL_MAX;
	    for (unsigned j = 0; j < nbc.size(); j++)
		// for (unsigned k = 0; k < track->lines.size(); k++)
		{
		    int k = trackmap (nbc[j]);
		    if (k > -1 && is_sametype (track->get_type (k), ct))
		    {
		    // if (!track->is_original (k))
		    // 	continue;
		    
		    // if (nbc[j] != track->coord (k))
		    // 	continue;

		    // int kk = trackmap (nbc[j]);
		    // if (kk != k)
		    // 	printf ("SOOCA %d %d\n", k, kk);


		    // if (!is_sametype (track->get_type (k), ct))
		    // 	continue;
		
			double s = track->strength (k);
			if (s > s_max)
			    s_max = s;
		    }
		}
	
	    if (s_max == -DBL_MAX)
	    {
		deleted_not_found++;
		swiss_class[i].ct = OTHER;
	    }
	    else if (s_max < strength_threshold)
	    {	
		deleted_below_threshold++;
		swiss_class[i].ct = OTHER;
	    }

	    if (swiss_class[i].st == NAMED_MAIN_SUMMIT_200 &&
		swiss_class[i].ct == OTHER)
	    {
		swiss_class[i].print ();
		Point ppp;
		csvio.img2asc (swiss[i].p, &ppp);
		printf ("geo: %lf,%lf\n", ppp.x, ppp.y);
		printf ("\n");
	    }
	}    

	printf ("Declassified, not found: %d. Declassified, below threshold: %d.\n"
		"New classification...\n", deleted_not_found, deleted_below_threshold);
	print_swiss (swiss_class);
    }
    
    csvio.save (output_swiss_file, swiss_class);

    if (perform_final_report_swiss)
	check_and_print_swiss (swiss_class);
    if (perform_final_report_generic)
	check_and_print_generic (swiss_class);
}


void print_swiss (std::vector<SwissSpotHeight> points)
{
    int other_num, peak_num, pit_num, saddle_num;
    other_num = peak_num = pit_num = saddle_num = 0;
    
    for (unsigned i = 0; i < points.size(); i++)
	if (points[i].ct == OTHER)
	    other_num++;
	else if (points[i].ct == PEAK)
	    peak_num++;
	else if (points[i].ct == PIT)
	    pit_num++;
	else if (points[i].ct == SADDLE)
	    saddle_num++;
	else
	    fprintf (stderr, "WARNING: wrong type in selection, point %d, type %d",
		     i, points[i].ct);

    printf ("Swiss points total: %zu. Peaks: %d, Pits %d, Saddles %d. "
	    "Not classified (OTHER) %d\n\n",
	    points.size(), peak_num, pit_num, saddle_num, other_num);
}
