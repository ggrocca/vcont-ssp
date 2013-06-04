#include <limits.h>
#include <float.h>

#include "scalespace.hh"
#include "gaussian.hh"

#include "htimer.hh"


void scan_critical_points (Dem* d, std::vector<CriticalPoint>& cps)
{
    for (int i = 0; i < d->width; i++)
    	for (int j = 0; j < d->height; j++)
	{
	    Coord c = Coord (i, j);
	    CriticalType ct;

	    if ((ct = d->point_type (c)) != REG)
	    {
		CriticalPoint cp;
		cp.c = c;
		cp.t = ct;
		cps.push_back (cp);
	    }
	}
}


// CriticalType _critical_max = {{INT_MAX, INT_MAX}, MIN};
// CriticalType _critical_min = {{INT_MIN, INT_MIN}, MAX};
Coord _cmax (INT_MAX, INT_MAX);
Coord _cmin (INT_MIN, INT_MIN);
CriticalPoint _critical_max (_cmax, MIN);
CriticalPoint _critical_min (_cmin, MAX);

RelationType _get_ext_relation (Coord a, Coord b, Dem* d)
{
    if (a.x == INT_MAX && a.y == INT_MAX)
	return GT;
    if (b.x == INT_MAX && b.y == INT_MAX)
	return LT;

    if (a.x == INT_MIN && a.y == INT_MIN)
	return LT;
    if (b.x == INT_MIN && b.y == INT_MIN)
	return GT;

    if (a.x == b.x && a.y == b.y)
	return EQ;

    return d->point_relation (a, b);
}

RelationType _get_ext_relation (CriticalPoint a, CriticalPoint b, Dem* d)
{
    return _get_ext_relation (a.c, b.c, d);
}

CriticalPoint _discover_iline (CriticalPoint prev, Dem* d, Grid<char>& il)
{
    CriticalType search = prev.t;

    if ((d->point_type (prev.c) == search) || d->is_clip (prev.c)) // max or min reached
	return prev;

    il (prev.c) = search;

    // search for the max/min in neighbours
    Coord p;
    //double next_v, curr_v;
    CriticalPoint next, curr;

    curr.t = search;
    if (search == MAX)
	curr = _critical_min;
    if (search == MIN)
	curr = _critical_max;

    for (int i = 0; i < 8; i++)
    {
	prev.c.round_trip_6 (&p);
	next.c = p;
	if ((search == MAX && _get_ext_relation (next, curr, d) == GT) ||
	    (search == MIN && _get_ext_relation (next, curr, d) == LT))
	    curr.c = p;
    }

    return _discover_iline (curr, d, il);
}

void _draw_integral_lines (Coord c, Dem* d, Grid<char>& il)
{
    // gira intorno a x,y finche' non c'e' il primo cambio di segno

    Coord p;
    RelationType prev_sign, sign;

    c.round_trip_6 (&p);
    prev_sign = d->point_relation (p, c);

    while (true)
    {
	c.round_trip_6 (&p);
	sign = d->point_relation (p, c);
	if (prev_sign != sign)
	    break;
    }

    // trova i massimi di ogni zona piu' e i minimi di ogni zona meno

    CriticalPoint next;
    CriticalPoint curr_max = _critical_min;
    CriticalPoint curr_min = _critical_max;
    std::vector<CriticalPoint> maximums;
    std::vector<CriticalPoint> minimums;

    bool back_to_start = false;
    Coord start_p;
    start_p = p;
    prev_sign = sign;
    while (true)
    {
	next.c = p;
	sign = d->point_relation (p, c);
	if (sign != prev_sign)
	{
	    if (sign == LT) // prev_sign was GT, add curr_max_p to maximums
		maximums.push_back (curr_max);
	    if (sign == GT) // prev_sign was LT, add curr_min_p to minimums
		minimums.push_back (curr_min);

	    curr_max = _critical_min;
	    curr_min = _critical_max;
	}	    

	if (back_to_start)
	    break;

	if (sign == GT && _get_ext_relation (next, curr_max, d) == GT)
	{
	    curr_max.c = p;
	    curr_max.t = MAX;
	}

	if (sign == LT && _get_ext_relation (next, curr_min, d) == LT)
	{
	    curr_min.c = p;
	    curr_min.t = MIN;
	}

	prev_sign = sign;

	c.round_trip_6 (&p);

	if (start_p == p)
	    back_to_start = true;
    }

    // per ogni max min lancia _discover_iline

    for (unsigned i = 0; i < maximums.size(); i++)
	_discover_iline (maximums[i], d, il);

    for (unsigned i = 0; i < minimums.size(); i++)
	_discover_iline (minimums[i], d, il);

    // GG TODO ritorna i max/min trovati in un vector
}


void scan_integral_lines (Dem* d, std::vector<CriticalPoint>& cp, Grid<char>& il)
{
    for (unsigned i = 0; i < cp.size(); i++)
	if (cp[i].t == SA2 || cp[i].t == SA3)
	    _draw_integral_lines (cp[i].c, d, il);
}
 

ScaleSpace::ScaleSpace (DEMReader* base, int levels, ScaleSpaceOpts opts)
{
    this->levels = levels;
    dem = std::vector<Dem*>(levels);

    TGaussianBlur<double> BlurFilter;
    dem[0] = new Dem (base);

    if (opts.check (ScaleSpaceOpts::PERTURB))
    {
	tprintp ("!!!!!!!!", "%s", " PERTURB \n");

	Dem* td = new Dem (*(dem[0]), opts.perturb_amp, opts.perturb_seed);
	delete (dem[0]);
	dem[0] = td;
    }

    if (opts.check (ScaleSpaceOpts::PRESMOOTH))
    {
	tprintp ("!!!!!!!!", "%s", " PRESMOOTH \n");

	Dem* td = new Dem (*(dem[0]), BlurFilter);
	delete (dem[0]);
	dem[0] = td;
    }

    if (opts.check (ScaleSpaceOpts::CROP))
    {
	tprintp ("!!!!!!!!", "%s", " CROP \n");

	Dem* td = new Dem (*(dem[0]), opts.crop_a, opts.crop_b);
	delete (dem[0]);
	dem[0] = td;
    }

    int base_width = dem[0]->width;
    int base_height = dem[0]->height;

    for (int i = 1; i < levels; i++)
    {
	// dem[i] = new Dem (base_width, base_height);

	int window;
	window = level2window (i-1);

	if (window <= 0)
	    eprint ("Wrong window %d, level %d\n", window, i);
	
	ht h;
	ht_start (&h);

	dem[i] = new Dem (*(dem[i-1]), BlurFilter, window);	
	// BlurFilter.Filter (&(dem[i-1]->data[0]), &(dem[i]->data[0]),
	// 		   base_width, base_height, window);

	tprintp ("###$$$", "Blurred level %d, window %d, elapsed %s\n",
		 i, window, H2S_f (ht_get(&h), 's'));
    }

    if (opts.check (ScaleSpaceOpts::CLIP))
    {
	tprintp ("!!!!!!!!", "%s", " CLIP \n");

	dem[0]->clip_background (opts.clip_value);

	for (int i = 1; i < levels; i++)
	    dem[i]->clip_background ((*dem[0]), opts.clip_value);
    }

    if (opts.check (ScaleSpaceOpts::JUMP))
    {
	tprintp ("!!!!!!!!", "%s", " JUMP \n");

	if (opts.jump_num != 1)
	    eprint ("%s", "Only 1 jump supported now.\n");

	delete dem[0];
	for (int i = 1; i < levels; i++)
	    dem[i-1] = dem[i];
	dem.resize (dem.size() - 1);
	levels--;
    }

    critical = std::vector< std::vector<CriticalPoint> > (levels);

    // GG HACK
    // return;

    for (int i = 0; i < levels; i++)
    {
	critical[i] = std::vector<CriticalPoint>();
	scan_critical_points (dem[i], critical[i]);
	tprintp ("###$$$", "Critical points, scanned level %d\n", i);
    }

    if (opts.check (ScaleSpaceOpts::ILINES))
    {
	tprintp ("!!!!!!!!", "%s", " ILINES \n");

	ilines = std::vector< Grid<char> > (levels);

	for (int i = 0; i < levels; i++)
	{
	    ilines[i] = Grid<char> (base_width, base_height, false);

	    scan_integral_lines (dem[i], critical[i], ilines[i]);
	    tprintp ("###$$$", "Integral lines, scanned level %d\n", i);
	}
    }
}

double ScaleSpace::operator() (int level, int x, int y)
{
    Dem& d = *(dem[level]);
    return d (x,y);
}

double ScaleSpace::operator() (int level, Coord c)
{
    Dem& d = *(dem[level]);
    return d (c);
}

void ScaleSpace::point_print_interpolated (int level, double t, Coord c)
{
    point_print_interpolated (SCOPE_ALWAYS, level, t, c);
}

double __lerp (double t, double a, double b)
{
    return (a * (1 - t)) + (b * t);
}

static double _pdm (double d)
{
    return (d == -DBL_MAX) ? -4242.0 : d;
}

void ScaleSpace::point_print_interpolated (int scope, int level, double t, Coord cd)
{
    int i = level;
    int x = cd.x;
    int y = cd.y;

    double nw,n,ne,        w,c,e,        sw,s,se;
    double b_nw,b_n,b_ne,  b_w,b_c,b_e,  b_sw,b_s,b_se;
    double m_nw,m_n,m_ne,  m_w,m_c,m_e,  m_sw,m_s,m_se;

    b_nw = _pdm ((*this)(i, x-1, y+1));
    b_n  = _pdm ((*this)(i, x  , y+1));
    b_ne = _pdm ((*this)(i, x+1, y+1));
    b_w  = _pdm ((*this)(i, x-1, y  ));
    b_c  = _pdm ((*this)(i, x  , y  ));
    b_e  = _pdm ((*this)(i, x+1, y  ));
    b_sw = _pdm ((*this)(i, x-1, y-1));
    b_s  = _pdm ((*this)(i, x  , y-1));
    b_se = _pdm ((*this)(i, x+1, y-1));

    m_nw = _pdm ((*this)(i+1, x-1, y+1));
    m_n  = _pdm ((*this)(i+1, x  , y+1));
    m_ne = _pdm ((*this)(i+1, x+1, y+1));
    m_w  = _pdm ((*this)(i+1, x-1, y  ));
    m_c  = _pdm ((*this)(i+1, x  , y  ));
    m_e  = _pdm ((*this)(i+1, x+1, y  ));
    m_sw = _pdm ((*this)(i+1, x-1, y-1));
    m_s  = _pdm ((*this)(i+1, x  , y-1));
    m_se = _pdm ((*this)(i+1, x+1, y-1));

    nw = __lerp (t, b_nw, m_nw);
    n  = __lerp (t, b_n, m_n);
    ne = __lerp (t, b_ne, m_ne);
    w  = __lerp (t, b_w, m_w);
    c  = __lerp (t, b_c, m_c);
    e  = __lerp (t, b_e, m_e);
    sw = __lerp (t, b_sw, m_sw);
    s  = __lerp (t, b_s, m_s);
    se = __lerp (t, b_se, m_se);

    oprints (scope, "--POINT %d %d  -----\n", x, y);
    oprints (scope,
	     "  % 6.8lf % 6.8lf % 6.8lf\n"
	     "  % 6.8lf % 6.8lf % 6.8lf\n"
	     "  % 6.8lf % 6.8lf % 6.8lf\n",
	             nw,      n,     ne,
	              w,      c,      e,
	             sw,      s,     se);
    oprints (scope, "%s", "--------\n");    
}


ScaleSpace::~ScaleSpace()
{
    for (int i = 0; i < levels; i++)
	delete dem[i];
}
