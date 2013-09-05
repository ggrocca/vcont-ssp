#include "flipper.hh"
#include "trackstats.hh"

#include <vector>
#include <algorithm>


Map::Map () : Grid<MapEntry> () {}

Map::Map(int w, int h) : Grid<MapEntry> (w, h) {}

Map::Map(Map& map) : Grid<MapEntry> (map.width, map.height)
{
    for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++)
	    (*this)(i, j) = map (i, j);
}

Map::Map(Dem& dem, std::vector<TrackLine>& lines) :
    Grid<MapEntry> (dem.width, dem.height)
{
    for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++)
	{
	    Coord c = Coord (i, j);
	    MapEntry me (dem.is_clip (c));

	    for (int k = 0; k < 6; k++)
	    {
		Coord r;
		c.round_trip_6 (&r);
		me.set_rel (k, dem.point_relation (c, r));
	    }

	    (*this)(c) = me;

	    if (point_type (c) != dem.point_type (c))
		eprint ("Point [%d,%d]. demtype: %c -- maptype: %c\n", i, j,
			critical2char (dem.point_type (c)),
			critical2char (point_type (c)));
	}

    for (unsigned i = 0; i < lines.size(); i++)
    {
	if (lines[i].entries.size() < 1)
	    eprint ("%s", "Empty line.\n");

	if (lines[i].is_dead())
	    continue;

	Coord c = lines[i].entries.back().c;
	CriticalType tracktype = lines[i].get_type();
	CriticalType maptype = point_type (c);	

	if (tracktype == MAX || tracktype == MIN || tracktype == SA2)
	    (*this)(c).tr[0] = i;
	else if (tracktype == SA3)
	{
	    if ((*this)(c).tr[0] == -1)
	    {
		(*this)(c).tr[0] = i;
		(*this)(c).tr[1] = lines[i].entries.back().mate;
	    }
	    else if ((*this)(c).tr[0] != lines[i].entries.back().mate ||
		     (*this)(c).tr[1] != (int)i)
		eprint ("%s", "Mismatched 2-saddle.\n");
	}
	else
	    eprint ("%s", "Wrong critical type in Map.\n");

	if (tracktype != maptype)
	    eprint ("lines[%d] -> [%d,%d] _ tracktype:%c _ maptype:%c\n",
		    i, c.x, c.y, critical2char (tracktype), critical2char (maptype));
    }

}

bool Map::is_clip (Coord c)
{
    return (*this)(c).is_clip;
}

bool Map::is_clip (int x, int y)
{
    return is_clip (Coord (x, y));
}

MapEntry& Map::operator() (Coord c) // = ABYSS
{
    return Grid<MapEntry>::operator()(c);
}

MapEntry& Map::operator() (int x, int y) // = ABYSS
{
    return Grid<MapEntry>::operator()(x, y);
}

// returns k-nth relation to neightbour, ordered first to sixth cw starting from -1,-1
RelationType Map::point_relation (Coord c, int idx)
{
    return (*this)(c).get_rel (idx);
}

RelationType Map::point_relation (int x, int y, int idx)
{
    return point_relation (Coord (x, y), idx);
}

CriticalType Map::point_type (Coord c)
{
    if (c.is_outside (width, height))
	return MIN;

    if (is_clip (c))
	return REG;

    RelationType current,previous,first;
    int changes = 0;

    first = previous = point_relation (c, 0);

    for (int i = 0; i < 6; i++)
    {
	current = point_relation (c, (i + 1) % 6);
	point_type_step (current, &previous, &changes);
    }

    return point_type_analyze (first, changes, c);
}

CriticalPair Map::pair(Edge e)
{
    CriticalPair p;
    p.a = point_type (e.l);
    p.b = point_type (e.r);
    return p;
}


RelationType Map::invert_relation (Edge e)
{
    Coord a = e.l;
    Coord b = e.r;

    int a_idx = a.linear_index (b);

    if (a_idx == -1)
    {
	eprint ("%s", "First call to linear_index() failed.\n");
	return EQ;
    }
    
    RelationType a_rel = inverted_relation (point_relation (a, a_idx));

    int b_idx = b.linear_index (a);

    if (b_idx == -1)
    {
	eprint ("%s", "Second call to linear_index() failed.\n");
	return EQ;
    }

    RelationType b_rel = inverted_relation (point_relation (b, b_idx));

    if (a_rel == b_rel)
	eprint ("Impossible inverted relation: [%d][%d]=%c <> [%d][%d]=%c\n",
		a.x, a.y, relation2char (a_rel), b.x, b.y, relation2char (b_rel));

    (*this)(a).set_rel (a_idx, a_rel);
    (*this)(b).set_rel (b_idx, b_rel);

    return a_rel;
}

void Map::update_relations (Dem* d, Coord c)
{
    for (int k = 0; k < 6; k++)
    {
	Coord r;
	c.round_trip_6 (&r);

	(*this)(c).set_rel (k, d->point_relation (c, r));
	if (r.is_inside (width, height))
	    (*this)(r).set_rel (r.linear_index (c), d->point_relation (r, c));
    }
}

void Map::point_print (Coord c)
{
    point_print (SCOPE_ALWAYS, c);
}

void Map::point_print (int scope, Coord c)
{
    MapEntry me = (*this)(c);
    
    tprintsp (scope, "@@", " map entry %d %d. tr[0]:%d _ tr[1]:%d @@\n",
	       c.x, c.y, me.tr[0], me.tr[1]);
    tprints (scope, "     %c %c\n"
	             "   %c %c %c\n"
	             "   %c %c\n",
	           relation2char (me.get_rel (2)), relation2char (me.get_rel (3)),
	      relation2char (me.get_rel (1)), '*', relation2char (me.get_rel (4)),
	      relation2char (me.get_rel (0)), relation2char (me.get_rel (5))
	    );
}


bool Map::check_relation (Dem& dem)
{
    bool r = true;

    for (int i = 0; i < width; i++)
    	for (int j = 0; j < height; j++)
	{
	    Coord a = Coord (i, j);

	    for (int k = 0; k < 6; k++)
	    {
		Coord b;

		a.round_trip_6 (&b);

		if (b.is_outside (width, height))
		    continue;

		if (a.linear_index (b) < 0 || a.linear_index (b) > 5 ||
		    b.linear_index (a) < 0 || b.linear_index (a) > 5)
		    eprint ("Linear index error: ab[%d] ba[%d].\n", 
			    a.linear_index (b), b.linear_index (a));

		if (a.linear_index (b) < 3 &&
		    (a.linear_index (b) + 3) != b.linear_index (a))
		    eprint ("Linear index mismatch 012: ab[%d] ba[%d].\n", 
			    a.linear_index (b), b.linear_index (a));

		if (a.linear_index (b) >= 3 &&
		    a.linear_index (b) != (b.linear_index (a) + 3))
		    eprint ("Linear index mismatch 345: ab[%d] ba[%d].\n", 
			    a.linear_index (b), b.linear_index (a));

		RelationType ra = point_relation (a, a.linear_index (b));
		RelationType rb = point_relation (b, b.linear_index (a));

		RelationType ra_d = dem.point_relation (a, b);
		RelationType rb_d = dem.point_relation (b, a);

		if ((ra == LT && rb == LT) || (ra == GT && rb == GT))
		{
		    eprint ("Relation mismatch in map: a[%d,%d] _ b[%d,%d] ___ "
			    "<ab :: i:%d: mr:%c dr:%c> <ba :: i:%d mr:%c dr:%c> ___ "
			    "a-b[% d,% d] _ b-a[% d,% d].\n",
			    a.x, a.y, b.x, b.y,
			    a.linear_index (b),
			    relation2char (ra), relation2char (ra_d),
			    b.linear_index (a),
			    relation2char (rb), relation2char (rb_d),
			    a.x - b.x, a.y - b.y,
			    b.x - a.x, b.y - a.y);

		    r = false;
		}

		if ((ra_d == LT && rb_d == LT) || (ra_d == GT && rb_d == GT))
		{
		    eprint ("Relation mismatch in dem: a[%d,%d] _ b[%d,%d] ___ "
			    "<ab :: dr:%c> <ba :: dr:%c>\n",
			    a.x, a.y, b.x, b.y,
			    relation2char (ra), relation2char (rb));

		    // dem.point_print (a);
		    // dem.point_print (b);

		    r = false;
		}

		if (ra != ra_d || rb != rb_d)
		{
		    eprint ("Relation mismatch map-dem: a[%d,%d] _ b[%d,%d] ___ "
			    "<ab :: mr:%c dr:%c> <ba :: mr:%c dr:%c>\n",
			    a.x, a.y, b.x, b.y,
			    relation2char (ra), relation2char (ra_d),
			    relation2char (rb), relation2char (rb_d));

		    r = false;
		}

	    }
	}

    return r;
}


bool Map::check_relation (Map& mc)
{
    bool r = true;

    if (width != mc.width || height != mc.height)
    {
	eprint ("%s", "Different sizes.\n");
	return false;
    }

    for (int i = 0; i < width; i++)
    	for (int j = 0; j < height; j++)
    	{
	    MapEntry mel = (*this) (i, j);
	    MapEntry mer = mc (i, j);

	    if (mel.e[0] != mer.e[0] || mel.e[1] != mer.e[1] || 
		mel.e[2] != mer.e[2] || mel.e[3] != mer.e[3] || 
		mel.e[4] != mer.e[4] || mel.e[5] != mer.e[5])
	    {
		eprint ("Difference at point [%d][%d].\n", i, j);
		r = false;
		// printf ("@@@ check_relation(): NEW MAP ");
		// print_point (i, j);
		// printf ("@@@ check_relation(): OLD MAP ");
		// mc.print_point (i, j);
	    }
	}

    return r;
}


bool Map::check_reference (Map& mc)
{
    bool r = true;

    if (width != mc.width || height != mc.height)
    {
	eprint ("%s", "Different sizes.\n");
	return false;
    }

    for (int i = 0; i < width; i++)
    	for (int j = 0; j < height; j++)
    	{
	    MapEntry mel = (*this) (i, j);
	    MapEntry mer = mc (i, j);

	    if (mel.tr[0] == mer.tr[0] && mel.tr[1] == mer.tr[1])
		continue; // normal case
	    else if (mel.tr[0] == mer.tr[1] && mel.tr[1] == mer.tr[0] &&
		     mel.tr[0] != -1 && mel.tr[1] != -1)
		continue; // inverted 2-saddle case
	    else
	    {
		eprint ("Difference at point [%d][%d].\n", i, j);
		r = false;
		// printf ("@@@ check_reference(): NEW MAP ");
		// print_point (i, j);
		// printf ("@@@ check_reference(): OLD MAP ");
		// mc.print_point (i, j);
	    }
	}

    return r;
}



///////////////////
///////////////////
///////////////////
///////////////////
///// FLIPPER /////
///////////////////
///////////////////
///////////////////
///////////////////

void create_flips (Dem* b, Dem* n,
		   std::vector<Flip>& fs);


Flipper::Flipper (ScaleSpace& ss) : flip (ss.levels - 1)
{
    // debug trackstring
    FlipperDebug::setSS (&ss);

    // std::vector< std::vector<flip_t> > flip (ss.levels - 1);

    flips_total = 0;
    for (int i = 0; i < ss.levels - 1; i++)
    {
	tprint ("Start computing flips, level %d\n", i);

	create_flips (ss.dem[i], ss.dem[i+1], flip[i]);
	flips_total += flip[i].size ();

	// GG WARNING SHOULD BE ERROR
	if (SCOPE_FLIP)
	    for (unsigned j = 0; j < flip[i].size(); j++)
	    {
		if (flip[i][j].t > 1.0 || flip[i][j].t < 0.0)
		    tprints (SCOPE_FLIP, "DANGER: flip [%d] [%d] out of time.\n", i, j);
		// print_flip (flip[i][j], ss.dems, i);
	    }

	tprint ("End computing flips, level %d, %20d flips. Total flips:%20d\n",
		i, (int) flip[i].size(), flips_total);
    }
}


void process_flips_all (std::vector< std::vector<Flip> >& flips,
			std::vector<TrackLine>& lines,
			ScaleSpace& ss,
			TrackString& ts);

Track* Flipper::track (ScaleSpace& ss)
{
    (void) ss;

    Track* track = new Track ();

    // still have to think about how to do it; remember they are class members.
    // critical_original_num = critical_points.size();
    // critical_born_num = 0;
    // dem_width = ss.dems[0]->width;
    // dem_height = ss.dems[0]->height;

    TrackString ts = TrackString (ss.levels-1);

    process_flips_all (flip, track->lines, ss, ts);

    // for (unsigned i = 0; i < fs.size(); i++)
    // 	fs[i].clear();

    return track;
}

bool check_legality_flips (std::vector<Flip>& flips, Map* map);

/// initialize tracking lines
void init_critical_lines (std::vector<CriticalPoint>& critical,
			  std::vector<TrackLine>& lines);

// check if there are invalid flips and correct them in next dem
int control_filter_slow (std::vector<Flip>& flips, Map* map, Dem* b, Dem* n);
int control_filter_norm (std::vector<Flip>& flips, Map* map, Dem* b, Dem* n, int iter);
int control_filter_fast (int* f_idx_out, std::vector<Flip>& flips,
			 Map* map, Dem* b, Dem* n, int iter);

void Flipper::filter (Dem* b, Dem* n, int filter_algo,
		      std::vector<CriticalPoint>& crits)
{
    switch (filter_algo)
    {
    case 0:
	filter_slow (b, n, crits);
	break;

    case 1:
	filter_norm (b, n, crits);
	break;

    case 2:
	filter_fast (b, n, crits);
	break;

    default:
	eprintx (56, "%s", "Unrecognized flipper algorithm\n");
    }
}

void Flipper::filter_slow (Dem* b, Dem* n, std::vector<CriticalPoint>& crits)
{
    std::vector<Flip> fs;
    std::vector<TrackLine> lines;

    init_critical_lines (crits, lines);
    
    Map* map = new Map (*b, lines);
    create_flips (b, n, fs);

    int i = 0;
    while (control_filter_slow (fs, map, b, n) >= 0)
    {
	oprints (SCOPE_FILTER, "IN. Iteration: %d.\n", i++);
	delete map;
	map = new Map (*b, lines);
	create_flips (b, n, fs);
    }

    delete map;
}

void Flipper::filter_norm (Dem* b, Dem* n, std::vector<CriticalPoint>& crits)
{
    std::vector<Flip> fs;
    std::vector<TrackLine> lines;

    init_critical_lines (crits, lines);
    
    Map* map = new Map (*b, lines);
    create_flips (b, n, fs);

    int i = 0;
    int f = -1;
    while ((f = control_filter_norm (fs, map, b, n, i)) >= 0)
    {
	oprints (SCOPE_FILTER, "IN. Iteration: %d.\n", i++);
    }

    delete map;
}

// void Flipper::filter_check (Dem* b, Dem* n, std::vector<CriticalPoint>& crits)
// {
//     Dem* n_norm = new Dem (*n);
//     Dem* n_slow = new Dem (*n);

//     std::vector<Flip> fs_norm, fs_slow;
//     std::vector<TrackLine> lines;

//     init_critical_lines (crits, lines);
    
//     Map* map_norm = new Map (*b, lines);
//     Map* map_slow = new Map (*b, lines);
//     create_flips (b, n, fs_norm);
//     create_flips (b, n, fs_slow);

//     int i = 0;
//     int f_idx_norm = -1;
//     int f_idx_slow = -1;

//     while (true)
//     {
// 	f_idx_norm = control_filter_norm (fs_norm, map_norm, b, n_norm, i);
// 	f_idx_slow = control_filter_slow (fs_slow, map_slow, b, n_slow);

// 	tprintsp (SCOPE_FILTER, "<scope|filter_check> >>", "IN. Iteration: %d.\n", i++);

// 	delete map_slow;
// 	map_slow = new Map (*b, lines);
// 	create_flips (b, n, fs_slow);

// 	// check n
// 	// check map
// 	// check fs
//     }

//     delete map_norm;
//     delete map_slow;
//     delete n_norm;
//     delete n_slow;
// }

void Flipper::filter_fast (Dem* b, Dem* n, std::vector<CriticalPoint>& crits)
{
    std::vector<Flip> fs;
    std::vector<TrackLine> lines;

    init_critical_lines (crits, lines);
    
    Map* map = new Map (*b, lines);
    create_flips (b, n, fs);

    int i = 0;
    int f = -1;
    int f_idx = 0;
    while ((f = control_filter_fast (&f_idx, fs, map, b, n, i)) >= 0)
    {
	oprints (SCOPE_FILTER, "IN. Iteration: %d.\n", i++);
    }

    delete map;
}


///////////////////////////////////////////
////////// FLIP CREATION
///////////////////////////////////////////

bool check_flip (Dem* b, Dem* n, Edge e)
{
    RelationType prev_rel = b->point_relation (e.l, e.r);
    RelationType succ_rel = n->point_relation (e.l, e.r);
    
    return ((prev_rel == GT && succ_rel == LT) ||
	    (prev_rel == LT && succ_rel == GT) );
}

double get_flip_time (Dem* b, Dem* n, Edge e)
{
    double Za, Za_i, Zb, Zb_i;

    Za =   (*b)(e.l);
    Za_i = (*n)(e.l);
    Zb =   (*b)(e.r);
    Zb_i = (*n)(e.r);

    if (!(Za > Zb) && !(Za < Zb))
	return 0.0;

    return (Zb - Za) / (Zb - Za + Za_i - Zb_i);
}
// Edge __debug_edge = {2674,1025,1,1};
// bool __debug_stop = false;
void create_flips (Dem* b, Dem* n,
		   std::vector<Flip>& fs)
{
    fs.clear ();

    for (int i = 0; i < b->width; i++)
    	for (int j = 0; j < b->height; j++)
	{
	    // prepare 3 edges from current point pointing N, NW, W.
	    Edge edge[3];
	    edge[0].l.x = edge[1].l.x = edge[2].l.x = i;
	    edge[0].l.y = edge[1].l.y = edge[2].l.y = j;
	    edge[0].r.x = i;
	    edge[0].r.y = j + 1;
	    edge[1].r.x = i + 1;
	    edge[1].r.y = j + 1;
	    edge[2].r.x = i + 1;
	    edge[2].r.y = j;

	    for (int k = 0; k < 3; k++)
	    {
		// if (edge[k] == __debug_edge)
		// {
		//     __debug_stop = true;
		//     printf ("-------> HERE I AM\n");
		// }

		if (check_flip (b, n, edge[k]))
		{
		    Flip flip;
		    flip.e = edge[k];
		    flip.t = get_flip_time (b, n, edge[k]);
		    fs.push_back (flip);
		}
	    }
	}

    std::sort (fs.begin(), fs.end());
    // for (unsigned i = 0; i < fs.size(); i++)
    // {
    // // 	fs[i].t /= (fs[fs.size()-1].t + 0.0001);
    // // 	fs[i].t += (double)base;
    // }
}



///////////////////////////////////////////
/////////////// FLIP PROCESSING
///////////////////////////////////////////

// process flips
void process_flips (std::vector<Flip>& flips, Map* map,
		    std::vector<TrackLine>& lines, int base, TrackString& ts);

/// process each level of flips
void process_flips_all (std::vector< std::vector<Flip> >& flips,
			std::vector<TrackLine>& lines,
			ScaleSpace& ss,
			TrackString& ts)
{
    init_critical_lines (ss.critical[0], lines);

    Map* map = NULL;
    Map* oldmap;

    for (unsigned i = 0; i < flips.size(); i++)
    {
	oldmap = (map)? map : NULL;

	map = new Map (*(ss.dem[i]), lines);

	tprint ("begin_ check map, level %d\n", i);

	// must check with dem also.

	if (!map->check_relation (*(ss.dem[i])))
	    eprint ("Check map. level %d.\n", i);
	if (oldmap)
	{
	    map->check_relation (*oldmap);
	    map->check_reference (*oldmap);
	    delete oldmap;
	}

	tprint ("end_ check map, level %d\n", i);

	// printf ("begin_ checking critical prev level, level %d\n", i);
	// check_next_critical_points (cps, map, ss.c_points[i]);
	// printf ("end_ checking critical prev level, level %d\n", i);

	// printf ("begin_ checking global cri phase one, level %d\n", i);
	// check_cri_integrity_global (map, cps);
	// printf ("end_ checking global cri phase one, level %d\n", i);

	// printf ("begin_ checking global map phase one, level %d\n", i);
	// check_map_integrity_global (map, cps);
	// printf ("end_ checking global map phase one, level %d\n", i);

	// printf ("=============================================\n");
	// printf ("begin_ tracking flips, level %d\n", i);
    	process_flips (flips[i], map, lines, i, ts);
	// printf ("end_ tracking flips, level %d\n", i);
	// printf ("=============================================\n");

	// printf ("begin_ checking global cri phase two, level %d\n", i);
	// check_cri_integrity_global (map, cps);
	// printf ("end_ checking global cri phase two, level %d\n", i);

	// printf ("begin_ checking global map phase two, level %d\n", i);
	// check_map_integrity_global (map, cps);
	// printf ("end_ checking global map phase two, level %d\n", i);

	// // cps_print_special (cps);

	// printf ("begin_ checking critical lines, level %d\n", i);
	// check_critical_lines (cps);
	// printf ("end_ checking critical lines, level %d\n", i);

	// printf ("begin_ checking critical next level, level %d\n", i);
	// check_next_critical_points (cps, map, ss.c_points[i+1]);
	// printf ("end_ checking critical next level, level %d\n", i);

	// // delete map;

	Track::time_of_life = (double) i + 1;
	
	print_stats (lines);
    }

    ts.print ();

    delete map;
}



/// initialize tracking lines

void add_critical_line (std::vector<CriticalPoint>& critical,
			std::vector<TrackLine>& lines,
			int i, int mate)
{
    // add to critical point lines
    TrackLine line;
    line.type = critical[i].t;
    TrackEntry te;
    te.c = critical[i].c;
    te.t = 0.0;
    te.mate = mate;
    te.life = -1;
    line.entries.push_back (te);
    lines.push_back(line);
}
void init_critical_lines (std::vector<CriticalPoint>& critical,
			  std::vector<TrackLine>& lines)
{
    for (unsigned i = 0; i < critical.size(); i++)
    {
	if (critical[i].t == SA3)
	{
	    int cs = lines.size();
	    add_critical_line (critical, lines, i, cs+1);
	    add_critical_line (critical, lines, i, cs);
	}
	else
	    add_critical_line (critical, lines, i, -1);
    }

}

/// add a single flip
void pair_insert (std::vector<TrackLine>& lines, Map* map, Flip f,
		  CriticalPair before, CriticalPair after, int base);

void process_flips (std::vector<Flip>& flips, Map* map,
		    std::vector<TrackLine>& lines, int base, TrackString& ts)
{
    // std::vector<int> cpis;

    for (unsigned i = 0; i < flips.size(); i++)
    {
	// __flip2crindex (map, fs[i], cpis);

	CriticalPair before, after;

	before = map->pair (flips[i].e);
	map->invert_relation (flips[i].e);
	after = map->pair (flips[i].e);

	ts.insert (base, before, after);

	if (before.sum() != after.sum())
	    FlipperDebug::print_flip_interpolated (flips[i], before, after, map, base);

	// if (i == __debug_flip)
	//     tracking_debug_print (fs[i], before, after, map, base);

	pair_insert (lines, map, flips[i], before, after, base);

	// if (!check_map_integrity_local (map, cps, fs[i]))
	//     printf ("process_flips: maplocal was flip %d\n", i);

	// if (!check_cri_integrity_local (map, cps, cpis))
	//     printf ("process_flips: crilocal before was flip %d\n", i);
	// __flip2crindex (map, fs[i], cpis);
	// if (!check_cri_integrity_local (map, cps, cpis))
	//     printf ("process_flips: crilocal after was flip %d\n", i);
    }
}


// ggstart

/// add a single flip


// 1m -> RR
// m1 -> RR
// 1M -> RR
// M1 -> RR
void death_1 (std::vector<TrackLine>& lines, Map* map,
	      TrackEntry te0, TrackEntry te1)
{
    // get entries
    int i0 = ((*map)(te0.c)).get_ref_first ();
    int i1 = ((*map)(te1.c)).get_ref_first ();
    // map_entry me0, me1;
    // me0 = map->get_entry (ce0.x, ce0.y);
    // int i0 = me0.tr[0];
    // me1 = map->get_entry (ce1.x, ce1.y);
    // int i1 = me1.tr[0];

    // delete refs in map
    ((*map)(te0.c)).reset_ref_first ();
    ((*map)(te1.c)).reset_ref_first ();
    // me1.tr[0] = me0.tr[0] = -1;
    // map->set_entry (ce0.x, ce0.y, me0);
    // map->set_entry (ce1.x, ce1.y, me1);

    // push death of both points
    te0.life = i1;
    lines[i0].entries.push_back (te0);
    te1.life = i0;
    lines[i1].entries.push_back (te1);	
}

// 2m -> 1R
// M2 -> R1
// m2 -> R1
// 2M -> 1R
void death_2 (std::vector<TrackLine>& lines, Map* map,
	      CriticalType ct0, TrackEntry ce0,
	      CriticalType ct1, TrackEntry ce1)
{
    TrackEntry sal_e, sar_e;
    TrackEntry mm_e;

    // who's saddle, who's minmax? 
    if (ct0 == SA3 && (ct1 == MAX || ct1 == MIN))
    {
	sal_e = sar_e = ce0;
	mm_e = ce1;
    }
    else if (ct1 == SA3 && (ct0 == MAX || ct0 == MIN))
    {
	sal_e = sar_e = ce1;
	mm_e = ce0;
    }
    else
    {
	eprint ("%s", "Wrong input.\n");
	return;
    }

    // get indices of dying saddle, surviving saddle, dying minmax
    int is = ((*map)(sal_e.c)).get_ref_second ();
    int is_mate = ((*map)(sal_e.c)).get_ref_first ();
    int im = ((*map)(mm_e.c)).get_ref_first ();

    // in map, we lose reference to the second saddle and minmax
    ((*map)(sal_e.c)).reset_ref_second ();
    ((*map)(mm_e.c)).reset_ref_first ();

    // push death of saddle
    sal_e.life = im;
    sal_e.mate = is_mate;
    lines[is].entries.push_back (sal_e);

    // other saddles loses mate
    sar_e.mate = -1;
    lines[is_mate].entries.push_back (sar_e);

    // push death of minmax
    mm_e.life = is;
    lines[im].entries.push_back (mm_e);	
}

// RR -> 1M
// RR -> M1
// RR -> m1
// RR -> 1m
void birth_1 (std::vector<TrackLine>& lines, Map* map,
	      CriticalType ct0, TrackEntry te0,
	      CriticalType ct1, TrackEntry te1)
{
    // get future indices
    int i_te0 = lines.size();
    int i_te1 = lines.size() + 1;

    // create new lines and push birth of both points
    TrackLine nuline0, nuline1;
    nuline0.type = ct0;
    te0.life = i_te1;
    te0.mate = -1;
    nuline0.entries.push_back (te0);
    nuline1.type = ct1;
    te1.life = i_te0;
    te1.mate = -1;
    nuline1.entries.push_back (te1);
    lines.push_back (nuline0);
    lines.push_back (nuline1);

    // get map entries and set the new points
    ((*map)(te0.c)).set_ref_first (lines.size() - 2);
    ((*map)(te1.c)).set_ref_first (lines.size() - 1);
}

// R1 -> M2
// 1R -> 2M
// 1R -> 2m
// R1 -> m2
void birth_2 (std::vector<TrackLine>& lines, Map* map,
	      CriticalType ct0, TrackEntry te0,
	      CriticalType ct1, TrackEntry te1)
{
    // force 0 to be the 2-saddle
    if (ct1 == SA3)
    {
	CriticalType cts = ct0;
	ct0 = ct1;
	ct1 = cts;
	TrackEntry tes = te0;
	te0 = te1;
	te1 = tes;
    }
    else if (ct0 != SA3)
    {
	eprint ("%s", "Wrong input.\n");
	return;
    }

    // get map entries
    // map_entry me0, me1;
    // me0 = map->get_entry (te0.x, te0.y);
    // me1 = map->get_entry (te1.x, te1.y);
    // get previous saddle index, and future indices (saddle + minmax)
    int i_ps = ((*map)(te0.c)).get_ref_first (); // me0.tr[0];
    int i_te0 = lines.size();
    int i_te1 = lines.size() + 1;

    // update previous saddle with new mate
    TrackEntry te_ps = te0;
    te_ps.mate = i_te0;
    te_ps.life = -1;
    lines[i_ps].entries.push_back (te_ps);

    // create new lines. new saddle has previous as a mate
    TrackLine nuline0, nuline1;
    nuline0.type = ct0;
    te0.life = i_te1;
    te0.mate = i_ps;
    nuline0.entries.push_back (te0);
    nuline1.type = ct1;
    te1.life = i_te0;
    te1.mate = -1;
    nuline1.entries.push_back (te1);
    lines.push_back (nuline0);
    lines.push_back (nuline1);

    // set new second saddle and new minmax in map    
    ((*map)(te0.c)).set_ref_second (i_te0);
    // me0.tr[1] = i_te0;
    // map->set_entry (te0.x, te0.y, me0);
    ((*map)(te1.c)).set_ref_first (i_te1);
    // me1.tr[0] = i_te1;
    // map->set_entry (te1.x, te1.y, me1);
}

// 1R -> R1
// R1 -> 1R
// Rm -> mR
// mR -> Rm
// MR -> RM
// RM -> MR
void move_1 (std::vector<TrackLine>& lines, Map* map,
	     TrackEntry from_e, TrackEntry to_e)
{
    // get "from" map entry, retrieve index and set it to -1
    int i = ((*map)(from_e.c)).get_ref_first (); // from_e -- me.tr[0];
    ((*map)(from_e.c)).reset_ref_first ();

    // get "to" map entry, set index
    ((*map)(to_e.c)).set_ref_first (i);

    // push new line entry
    lines[i].entries.push_back (to_e);
}

// 12 -> 21
// 21 -> 12
void move_3 (std::vector<TrackLine>& lines, Map* map,
	     TrackEntry from_ce, TrackEntry to_ce)
{
    TrackEntry moving_ce = to_ce;
    // map_entry from_me, to_me;

    // get "from" map entry, retrieve both indexes
    int old_mate_i = ((*map)(from_ce.c)).get_ref_first ();// from_me.tr[0];
    int moving_i = ((*map)(from_ce.c)).get_ref_second (); // from_me.tr[1];
    // from_me = map->get_entry (from_ce.x, from_ce.y);

    // get "to" map entry, retrieve index
    int new_mate_i = ((*map)(to_ce.c)).get_ref_first ();

    // map reference lose second reference in from, gains it in to
    ((*map)(from_ce.c)).reset_ref_second ();
    // from_me.tr[1] = -1;
    // map->set_entry (from_ce.x, from_ce.y, from_me);
    ((*map)(to_ce.c)).set_ref_second (moving_i);
    // to_me.tr[1] = moving_i;
    // map->set_entry (to_ce.x, to_ce.y, to_me);

    // from loses its mate
    from_ce.mate = -1;
    lines[old_mate_i].entries.push_back (from_ce);

    // to gains new mate
    to_ce.mate = moving_i;
    lines[new_mate_i].entries.push_back (to_ce);
    
    // moving gains the saddle it's reaching as a mate.
    moving_ce.mate = new_mate_i;
    lines[moving_i].entries.push_back (moving_ce);
}

// R2 -> 11
// 2R -> 11
void move_21 (std::vector<TrackLine>& lines, Map* map,
	     TrackEntry from_ce, TrackEntry to_ce)
{
    // retrieve both indexes
    int old_mate_i = ((*map)(from_ce.c)).get_ref_first (); //from_me.tr[0];
    int moving_i = ((*map)(from_ce.c)).get_ref_second (); //from_me.tr[1];

    // map reference lose second reference in from, gains it in to
    ((*map)(from_ce.c)).reset_ref_second (); //from_me.tr[1] = -1;
    ((*map)(to_ce.c)).set_ref_first (moving_i); // to_me.tr[0] = moving_i;

    // from loses its mate
    from_ce.mate = -1;
    lines[old_mate_i].entries.push_back (from_ce);

    // moving has no mate
    to_ce.mate = -1;
    lines[moving_i].entries.push_back (to_ce);
}

// 11 -> R2
// 11 -> 2R
void move_12 (std::vector<TrackLine>& lines, Map* map,
	     TrackEntry from_ce, TrackEntry to_ce)
{
    TrackEntry moving_ce = to_ce;

    // get "from" map entry, retrieve index
    int from_i = ((*map)(from_ce.c)).get_ref_first (); //from_me.tr[0];

    // get "to" map entry, retrieve index
    int to_i = ((*map)(to_ce.c)).get_ref_first (); //to_me.tr[0];

    // map from reference loses its ref
    ((*map)(from_ce.c)).reset_ref_first ();

    // to gains reference to second saddle
    ((*map)(to_ce.c)).set_ref_second (from_i);

    // to gains new mate
    to_ce.mate = from_i;
    lines[to_i].entries.push_back (to_ce);
    
    // moving gains the saddle it's reaching as a mate.
    moving_ce.mate = to_i;
    lines[from_i].entries.push_back (moving_ce);
}

void pair_insert (std::vector<TrackLine>& lines, Map* map, Flip f,
		  CriticalPair before, CriticalPair after, int base)
{
    CriticalType bl, br, al, ar;
    bl = before.a;
    br = before.b;
    al = after.a;
    ar = after.b;

    TrackEntry el (f.e.l, f.t + ((double) base));
    TrackEntry er (f.e.r, f.t + ((double) base));

    if (before.sum() != after.sum())
	goto exit_error;

    if (bl == al && br == ar)
    {
	if ((bl == REG || bl == SA2) && (br == REG || br == SA2))
	    goto exit_success;
	else
	    goto exit_error;		
    }

    if (bl == ar && br == al)
    {
	if (bl != SA3 && br == REG)
	{
	    // 1-move from left to right
	    move_1 (lines, map, el, er);
	    goto exit_success;
	}
	if (bl == REG && br != SA3)
	{
	    // 1-move from right to left
	    move_1 (lines, map, er, el);
	    goto exit_success;
	}
	if (bl == SA3 && br == SA2)
	{
	    // 3-move from left to right
	    move_3 (lines, map, el, er);
	    goto exit_success;
	}
	if (bl == SA2 && br == SA3)
	{
	    // 3-move from right to left
	    move_3 (lines, map, er, el);
	    goto exit_success;
	}
	goto exit_error;
    }


    if ((bl != al && br != ar) && (bl != ar && br != al))
    {
	if (bl == REG && br == REG)
	{
	    // 1-birth
	    birth_1 (lines, map, al, el, ar, er);
	    goto exit_success;
	}
	if (al == REG && ar == REG)
	{
	    // 1-death
	    death_1 (lines, map, el, er);
	    goto exit_success;
	}
	if ((bl == REG && br == SA2) || (bl == SA2 && br == REG))
	{
	    // 2-birth
	    birth_2 (lines, map, al, el, ar, er);
	    goto exit_success;
	}
	if ((al == REG && ar == SA2) || (al == SA2 && ar == REG))
	{
	    // 2-death
	    death_2 (lines, map, bl, el, br, er);
	    goto exit_success;
	}
	if (bl == SA3 && br == REG)
	{
	    // 21-move from left to right
	    move_21 (lines, map, el, er);
	    goto exit_success;
	}
	if (bl == REG && br == SA3)
	{
	    // 21-move from right to left
	    move_21 (lines, map, er, el);
	    goto exit_success;
	}
	if (al == SA3 && ar == REG)
	{
	    // 12-move from right to left
	    move_12 (lines, map, er, el);
	    goto exit_success;
	}
	if (al == REG && ar == SA3)
	{
	    // 12-move from left to right
	    move_12 (lines, map, el, er);
	    goto exit_success;
	}
    }

    eprint ("%s", "Same sum, uncatched move.\n");

 exit_error:
    eprint ("%s", "Exit error!\n");
    FlipperDebug::print_flip_interpolated (f, before, after, map, base);

 exit_success:
    return;
}




///////////////////////////////////////////
/////////////// CONTROLLED TOPOLOGY
///////////////////////////////////////////


bool critical_pair_legality (CriticalPair before, CriticalPair after)
{
    CriticalType bl, br, al, ar;
    bl = before.a;
    br = before.b;
    al = after.a;
    ar = after.b;
    
   if ((bl != al && br != ar) && (bl != ar && br != al))
   {
       //  birth_1
       if (bl == REG && br == REG)
	   return false;
       
       // birth_2
       if ((bl == REG && br == SA2) || (bl == SA2 && br == REG))
	   return false;
   }

   return true;
}

void control_dem (Flip f, Dem* b, Dem* n)
{
    // prendo la quota z_flip in cui f.e.l e f.e.r sono orizzontali al tempo f.t
    double z_flip = ScaleSpace::lerp (f.t, (*b)(f.e.l), (*n)(f.e.l));

    // if (z_flip != ScaleSpace::lerp (f.t, (*b)(f.e.r), (*n)(f.e.r)))
    // 	eprint ("Different right and left z value at flip time.\n%.80lf\n%.80lf\n",
    // 		z_flip, ScaleSpace::lerp (f.t, (*b)(f.e.r), (*n)(f.e.r)));

    // prendo fra i vicini dell'edge, su dem_n, la piu' alta fra le
    // minori, z_minus,  e la piu' bassa fra le maggiori, z_plus.
    double z_minus = -DBL_MAX;
    double z_plus = DBL_MAX;

    Coord nc;
    for (int k = 0; k < 2; k++)
	for (int i = 0; i < 6; i++)
	{
	    if (k)
		f.e.l.round_trip_6 (&nc);
	    else
		f.e.r.round_trip_6 (&nc);

	    if ((*n)(nc) < z_flip && (*n)(nc) > z_minus)
		z_minus = (*n)(nc);

	    if ((*n)(nc) > z_flip && (*n)(nc) < z_plus)
		z_plus = (*n)(nc);
	}

    // se lo trovo imposto alla quantita' dell'altro
    // double z_diff_up = z_plus == DBL_MAX?
    // 	(z_flip - z_minus) / 2.0 : (z_plus - z_flip) / 2.0;
    // double z_diff_down = z_minus == -DBL_MAX?
    // 	(z_plus - z_flip) / 2.0 : (z_flip - z_minus) / 2.0;


    // se non lo trovo lo lascio al tempo del flip
    double z_diff_up = z_plus == DBL_MAX?
    	z_flip : (z_plus - z_flip) / 2.0;
    double z_diff_down = z_minus == -DBL_MAX?
    	z_flip : (z_flip - z_minus) / 2.0;        

    double z_up = z_flip + z_diff_up;
    double z_down = z_flip - z_diff_down;

    // if (z_minus == -DBL_MAX || z_plus == DBL_MAX)
    // 	eprint ("Could not find z values. z_flip: %lf, z_plus: %lf, z_minus: %lf.\n",
    // 		z_flip, z_plus, z_minus);    

    // if (z_plus <= z_flip || z_minus >= z_flip)
    // 	eprint ("Wrong z values. z_flip: %lf, z_plus: %lf, z_minus: %lf.\n",
    // 		z_flip, z_plus, z_minus);

    // il vertice che deve salire sale a (z_plus + Z_flip) / 2
    // l'altro scende (z_minus + z_flip) / 2
    // double z_up = (z_plus + z_flip) / 2.0;
    // double z_down = (z_minus + z_flip) / 2.0;

    // capisco chi fra f.e.l e f.e.r deve salire e chi scendere.
    // controllare che le due quote siano una minore dell'altra.
    if ((*n)(f.e.l) > (*n)(f.e.r))
    {
	(*n)(f.e.l) = z_down;
	(*n)(f.e.r) = z_up;

	    if ((*n)(f.e.l) >= (*n)(f.e.r))
		eprint ("%s", "Wrong l>r correction.\n");
    }
    else if ((*n)(f.e.l) < (*n)(f.e.r))
    {
	(*n)(f.e.l) = z_up;
	(*n)(f.e.r) = z_down;

	if ((*n)(f.e.l) <= (*n)(f.e.r))
	    eprint ("%s", "Wrong l<r correction.\n");
    }
    else
	eprint ("%s", "Impossible correction.\n");

    if ( ! ( ((*n)(f.e.l) > (*n)(f.e.r)) || ((*n)(f.e.l) < (*n)(f.e.r)) ) )
	eprintx (34, "%s", "Equal values after correction.\n");

}

bool check_map_dem_relations (Flip f, Map* m, Dem* d)
{
    bool r = true;
    Coord nc;

    for (int i = 0; i < 6; i++)
    {
	f.e.l.round_trip_6 (&nc);

	if (m->point_relation (f.e.l, i) != d->point_relation (f.e.l, nc))
	    r = false;
    }
    for (int i = 0; i < 6; i++)
    {
	f.e.r.round_trip_6 (&nc);

	if (m->point_relation (f.e.r, i) != d->point_relation (f.e.r, nc))
	    r = false;
    }

    return r;
}

int control_filter_slow (std::vector<Flip>& flips, Map* map, Dem* b, Dem* n)
{
    for (unsigned i = 0; i < flips.size(); i++)
    {
	CriticalPair before, after;

	before = map->pair (flips[i].e);
	map->invert_relation (flips[i].e);
	after = map->pair (flips[i].e);

	if (!critical_pair_legality (before, after))
	{
	    // correct in Dem* n
	    control_dem (flips[i], b, n);

	    // GG here control critical type of flipped points is back to normal
	    // CriticalPair control;
	    // control.a = n->point_type (flips[i].e.l);
	    // control.b = n->point_type (flips[i].e.r);
	    // if (before != control)
	    // 	tprintsp (SCOPE_FILTER, "<scope|filter> >>", "flip: %d. "
	    // 		"before: %c%c. after: %c%c. control: %c%c.\n", i,
	    // 		critical2char (before.a), critical2char (before.b), 
	    // 		critical2char (after.a), critical2char (after.b), 
	    // 		critical2char (control.a), critical2char (control.b));

	    // // GG here control that relations are back to before
	    // // (confront with map after inverted again)
	    // map->invert_relation (flips[i].e);
	    // if (!check_map_dem_relations (flips[i], map, n))
	    // 	eprint ("Not the same relations as before, flip: %d.\n", i);

	    tprintsp (SCOPE_FILTER, "<scope|filter_slow> >>", "OUT. Reached: %d/%zu.\n",
		      i, flips.size());
	    return i;
	}


	// if (before.sum() != after.sum())
	// FlipperDebug::print_flip_interpolated (flips[i], before, after, map, base);

	// if (i == __debug_flip)
	//     tracking_debug_print (fs[i], before, after, map, base);

	///////////// pair_insert (lines, map, flips[i], before, after, base);

	// if (!check_map_integrity_local (map, cps, fs[i]))
	//     printf ("process_flips: maplocal was flip %d\n", i);

	// if (!check_cri_integrity_local (map, cps, cpis))
	//     printf ("process_flips: crilocal before was flip %d\n", i);
	// __flip2crindex (map, fs[i], cpis);
	// if (!check_cri_integrity_local (map, cps, cpis))
	//     printf ("process_flips: crilocal after was flip %d\n", i);
    }

    return -1;
}

void get_local_flips (Edge e, Dem* b, Dem *n, std::vector<Flip>& fs)
{
    fs.clear();

    for (int k = 0; k < 2; k++)
	for (int i = 0; i < 6; i++)
	{
	    Coord c, oc, nc;
	    c = k? e.l : e.r;
	    oc = k? e.r : e.l;

	    c.round_trip_6 (&nc);

	    if (nc == oc)
		continue;

	    Flip f;
	    
	    if (c >= nc)
	    {
		f.e.l = nc;
		f.e.r = c;
	    }
	    else if (c <= nc)
	    {
		f.e.l = c;
		f.e.r = nc;
	    }
	    else
		eprint ("Impossible coords c(%d,%d) nc(%d,%d)", c.x, c.y, nc.x, nc.y);
	    
	    if (check_flip (b, n, f.e))
		f.t = get_flip_time (b, n, f.e);
	    else
		f.t = -1.0;

	    fs.push_back (f);
	}
}

int binary_search_flip (std::vector<Flip>& fs, Flip f)
{
    int start = 0;
    int end = fs.size ();

    while (end > start)
    {
	int i = ((end - start) / 2) + start;

	if (fs[i] == f)
	    return i;
	else if (f < fs[i]) // first half
	    end = i;
	else // second half
	    start = i+1;
    }

    return -1;
}

// GG: non trova i flip perche' nn ha inserito un flip che avrebbe dovuto inserire

Flip Flip::highest ()
{
    Flip f;
    f.t = DBL_MAX;
    f.e.l = f.e.r = Coord (INT_MAX, INT_MAX);
    return f;
}
Flip Flip::lowest ()
{
    Flip f;
    f.t = -DBL_MAX;
    f.e.l = f.e.r = Coord (INT_MIN, INT_MIN);
    return f;

}

Flip match_flips (Flip f_curr, std::vector<Flip>& old_fs, std::vector<Flip>& new_fs,
		  std::vector<Flip>& all_fs, int iteration, int flip_count)
{
    old_fs.push_back (f_curr);
    f_curr.t = -1.0;
    new_fs.push_back (f_curr);

    if (old_fs.size() != 11 || old_fs.size() != new_fs.size())
	eprint ("Wrong local flips size. old: %zu, new: %zu\n", 
		old_fs.size(), new_fs.size());

    int delete_flips_count = 0;

    int mark[11];
    Flip lower_new_f = (flip_count+1 >= (int) all_fs.size())?
	Flip::highest() : all_fs[flip_count+1];

    for (int i = 0; i < 11; i++)
    {
	mark[i] = -4;

	assert (old_fs[i].e == new_fs[i].e);

	if (new_fs[i].t >= 0.0 && new_fs[i] < lower_new_f)
	    lower_new_f = new_fs[i];

	// if old and not new
	if (old_fs[i].t >= 0.0 && new_fs[i].t < 0.0)
	{
	    // search using old_t and delete (new_t > 1.0)
	    int idx = binary_search_flip (all_fs, old_fs[i]);

	    if (idx < 0)
		eprintx (35, "Flip not found. flip %d, flip_count %d, iteration %d."
			 " old and not new\n", i, flip_count, iteration);

	    mark[i] = idx;
	    new_fs[i].t = 2.0;
	    delete_flips_count++;
	}
	// if old and new, and old_t != new_t
	else if (old_fs[i].t >= 0.0 && new_fs[i].t >= 0.0 && old_fs[i].t != new_fs[i].t)
	{	    
	    // search using old_t, change to new_t
	    int idx = binary_search_flip (all_fs, old_fs[i]);

	    if (idx < 0)
		eprintx (35, "Flip not found. flip %d, flip_count %d, iteration %d."
			 " old and new\n", i, flip_count, iteration);

	    mark[i] = idx;
	    //all_fs[idx].t = new_fs[i].t;
	}
	// if not old and new
	else if (old_fs[i].t < 0.0 && new_fs[i].t >= 0.0)
	    mark[i] = -1; // pushback with new_t
	// nothing to do
	else if ((old_fs[i].t >= 0.0 && new_fs[i].t >= 0.0 &&
		  old_fs[i].t == new_fs[i].t)
		 ||
		 (old_fs[i].t < 0.0 && new_fs[i].t < 0.0 ) )
		 mark[i] = -2;
	// error
	else
	    mark[i] = -3;
    }

    for (int i = 0; i < 11; i++)
    {
	if (mark[i] >= 0)
	    all_fs[ mark[i] ].t = new_fs[i].t;
	else if (mark[i] == -1)
	    all_fs.push_back (new_fs[i]);
	else if (mark[i] != -2)
	    eprintx (91, "Bad mark value. mark[%d] = %d", i, mark[i]);
    }

    // sort all flips
    std::sort (all_fs.begin(), all_fs.end()); 
    
    // resize all to delete flips with t > 1.0
    all_fs.resize (all_fs.size() - delete_flips_count);

    return lower_new_f;
}

int control_filter_norm (std::vector<Flip>& flips, Map* map, Dem* b, Dem* n, int iter)
{
    for (unsigned i = 0; i < flips.size(); i++)
    {
	CriticalPair before, after;

	before = map->pair (flips[i].e);
	map->invert_relation (flips[i].e);
	after = map->pair (flips[i].e);

	if (!critical_pair_legality (before, after))
	{
	    std::vector<Flip> old_flips;
	    std::vector<Flip> new_flips;
	    
	    // GG get old edge neighbours flips
	    get_local_flips (flips[i].e, b, n, old_flips);

	    // correct in Dem* n
	    control_dem (flips[i], b, n);

	    //////////// correct map
	    for (int k = i; k >= 0; k--)
		map->invert_relation (flips[k].e);
	    // map->invert_relation (flips[i].e);
	    // GG local update of relations was bullshit
	    // map->update_relations (n, flips[i].e.l);
	    // map->update_relations (n, flips[i].e.r);

	    ////////// correct flips

	    // GG get new neighbours flip
	    get_local_flips (flips[i].e, b, n, new_flips);

	    // GG match old and new, delete current
	    match_flips (flips[i], old_flips, new_flips, flips, iter, i);

	    oprints (SCOPE_FILTER, "OUT. Reached: %d/%zu.\n", i, flips.size());
	    return i;
	}
    }

    return -1;
}

int control_filter_fast (int* f_idx_out, std::vector<Flip>& flips,
			  Map* map, Dem* b, Dem* n, int iter)
{
    int f_idx = *f_idx_out;

    for (unsigned i = f_idx; i < flips.size(); i++)
    {
	CriticalPair before, after;

	before = map->pair (flips[i].e);
	map->invert_relation (flips[i].e);
	after = map->pair (flips[i].e);

	if (!critical_pair_legality (before, after))
	{
	    std::vector<Flip> old_flips;
	    std::vector<Flip> new_flips;
	    
	    // GG get old edge neighbours flips
	    get_local_flips (flips[i].e, b, n, old_flips);

	    // correct in Dem* n
	    control_dem (flips[i], b, n);

	    //////////// correct map
	    for (int k = i; k >= f_idx; k--)
		map->invert_relation (flips[k].e);
	    // map->invert_relation (flips[i].e);
	    // GG local update of relations was bullshit
	    // map->update_relations (n, flips[i].e.l);
	    // map->update_relations (n, flips[i].e.r);

	    ////////// correct flips

	    // GG get new neighbours flip
	    get_local_flips (flips[i].e, b, n, new_flips);

	    // GG match old and new, delete current
	    Flip lf = match_flips (flips[i], old_flips, new_flips, flips, iter, i);

	    if (lf == Flip::highest())
	    {
		*f_idx_out = flips.size();
		i = -1;
	    }
	    else if ((*f_idx_out = binary_search_flip (flips, lf)) < 0)
		eprintx (35, "Flip not found. i %d, iteration %d. f_idx_out: %d, "
			 "flip.t = %lf, f.e.l (%d, %d), f.e.r (%d,%d)\n",
			 i, iter, *f_idx_out,
			 lf.t, lf.e.l.x, lf.e.l.y, lf.e.r.x, lf.e.r.y);

	    oprints (SCOPE_FILTER, "OUT. Reached: %d/%zu.\n", i, flips.size());
	    return i;
	}
    }

    return -1;
}
