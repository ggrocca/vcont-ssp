#include "trackstats.hh"

elem::elem (CriticalPair before, CriticalPair after)
{
    move[0] = critical2char(before.a);
    move[1] = critical2char(before.b);
    move[2] = critical2char(after.a);
    move[3] = critical2char(after.b);
    move[4] = '\0';

    count = 1;
}

elem::elem (const elem& copy)
{
    count = copy.count;
    for (int i = 0; i < __MOVELEN; i++)
	move[i] = copy.move[i];    
}

elem& elem::operator=(const elem& rhs)
{
    this->count = rhs.count;
    for (int i = 0; i < __MOVELEN; i++)
	this->move[i] = rhs.move[i];    
    return *this;
}

elem& elem::operator++()
{
    (this->count)++;
    return *this;
}

bool elem::operator<(const elem& rhs)
{
    return strcmp (this->move, rhs.move) > 0;
}

bool elem::operator==(const elem& rhs)
{
    return strcmp (this->move, rhs.move) == 0;
}

void elem::values (int* before, int* after)
{
    *before = char2value (move[0]) + char2value (move[1]);
    *after = char2value (move[2]) + char2value (move[3]);
}


bool _sort_elem_count(elem& lhs, elem& rhs)
{
    return lhs.count > rhs.count;
}



void trackstring_insert (std::list<elem> & trackstring, CriticalPair before, CriticalPair after)
{
    elem enew (before, after);

    std::list<elem>::iterator i;
    for (i = trackstring.begin(); i != trackstring.end(); ++i)
	if (*i < enew)
	    continue;
	else if (*i == enew)
	{
	    ++(*i);
	    break;
	}
	else
	{
	    trackstring.insert (i, enew);
	    break;
	}
    if (i == trackstring.end())
	trackstring.push_back (enew);
}

void trackstring_print (std::list<elem> & trackstring)
{
    int size = 0;
    int total = 0;

    trackstring.sort (_sort_elem_count);

    printf ("R: regular; M: maximum; m: minimum; 1: 1-saddle; 2: 2-saddle\n");

    for (std::list<elem>::iterator i = trackstring.begin();
	 i != trackstring.end(); ++i)
    {
	size++;
	total += (*i).count;
	int before, after;
	(*i).values (&before, &after);

	printf ("!!! Eq:%d == %c%c -> %c%c  == [%d]\t\t"
		"before.sum: %d, after.sum: %d.\n",
		before == after,
		(*i).move[0], (*i).move[1], (*i).move[2], (*i).move[3], 
		(*i).count, before, after);
    }
    printf ("!!! trackstring: total moves %d, found %d different types\n",
	    total, size);

    trackstring.sort();
}

TrackString::TrackString (int levels)
{
    global = std::list<elem> ();
    local = std::vector< std::list<elem> > (levels);    
}

void TrackString::insert (int level, CriticalPair a, CriticalPair b)
{
    trackstring_insert (global, a, b);
    trackstring_insert (local[level], a, b);
}

void TrackString::print ()
{
    for (unsigned i = 0; i < local.size(); i++)
    {
	printf ("\n\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	printf ("$$$$$$$ TRACKSTRING LOCAL, LEVEL %d\n", i);
	trackstring_print (local[i]);
	printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    }
    printf ("\n\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    printf ("$$$$$$$ TRACKSTRING GLOBAL\n");    
    trackstring_print (global);
    printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
}


void FlipperDebug::print_flip_interpolated (Flip f,
					    CriticalPair before, CriticalPair after,
					    Map* map, int base)
{
    Dem* dem = _ss->dem[base];
    Dem* next = _ss->dem[base+1];

    CriticalType bl, br, al, ar;
    bl = before.a;
    br = before.b;
    al = after.a;
    ar = after.b;

    TrackEntry el, er;
    el.t = er.t = f.t;
    el.c = f.e.l;
    er.c = f.e.r;

    tprints (SCOPE_TRACKING, "%s", "======================\n");
    tprints (SCOPE_TRACKING, "flip: t=%f, (%d,%d)--(%d,%d)",
	     f.t, el.c.x, el.c.y, er.c.x, er.c.y);
    tprints (SCOPE_TRACKING, "type: %c,%c ---> %c,%c. before: %d, after %d \n",
	    critical2char(bl),critical2char(br),
	    critical2char(al),critical2char(ar),
	    before.sum(), after.sum());
    map->point_print (SCOPE_TRACKING, el.c);
    dem->point_print (SCOPE_TRACKING, el.c);
    next->point_print (SCOPE_TRACKING, el.c);
    _ss->point_print_interpolated (SCOPE_TRACKING, base, f.t, el.c);
    map->point_print (SCOPE_TRACKING, er.c);
    dem->point_print (SCOPE_TRACKING, er.c);
    next->point_print (SCOPE_TRACKING, er.c);
    _ss->point_print_interpolated (SCOPE_TRACKING, base, f.t, er.c);
    tprints (SCOPE_TRACKING, "%s", "======================\n");
}

void FlipperDebug::eprint_invert_map_flip (Coord a, Coord b,
			     RelationType r1, RelationType r2, int base)
{
    Dem* dem = _ss->dem[base];
    eprint ("Same edge, relation discordance!"
	    " r1:%d, r2:%d -- ax:%d, ay:%d -- bx:%d, by:%d || dr1:%d, dr2: %d\n",
	    r1, r2, a.x, a.y, b.x, b.y,
	    dem->point_relation (a, b),
	    dem->point_relation (a, b));
}

ScaleSpace* FlipperDebug::_ss;

void FlipperDebug::setSS (ScaleSpace* ss)
{
    _ss = ss;
}

void __acc_stats (double curr, double *av, double *max, double *min)
{
    if (av)
	*av += curr;

    if (max && *max < curr)
	*max = curr;

    if (min && *min > curr)
	*min = curr;
}

void print_stats (std::vector<TrackLine>& cps)
{
    int total, original_num, born_num, dead_num, alive_num,
	born_dead, born_alive, original_dead, original_alive;

    total = original_num = born_num = dead_num = alive_num =
	born_dead = born_alive = original_dead = original_alive = 0;

    double av_lifetime_dead, av_lifetime_original_dead, av_lifetime_born_dead,
	av_lifetime_born_alive, min_lifetime_dead, max_lifetime_dead, 
	min_lifetime_born_alive, max_lifetime_born_alive;

    double av_travel, min_travel, max_travel,
	av_travel_alive, min_travel_alive, max_travel_alive,
	av_travel_original_alive, min_travel_original_alive, max_travel_original_alive;

    
    av_lifetime_dead = av_lifetime_original_dead = av_lifetime_born_dead =
	av_lifetime_born_alive = av_travel = av_travel_alive =
	av_travel_original_alive = 0.0f;
    max_lifetime_dead = max_lifetime_born_alive =
	max_travel = max_travel_alive = max_travel_original_alive = -DBL_MAX;
    min_lifetime_dead = min_lifetime_born_alive =
	min_travel = min_travel_alive = min_travel_original_alive = DBL_MAX;

    for (unsigned i = 0; i < cps.size(); i++)
    {
	total++;
	cps[i].is_original()? original_num++ : original_num ;
	cps[i].is_born()? born_num++ : born_num ;
	cps[i].is_dead()? dead_num++ : dead_num ;
	cps[i].is_alive()? alive_num++ : alive_num ;
	cps[i].is_born() && cps[i].is_alive()? born_alive++ : born_alive ;
	cps[i].is_born() && cps[i].is_dead()? born_dead++ : born_dead ;
	cps[i].is_original() && cps[i].is_alive()? original_alive++ : original_alive ;
	cps[i].is_original() && cps[i].is_dead()? original_dead++ : original_dead ;

	__acc_stats (cps[i].travel(), &av_travel,
		     &max_travel, &min_travel);

	if (cps[i].is_dead())
	    __acc_stats (cps[i].lifetime(), &av_lifetime_dead,
			 &max_lifetime_dead, &min_lifetime_dead);

	if (cps[i].is_alive())
	    __acc_stats (cps[i].travel(), &av_travel_alive,
			 &max_travel_alive, &min_travel_alive);

	if (cps[i].is_original() && cps[i].is_dead())
	    __acc_stats (cps[i].lifetime(), &av_lifetime_original_dead, 0, 0);

	if (cps[i].is_original() && cps[i].is_alive())
	    __acc_stats (cps[i].travel(), &av_travel_original_alive,
			 &max_travel_original_alive, &min_travel_original_alive);

	if (cps[i].is_born() && cps[i].is_dead())
	    __acc_stats (cps[i].lifetime(), &av_lifetime_born_dead, 0, 0);

	if (cps[i].is_born() && cps[i].is_alive())
	    __acc_stats (cps[i].lifetime(), &av_lifetime_born_alive,
			 &max_lifetime_born_alive, &min_lifetime_born_alive);
    }

    av_lifetime_dead /= ((double) dead_num);
    av_lifetime_original_dead /= ((double) original_dead);
    av_lifetime_born_dead /= ((double) born_dead);
    av_lifetime_born_alive /= ((double) born_alive);
    av_travel /= ((double) total);
    av_travel_alive /= ((double) alive_num);
    av_travel_original_alive /= ((double) original_alive);

    printf ("\n");
    printf ("************************\n");
    printf ("\n");
    printf ("total:             %d\n", total);
    printf ("original_num:      %d\n", original_num);
    printf ("born_num:          %d\n", born_num);
    printf ("dead_num:          %d\n", dead_num);
    printf ("alive_num:         %d\n", alive_num);
    printf ("born_dead:         %d\n", born_dead);
    printf ("born_alive:        %d\n", born_alive);
    printf ("original_dead:     %d\n", original_dead);
    printf ("original_alive:    %d\n", original_alive);
    printf ("\n");
    printf ("------------------------\n");
    printf ("\n");
    printf ("av_lifetime_born_alive:       %lf\n", av_lifetime_born_alive);
    printf ("max_lifetime_born_alive:      %.20lf\n", max_lifetime_born_alive);
    printf ("min_lifetime_born_alive:      %lf\n", min_lifetime_born_alive);
    printf ("\n");
    printf ("av_lifetime_dead:             %lf\n", av_lifetime_dead);
    printf ("max_lifetime_dead:            %lf\n", max_lifetime_dead);
    printf ("min_lifetime_dead:            %.20lf\n", min_lifetime_dead);
    printf ("\n");
    printf ("av_lifetime_born_dead:        %lf\n", av_lifetime_born_dead);
    printf ("av_lifetime_original_dead:    %lf\n", av_lifetime_original_dead);
    printf ("\n");
    printf ("av_travel:                    %lf\n", av_travel);
    printf ("max_travel:                   %lf\n", max_travel);
    printf ("min_travel:                   %.20lf\n", min_travel);
    printf ("\n");
    printf ("av_travel_alive:              %lf\n", av_travel_alive);
    printf ("max_travel_alive:             %lf\n", max_travel_alive);
    printf ("min_travel_alive:             %.20lf\n", min_travel_alive);
    printf ("\n");
    printf ("av_travel_original_alive:     %lf\n", av_travel_original_alive);
    printf ("max_travel_original_alive:    %lf\n", max_travel_original_alive);
    printf ("min_travel_original_alive:    %.20lf\n", min_travel_original_alive);
    printf ("\n");
    printf ("************************\n");
    printf ("\n");

}
