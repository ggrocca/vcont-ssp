#include "flipper_mesh.hh"
#include "trackstats.hh"

#include <vector>
#include <algorithm>

Map_M::Map_M (Mesh& mesh) : Field<MapEntry_M> (mesh) {}

Map_M::Map_M (Map_M& map) : Field<MapEntry_M> (mesh)
{
    for (unsigned i = 0; i < size(); i++)
	(*this)[i] = map[i];
}

Map_M::Map_M (ScalarField& sc, std::vector<TrackLine_M>& lines)
    : Field<MapEntry_M> (sc.mesh)
{
    for (unsigned i = 0; i < size(); i++)
    {
	std::vector<int> vn;
	mesh.neighborhood (i, vn);

	MapEntry_M me;
	me.set_rel_num (vn.size());
	for (unsigned j = 0; j < vn.size (); j++)
	    me.set_rel (j, sc.point_relation (i, vn[j]));

	(*this)[i] = me;
	
	if (point_type (i) != sc.point_type (i))
	    eprint ("Point [%d]. sctype: %c -- maptype: %c\n", i,
		    critical2char (sc.point_type (i)),
		    critical2char (point_type (i)));
    }

    for (unsigned i = 0; i < lines.size(); i++)
    {
	if (lines[i].entries.size() < 1)
	    eprint ("%s", "Empty line.\n");

	if (lines[i].is_dead())
	    continue;

	int v = lines[i].entries.back().v;
	CriticalType tracktype = lines[i].get_type();
	CriticalType maptype = point_type (v);	

	if (tracktype == MAX || tracktype == MIN || tracktype == SA2)
	    (*this)[v].tr[0] = i;
	else if (tracktype == SA3)
	{
	    if ((*this)[v].tr[0] == -1)
	    {
		(*this)[v].tr[0] = i;
		(*this)[v].tr[1] = lines[i].entries.back().mate;
	    }
	    else if ((*this)[v].tr[0] != lines[i].entries.back().mate ||
		     (*this)[v].tr[1] != (int) i)
		eprint ("%s", "Mismatched 2-saddle.\n");
	}
	else
	    eprint ("%s", "Wrong critical type in Map.\n");

	if (tracktype != maptype)
	    eprint ("lines[%d] -> [%d] _ tracktype:%c _ maptype:%c\n",
		    i, v, critical2char (tracktype), critical2char (maptype));
    }
}

MapEntry_M& Map_M::operator() (int v)
{
    return Field<MapEntry_M>::operator()(v);
}

MapEntry_M& Map_M::operator[] (int v)
{
    return Field<MapEntry_M>::operator[](v);
}

RelationType Map_M::point_relation (int v, int idx)
{
    return (*this)[v].get_rel (idx);
}

int Map_M::point_relation_num (int v)
{
    return (*this)[v].get_rel_num ();
}


CriticalType Map_M::point_type (int v)
{
    RelationType current,previous,first;
    int changes = 0;

    first = previous = point_relation (v, point_relation_num (v) - 1);

    for (int i = 0; i < point_relation_num (v); i++)
    {
	current = point_relation (v, i);
	point_type_step (current, &previous, &changes);
    }

    return point_type_analyze (first, changes, v, point_relation_num (v));
}

CriticalPair Map_M::pair(Edge_M e)
{
    CriticalPair p;
    p.a = point_type (e.l);
    p.b = point_type (e.r);
    return p;

}


RelationType Map_M::invert_relation (Edge_M e)
{
    int a = e.l;
    int b = e.r;

    int a_idx = mesh.neighbor_index (a, b);

    if (a_idx == -1)
    {
	eprint ("%s", "First call to mesh.neighbor_index() failed.\n");
	return EQ;
    }
    
    RelationType a_rel = inverted_relation (point_relation (a, a_idx));

    int b_idx = mesh.neighbor_index (b, a);

    if (b_idx == -1)
    {
	eprint ("%s", "Second call to mesh.neighbor_index() failed.\n");
	return EQ;
    }

    RelationType b_rel = inverted_relation (point_relation (b, b_idx));

    if (a_rel == b_rel)
	eprint ("Impossible inverted relation: [%d]=%c <> [%d]=%c\n",
		a, relation2char (a_rel), b, relation2char (b_rel));

    (*this)[a].set_rel (a_idx, a_rel);
    (*this)[b].set_rel (b_idx, b_rel);

    return a_rel;
}

void Map_M::update_relations (ScalarField* sc, int v)
{
    std::vector<int> vn;
    mesh.neighborhood (v, vn);

    if ((int) vn.size() != point_relation_num (v))
	
    
    for (int i = 0; i < point_relation_num (v); i++)
    {
	(*this)(v).set_rel (i, sc->point_relation (v, vn[i]));
	//if (r.is_inside (width, height)) always inside in a mesh, no virtual bottom
	(*this)(vn[i]).set_rel (mesh.neighbor_index (vn[i], v), sc->point_relation (vn[i], v));
	//(*this)(r).set_rel (r.linear_index (c), d->point_relation (r, c));
    }
}

void create_flips_m (ScalarField* b, ScalarField* n, std::vector<Flip_M>& fs);

Flipper_M::Flipper_M (SurfaceScaleSpace& ss) : flip (ss.levels - 1)
{
    // debug trackstring
    // FlipperDebug::setSS (&ss);

    // std::vector< std::vector<flip_t> > flip (ss.levels - 1);

    flips_total = 0;
    for (int i = 0; i < ss.levels - 1; i++)
    {
	tprint ("Start computing flips, level %d\n", i);

	create_flips_m (ss.fields[i], ss.fields[i+1], flip[i]);
	flips_total += flip[i].size ();

	// GG WARNING SHOULD BE ERROR
	if (SCOPE_FLIP)
	    for (unsigned j = 0; j < flip[i].size(); j++)
	    {
		if (flip[i][j].t > 1.0 || flip[i][j].t < 0.0)
		    tprints (SCOPE_FLIP, "DANGER: flip [%d] [%d] out of time.\n", i, j);
		// print_flip (flip[i][j], ss.fields, i);
	    }

	tprint ("End computing flips, level %d, %20d flips. Total flips:%20d\n",
		i, (int) flip[i].size(), flips_total);
    }
}

void process_flips_all_m (std::vector< std::vector<Flip_M> >& flips,
			  Track_M* track,
			  SurfaceScaleSpace& ss,
			  TrackString& ts);


void Flipper_M::track (SurfaceScaleSpace& ss, Track_M* track)
{
    TrackString ts = TrackString (ss.levels - 1);

    process_flips_all_m (flip, track, ss, ts);

    track->size = ss.fields[0]->size();
}


///////////////////////////////////////////
////////// FLIP CREATION
///////////////////////////////////////////

bool check_flip_m (ScalarField* b, ScalarField* n, Edge_M e)
{
    RelationType prev_rel = b->point_relation (e.l, e.r);
    RelationType succ_rel = n->point_relation (e.l, e.r);
    
    return ((prev_rel == GT && succ_rel == LT) ||
	    (prev_rel == LT && succ_rel == GT) );
}

double get_flip_time_m (ScalarField* b, ScalarField* n, Edge_M e)
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

void create_flips_m (ScalarField* b, ScalarField* n, std::vector<Flip_M>& fs)
{
    fs.clear ();
    
    std::vector< std::vector<int> > edges;
    b->mesh.edges (edges);
    
    for (unsigned i = 0; i < edges.size(); i++)
    {
	Edge_M e;
	e = Edge_M (edges[i][0], edges[i][1]);
	
	if (check_flip_m (b, n, e))
	{
	    Flip_M flip;
	    flip.e = e;
	    flip.t = get_flip_time_m (b, n, e);
	    fs.push_back (flip);
	}
    }

    std::sort (fs.begin(), fs.end());
}


///////////////////////////////////////////
/////////////// FLIP PROCESSING
///////////////////////////////////////////

void init_critical_lines_m (std::vector<CriticalVertex>& critical,
			  std::vector<TrackLine_M>& lines);

void process_flips_m (std::vector<Flip_M>& flips, Map_M* map,
		    std::vector<TrackLine_M>& lines,
		    int base, TrackString& ts);

void process_flips_all_m (std::vector< std::vector<Flip_M> >& flips,
			  Track_M* track,
			  SurfaceScaleSpace& ss,
			  TrackString& ts)
{
    //-- ss.fields[0]->mesh.print_info (1);
    init_critical_lines_m (ss.criticals[0], track->lines);
    //-- ss.fields[0]->mesh.print_info (2);

    Map_M* map = NULL;
    // Map* oldmap;

    for (unsigned i = 0; i < flips.size(); i++)
    {
	// oldmap = (map)? map : NULL;
	//-- ss.fields[0]->mesh.print_info ((3*100)+(i*10)+1);

	map = new Map_M (*(ss.fields[i]), track->lines);

	//-- ss.fields[0]->mesh.print_info ((3*100)+(i*10)+2);

	// tprint ("begin_ check map, level %d\n", i);

	// // must check with dem also.

	// if (!map->check_relation (*(ss.dem[i])))
	//     eprint ("Check map. level %d.\n", i);
	// if (oldmap)
	// {
	//     map->check_relation (*oldmap);
	//     map->check_reference (*oldmap);
	//     delete oldmap;
	// }

	// tprint ("end_ check map, level %d\n", i);

	//-- ss.fields[0]->mesh.print_info ((3*100)+(i*10)+4);

    	process_flips_m (flips[i], map, track->lines, i, ts);

	//-- ss.fields[0]->mesh.print_info ((3*100)+(i*10)+5);

	track->time_of_life = (double) i + 1;
	
	//print_stats (track);
    }

    ts.print ();

    delete map;
}

void add_critical_line_m (std::vector<CriticalVertex>& critical,
			  std::vector<TrackLine_M>& lines,
			  int i, int mate)
{
    // add to critical point lines
    TrackLine_M line;
    line.type = critical[i].t;
    TrackEntry_M te;
    te.v = critical[i].v;
    te.t = 0.0;
    te.mate = mate;
    te.life = -1;
    line.entries.push_back (te);
    lines.push_back(line);
}

void init_critical_lines_m (std::vector<CriticalVertex>& critical,
			    std::vector<TrackLine_M>& lines)
{
    for (unsigned i = 0; i < critical.size(); i++)
    {
	if (critical[i].t == SA3)
	{
	    int cs = lines.size();
	    add_critical_line_m (critical, lines, i, cs+1);
	    add_critical_line_m (critical, lines, i, cs);
	}
	else
	    add_critical_line_m (critical, lines, i, -1);
    }

}

void pair_insert_m (std::vector<TrackLine_M>& lines,
		    Map_M* map, Flip_M f,
		    CriticalPair before, CriticalPair after, int base);

void process_flips_m (std::vector<Flip_M>& flips, Map_M* map,
		      std::vector<TrackLine_M>& lines,
		      int base, TrackString& ts)
{
    for (unsigned i = 0; i < flips.size(); i++)
    {
	CriticalPair before, after;

	before = map->pair (flips[i].e);
	map->invert_relation (flips[i].e);
	after = map->pair (flips[i].e);

	ts.insert (base, before, after);

	pair_insert_m (lines, map, flips[i], before, after, base);
    }
}

// 1m -> RR
// m1 -> RR
// 1M -> RR
// M1 -> RR
void death_1 (std::vector<TrackLine_M>& lines, Map_M* map,
	      TrackEntry_M te0, TrackEntry_M te1)
{
    // get entries
    int i0 = ((*map)(te0.v)).get_ref_first ();
    int i1 = ((*map)(te1.v)).get_ref_first ();

    // delete refs in map
    ((*map)(te0.v)).reset_ref_first ();
    ((*map)(te1.v)).reset_ref_first ();
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
void death_2 (std::vector<TrackLine_M>& lines, Map_M* map,
	      CriticalType ct0, TrackEntry_M ce0,
	      CriticalType ct1, TrackEntry_M ce1)
{
    TrackEntry_M sal_e, sar_e;
    TrackEntry_M mm_e;

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
    int is = ((*map)(sal_e.v)).get_ref_second ();
    int is_mate = ((*map)(sal_e.v)).get_ref_first ();
    int im = ((*map)(mm_e.v)).get_ref_first ();

    // in map, we lose reference to the second saddle and minmax
    ((*map)(sal_e.v)).reset_ref_second ();
    ((*map)(mm_e.v)).reset_ref_first ();

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
void birth_1 (std::vector<TrackLine_M>& lines, Map_M* map,
	      CriticalType ct0, TrackEntry_M te0,
	      CriticalType ct1, TrackEntry_M te1)
{
    // get future indices
    int i_te0 = lines.size();
    int i_te1 = lines.size() + 1;

    // create new lines and push birth of both points
    TrackLine_M nuline0, nuline1;
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
    ((*map)(te0.v)).set_ref_first (lines.size() - 2);
    ((*map)(te1.v)).set_ref_first (lines.size() - 1);
}

// R1 -> M2
// 1R -> 2M
// 1R -> 2m
// R1 -> m2
void birth_2 (std::vector<TrackLine_M>& lines, Map_M* map,
	      CriticalType ct0, TrackEntry_M te0,
	      CriticalType ct1, TrackEntry_M te1)
{
    // force 0 to be the 2-saddle
    if (ct1 == SA3)
    {
	CriticalType cts = ct0;
	ct0 = ct1;
	ct1 = cts;
	TrackEntry_M tes = te0;
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
    int i_ps = ((*map)(te0.v)).get_ref_first (); // me0.tr[0];
    int i_te0 = lines.size();
    int i_te1 = lines.size() + 1;

    // update previous saddle with new mate
    TrackEntry_M te_ps = te0;
    te_ps.mate = i_te0;
    te_ps.life = -1;
    lines[i_ps].entries.push_back (te_ps);

    // create new lines. new saddle has previous as a mate
    TrackLine_M nuline0, nuline1;
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
    ((*map)(te0.v)).set_ref_second (i_te0);
    // me0.tr[1] = i_te0;
    // map->set_entry (te0.x, te0.y, me0);
    ((*map)(te1.v)).set_ref_first (i_te1);
    // me1.tr[0] = i_te1;
    // map->set_entry (te1.x, te1.y, me1);
}

// 1R -> R1
// R1 -> 1R
// Rm -> mR
// mR -> Rm
// MR -> RM
// RM -> MR
void move_1 (std::vector<TrackLine_M>& lines, Map_M* map,
	     TrackEntry_M from_e, TrackEntry_M to_e)
{
    // get "from" map entry, retrieve index and set it to -1
    int i = ((*map)(from_e.v)).get_ref_first (); // from_e -- me.tr[0];
    ((*map)(from_e.v)).reset_ref_first ();

    // get "to" map entry, set index
    ((*map)(to_e.v)).set_ref_first (i);

    // push new line entry
    lines[i].entries.push_back (to_e);
}

// 12 -> 21
// 21 -> 12
void move_3 (std::vector<TrackLine_M>& lines, Map_M* map,
	     TrackEntry_M from_ce, TrackEntry_M to_ce)
{
    TrackEntry_M moving_ce = to_ce;
    // map_entry from_me, to_me;

    // get "from" map entry, retrieve both indexes
    int old_mate_i = ((*map)(from_ce.v)).get_ref_first ();// from_me.tr[0];
    int moving_i = ((*map)(from_ce.v)).get_ref_second (); // from_me.tr[1];
    // from_me = map->get_entry (from_ce.x, from_ce.y);

    // get "to" map entry, retrieve index
    int new_mate_i = ((*map)(to_ce.v)).get_ref_first ();

    // map reference lose second reference in from, gains it in to
    ((*map)(from_ce.v)).reset_ref_second ();
    // from_me.tr[1] = -1;
    // map->set_entry (from_ce.x, from_ce.y, from_me);
    ((*map)(to_ce.v)).set_ref_second (moving_i);
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
void move_21 (std::vector<TrackLine_M>& lines, Map_M* map,
	     TrackEntry_M from_ce, TrackEntry_M to_ce)
{
    // retrieve both indexes
    int old_mate_i = ((*map)(from_ce.v)).get_ref_first (); //from_me.tr[0];
    int moving_i = ((*map)(from_ce.v)).get_ref_second (); //from_me.tr[1];

    // map reference lose second reference in from, gains it in to
    ((*map)(from_ce.v)).reset_ref_second (); //from_me.tr[1] = -1;
    ((*map)(to_ce.v)).set_ref_first (moving_i); // to_me.tr[0] = moving_i;

    // from loses its mate
    from_ce.mate = -1;
    lines[old_mate_i].entries.push_back (from_ce);

    // moving has no mate
    to_ce.mate = -1;
    lines[moving_i].entries.push_back (to_ce);
}

// 11 -> R2
// 11 -> 2R
void move_12 (std::vector<TrackLine_M>& lines, Map_M* map,
	     TrackEntry_M from_ce, TrackEntry_M to_ce)
{
    TrackEntry_M moving_ce = to_ce;

    // get "from" map entry, retrieve index
    int from_i = ((*map)(from_ce.v)).get_ref_first (); //from_me.tr[0];

    // get "to" map entry, retrieve index
    int to_i = ((*map)(to_ce.v)).get_ref_first (); //to_me.tr[0];

    // map from reference loses its ref
    ((*map)(from_ce.v)).reset_ref_first ();

    // to gains reference to second saddle
    ((*map)(to_ce.v)).set_ref_second (from_i);

    // to gains new mate
    to_ce.mate = from_i;
    lines[to_i].entries.push_back (to_ce);
    
    // moving gains the saddle it's reaching as a mate.
    moving_ce.mate = to_i;
    lines[from_i].entries.push_back (moving_ce);
}

void pair_insert_m (std::vector<TrackLine_M>& lines,
		    Map_M* map, Flip_M f,
		    CriticalPair before, CriticalPair after, int base)
{
    CriticalType bl, br, al, ar;
    bl = before.a;
    br = before.b;
    al = after.a;
    ar = after.b;

    double current_time = f.t + ((double) base);

    TrackEntry_M el (f.e.l, current_time);
    TrackEntry_M er (f.e.r, current_time);

    // FlippingEvent event;
    // event.time = current_time;

    if (before.sum() != after.sum())
    {
	eprint ("%s", "Different sum, Euler formula not respected.\n");
	goto exit_error;
    }

    if (bl == al && br == ar)
    {
	if ((bl == REG || bl == SA2) && (br == REG || br == SA2))
	    goto exit_regular;
	else
	{
	    eprint ("%s", "Should have been regular.\n");
	    goto exit_error;
	}
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
	eprint ("%s", "Should have been a normal move.\n");
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
	eprint ("%s", "Should have been birth-death-2move.\n");
	goto exit_error;
    }

    eprint ("%s", "Same sum, uncatched move.\n");

 exit_error:
    // if (SCOPE_TRACKING)
    // 	FlipperDebug::print_flip_interpolated (f, before, after, map, base);
    eprintx (42, "%s", "Exit error!\n");

 exit_success:
    // events.push_back (event);

 exit_regular:
    return;
}

    
