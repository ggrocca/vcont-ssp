#include "flipper.hh"
#include "trackstats.hh"

#include <vector>
#include <algorithm>

MapEntry::MapEntry () : is_clip (false) { tr[0] = tr[1] = -1; }
MapEntry::MapEntry (bool is_clip) : is_clip (is_clip) { tr[0] = tr[1] = -1; }

void MapEntry::set_rel (int i, RelationType r)
{
    e[i] = (char) r;
}

RelationType MapEntry::get_rel (int i)
{
    return (RelationType) e[i];
}

Map::Map () : Grid<MapEntry> () {}

Map::Map(int w, int h) : Grid<MapEntry> (w, h) {}

Map::Map(Map& map) : Grid<MapEntry> (map.width, map.height)
{
    for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++)
	    (*this)(i, j) = map (i, j);
}

Map::Map(Dem& dem) : Grid<MapEntry> (dem.width, dem.height)
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

RelationType Map::invert_relation (Coord a, Coord b)
{
    int a_idx = a.linear_index (b);
    RelationType a_rel = inverted_relation (point_relation (a, a_idx));

    int b_idx = b.linear_index (a);
    RelationType b_rel = inverted_relation (point_relation (b, b_idx));

    if (a_rel != b_rel)
	eprint ("Impossible inverted relation: [%d][%d]=%c <> [%d][%d]=%c\n",
		a.x, a.y, b.x, b.y, relation2char (a_rel), relation2char (b_rel));

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




///////////////////
///////////////////
///////////////////
///////////////////
///// FLIPPER /////
///////////////////
///////////////////
///////////////////
///////////////////

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


// void init_critical_lines (std::vector<CriticalPoint>& critical,
// 			  std::vector<TrackLine>& lines);




Track* Flipper::track (ScaleSpace& ss)
{
    (void) ss;

    // Track* track = new Track ();

    // init_critical_lines (ss.critical[0], lines);

    // still have to think about how to do it; remember they are class members.
    // critical_original_num = critical_points.size();
    // critical_born_num = 0;
    // dem_width = ss.dems[0]->width;
    // dem_height = ss.dems[0]->height;

    // TrackString ts = TrackString (ss.levels-1);

    // process_flips_all (flip, lines, ss, ts);

    // for (unsigned i = 0; i < fs.size(); i++)
    // 	fs[i].clear();

    // return track;

    return NULL;
}



// void add_critical_line (std::vector<CriticalPoint>& critical,
// 			std::vector<TrackLine>& lines,
// 			int i, int mate)
// {
//     // add to critical point lines
//     TrackLine line;
//     line.type = critical[i].t;
//     TrackEntry te;
//     te.c = critical[i].c;
//     te.t = 0.0;
//     te.mate = mate;
//     te.life = -1;
//     line.entries.push_back (te);
//     lines.push_back(line);
// }
// void init_critical_lines (std::vector<CriticalPoint>& critical,
// 			  std::vector<TrackLine>& lines)
// {
//     for (unsigned i = 0; i < critical.size(); i++)
//     {
// 	if (critical[i].t == SA3)
// 	{
// 	    int cs = lines.size();
// 	    add_critical_line (critical, lines, i, cs+1);
// 	    add_critical_line (critical, lines, i, cs);
// 	}
// 	else
// 	    add_critical_line (critical, lines, i, -1);
//     }

// }
