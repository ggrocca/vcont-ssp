#include "demreader.hh"
#include "csvreader.hh"

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


void print_help (FILE* f)
{
    fprintf (f, "Usage: swissclassifier\n"
	     // "[-s scalespacefile.ssp]\n"
	     "-i inputname : csv input file\n"
	     "-a asc file : for georegistration\n"
	     "[-o outputname] : csv output file\n"
	     "[-W WINDOW] : pixel window for fit classification. default 2 (5x5)\n"
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

	    case 'o':
                output_swiss_file = (*++argv);
                argc--;
                break;

            case 'W':
                window = atoi (*++argv);
                argc--;
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
	return b();
    }
    
    double dxx(double x, double y)
    {
	return 2.0*a();
    }
    
    double dyy(double x, double y)
    {
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

    bool isZeroInWindow (double w, Point* zero)
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
	zero->x = xx(0);
	zero->y = xx(1);
	return
	    xx(0) > -w &&
	    xx(0) <  w &&
	    xx(1) > -w &&
	    xx(1) <  w;
    }

    SwissType studyHessian (Point zero)
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

	if (det == 0.0 || first == 0.0)
	    eprintx (-1,"Null determinant (%lf) or first term (%lf).\n", det, first);

	eprintx (-1,"%s\n","Impossible combination.");
    }
    
    SwissType getSwissType (double w)
    {
	Point z;
	if (isZeroInWindow (w, &z))
	    return studyHessian (z);
	else
	    return OTHER;
    }

    void print ()
    {
	for (int i = 0; i < 6; i++)
	    fprintf (stderr, "%lf%c", data[i], i == 5? '\n':' ');
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
double cellsize, xllcorner, yllcorner;
std::vector <SwissSpotHeight> swiss;
std::vector <SwissSpotHeight> swiss_class;

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

int main (int argc, char *argv[])
{
    app_init (argc, argv);

    ASCReader ascr (asc_file);
    width = ascr.width;
    height = ascr.height;
    xllcorner = ascr.xllcorner;
    yllcorner = ascr.yllcorner;
    cellsize = ascr.cellsize;
    
    CSVReader csvio (width, height, cellsize, xllcorner, yllcorner);
    csvio.load (input_swiss_file, swiss);
    
    std::vector<Coord> nbw;
    fill_window_coords (nbw, window);
    
    // classification
    // per ogni punto svizzero che giace su un pixel distante almeno W dai bordi
    for (int i = 0; i < swiss.size(); i++)
    {
	Coord c = point2coord (swiss[i].p);
	if (c.is_inside (width, height, window))
	{
	    // impacchetta tutto nei vector3D eigen aggiungendo le z
	    std::vector<Coord> nbc;
	    std::vector<Eigen::Vector3d> VV;
	    fill_window_coords (c, nbc, window);
	    VV.resize (nbc.size());
	    for (int j = 0; j < nbc.size(); j++)
	    {
		VV[j][0] = nbw[j].x;
		VV[j][1] = nbw[j].y;
		VV[j][2] = ascr.get_pixel (nbc[j].x, nbc[j].y);
	    }
	    
	    // fai la quadrica, fatti dare il tipo, salvalo
	    Quadric quadpol = Quadric::fit (VV);
	    
	    fprintf (stderr, "point %d ", i);
	    quadpol.print ();
	    // fprintf (stderr, "\n", quadpol.a(), quadpol.b(), quadpol.c(), quadpol.d(),
	    // 	     quadpol.e(), quadpol.f());
	    
	    swiss[i].t = quadpol.getSwissType (window);
	    swiss_class.push_back (swiss[i]);
	}
    }
    
    csvio.save (output_swiss_file, swiss_class, true);
}
