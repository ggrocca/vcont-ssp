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
	    eprint ("lines[%d] -> [%d,%d] _ tracktype:%c _ maptypr:%c\n",
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

/// initialize tracking lines
void init_critical_lines (std::vector<CriticalPoint>& critical,
			  std::vector<TrackLine>& lines);

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
	
	// print_stats(cps);
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
