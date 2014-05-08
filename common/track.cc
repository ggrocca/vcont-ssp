#include "track.hh"

#include <math.h>

int Track::get_other_death (int i)
{
    if (!is_dead (i))
	eprintx (42, "Critical %d not dead.\n", i);

    return lines[i].entries.back ().life;
}


int Track::get_other_birth (int i)
{
    if (!is_born (i))
	eprintx (42, "Critical %d not born.\n", i);

    return lines[i].entries[0].life;
}


double Track::death_time (int i)
{
    if (!is_dead (i))
	eprintx (42, "Critical %d not dead.\n", i);

    return lines[i].entries.back ().t;
}


double Track::birth_time (int i)
{
    if (!is_born (i))
	eprintx (42, "Critical %d not born.\n", i);

    return lines[i].entries[0].t;
}



CriticalType Track::original_type (int i)
{
    return lines[i].type;
}


double Track::lifetime_elixir (int i)
{
    return lines[i].elixir;
}

// returns index to first alive entry occurring before t.
// -1 if not found
int TrackLine::get_entry (double t)
{
    double pt = 0.0;
    double nt = 0.0;

    for (unsigned j = 0; j < entries.size(); j++)
    {
	nt = entries[j].t;

	if ((t >= pt && t <= nt) || j == entries.size() - 1)
	    return is_alive (j)? j : -1;

	pt = nt;
    }

    return -1;
}

int Track::get_entry (int i, double t)
{
    return lines[i].get_entry (t);
}


CriticalType TrackLine::get_type ()
{
    if (entries.size() < 1)
    {
	eprintx (42, "%s", "Empty line.\n");
	return EQU;
    }
    if (type == MAX || type == MIN)
	return type;

    if (type == SA2 || type == SA3)
	return (entries.back().mate == -1)? SA2 : SA3;

    eprintx (42, "%s", "Returning wrong type.\n");
    return EQU;
}

CriticalType Track::get_type (int i)
{
    return lines[i].get_type ();
}


CriticalType TrackLine::get_type (unsigned j)
{
    if (j >= entries.size())
    {
	eprintx (42, "%s", "Out of bounds.\n");
	return EQU;
    }
    if (type == MAX || type == MIN)
	return type;

    if (type == SA2 || type == SA3)
	return (entries[j].mate == -1)? SA2 : SA3;

    eprintx (42, "%s", "Returning wrong type.\n");
    return EQU;
}

CriticalType Track::get_type (int i, unsigned j)
{
    return lines[i].get_type (j);
}


bool TrackLine::is_dead ()
{
    if (entries.size() < 1)
    {
	eprintx (42, "%s", "Empty line.\n");
	return true;
    }

    return (entries.size() > 1 && entries.back().life != -1);
}

bool Track::is_dead (int i)
{
    return lines[i].is_dead ();    
}

bool TrackLine::is_dead (unsigned j)
{
    if (j >= entries.size())
    {
	eprintx (42, "%s", "Out of bounds.\n");
	return true;
    }

    return (j >= 1 && entries[j].life != -1);
}

bool Track::is_dead (int i, unsigned j)
{
    return lines[i].is_dead (j);
}


bool TrackLine::is_alive ()
{
    return !is_dead();
}

bool Track::is_alive (int i)
{
    return lines[i].is_alive ();
}


bool TrackLine::is_alive (unsigned j)
{
    return !is_dead(j);
}

bool Track::is_alive (int i, unsigned j)
{
    return lines[i].is_alive (j);
}

// bool TrackLine::is_active ()
// {
//     if (entries.size() < 1)
//     {
// 	eprintx (42, "%s", "Empty line.\n");
// 	return true;
//     }

//     return
// 	!
// 	(is_)
// }

// bool TrackLine::is_active (unsigned j)
// {
//     return !is_dead(j);
// }

bool TrackLine::is_original ()
{
    if (entries.size() < 1)
    {
	eprintx (42, "%s", "Empty line\n");
	return false;
    }
    return (entries[0].life == -1);
}

bool Track::is_original (int i)
{
    return lines[i].is_original ();    
}

bool TrackLine::is_born ()
{
    return !is_original();
}

bool Track::is_born (int i)
{
    return lines[i].is_born ();    
}


bool TrackLine::is_getting_born (unsigned j)
{
    return is_born () && j == 0;
}

bool Track::is_getting_born (int i, unsigned j)
{
    return lines[i].is_getting_born (j);        
}

double TrackLine::travel ()
{
    double t = 0.0f;
	
    if (entries.size() == 1)
	return t;

    for (unsigned i = 1; i < entries.size(); i++)
    {
	int diff_x = abs (entries[i-1].c.x - entries[i].c.x);
	int diff_y = abs (entries[i-1].c.y - entries[i].c.y);
	if (diff_x == 1 && diff_y == 1)
	    t += sqrt(2.0f);
	else if (diff_x == 1 || diff_y == 1)
	    t += 1.0f;
	else if (diff_x == 0 && diff_y == 0)
	    continue;
	else
	    eprintx (42, "%s", "Wrong travel\n");
    }
    return t;
}

double Track::travel (int i)
{
    return lines[i].travel ();            
}

// double TrackLine::lifetime ()
// {
//     if (is_dead())
// 	return entries.back().t - entries[0].t; 
//     if (is_alive() && is_original())
// 	return Track::time_of_life;
//     if (is_alive() && is_born())
// 	return Track::time_of_life - entries[0].t;
//     eprintx (42, "%s", "Time travel.\n");
//     return 0.0;
// }

double Track::lifetime (int i)
{
    if (is_dead(i))
	return lines[i].entries.back().t - lines[i].entries[0].t; 
    if (is_alive(i) && is_original(i))
	return time_of_life;
    if (is_alive(i) && is_born(i))
	return time_of_life - lines[i].entries[0].t;
    eprintx (42, "%s", "Time travel.\n");
    return 0.0;
}


// Point TrackLine::final_point (std::vector<TrackLine>& lines)
// {
//     Point p;
//     double x =  entries.back().c.x;
//     double y =  entries.back().c.y;

//     if (is_alive())
//     {
// 	p.x = x;
// 	p.y = y;
//     }

//     else
//     {
// 	int life = entries.back().life;
// 	double lx = lines[life].entries.back().c.x;
// 	double ly = lines[life].entries.back().c.y;
// 	p.x = x + ((lx - x)/2.0);
// 	p.y = y + ((ly - y)/2.0);
//     }
//     return p;
// }

Point Track::final_point (int i)
{
    Point p;
    double x =  lines[i].entries.back().c.x;
    double y =  lines[i].entries.back().c.y;

    if (is_alive(i))
    {
	p.x = x;
	p.y = y;
    }

    else
    {
	int life = lines[i].entries.back().life;
	double lx = lines[life].entries.back().c.x;
	double ly = lines[life].entries.back().c.y;
	p.x = x + ((lx - x)/2.0);
	p.y = y + ((ly - y)/2.0);
    }
    return p;
}

// Point TrackLine::start_point (std::vector<TrackLine>& lines)
// {
//     Point p;
//     double x =  entries[0].c.x;
//     double y =  entries[0].c.y;
    
//     if (!is_born ())
//     {
//         p.x = x;
//         p.y = y;
//     }
//     else
//     {
// 	int life = entries[0].life;
// 	double lx = lines[life].entries[0].c.x;
// 	double ly = lines[life].entries[0].c.y;
// 	p.x = x + ((lx - x)/2.0);
// 	p.y = y + ((ly - y)/2.0);
//     }
    
//     return p;
// }

Point Track::start_point (int i)
{
    Point p;
    double x =  lines[i].entries[0].c.x;
    double y =  lines[i].entries[0].c.y;

    if (!is_born (i))
    {
	p.x = x;
	p.y = y;
    }
    else
    {
	int life = lines[i].entries[0].life;
	double lx = lines[life].entries[0].c.x;
	double ly = lines[life].entries[0].c.y;
	p.x = x + ((lx - x)/2.0);
	p.y = y + ((ly - y)/2.0);
    }

    return p;
}


// GG old ideas

// int time2index (double t); // get valid line index for given time.
// bool is_alive (int i);
// bool is_dead (int i);
// bool is_alive (double t);
// bool is_dead (double t);

// se mi faccio tornare le entry a un certo indice e tempo, e la
// linea a un certo indice e tempo, poi posso ricostruire la
// struttura a un certo tempo.

// double Track::time_of_life;

// Track::Track () {}
// Track::~Track () {}
// Track (ScaleSpace& ss); // should be implemented as flipper algorithm
Track::Track (FILE *f)
{
    int critical_points_num = 0;
    fscanf (f, "#lines: %d\n", &critical_points_num);
    fscanf (f, "tol: %lf\n\n\n", &time_of_life);

    tprints (SCOPE_TRACKING, "reading %d criticals, lifetime %lf\n",
	    critical_points_num, time_of_life);

    for (int i = 0; i < critical_points_num; i++)
    {
	char ctype;
	int lines_num = 0;
	double eli, str, sca;
	fscanf (f, "type: %c\n"
		"eli: %lf\nstr: %lf\nsca: %lf\n"
		"#entries: %d\n\n",
		&ctype,
		&eli, &str, &sca,
		&lines_num);
		// critical2char (critical_points[i].type), 
		// critical_points[i].line.size());
	TrackLine newline;
	newline.type = char2critical (ctype);
	newline.elixir = eli;
	newline.strength = str;
	newline.scale = sca;
	lines.push_back (newline);

	for (int j = 0; j < lines_num; j++)
	{
	    TrackEntry newentry;
	    fscanf (f, "x:%d y:%d ## mate:%d life:%d\ntime: %lf\n", 
		    &(newentry.c.x),
		    &(newentry.c.y),
		    &(newentry.mate),
		    &(newentry.life),
		    &(newentry.t));
	    lines[i].entries.push_back (newentry);
	}
	fscanf (f, "\n\n");
    }
}


void Track::get_strength (Dem *d)
{
    for (unsigned i = 0; i < lines.size(); i++)
    {
	if (lines[i].is_original ())
	{
	    double strength;
	    int kernel_max;
	    // double scale = ScaleSpace::time2scale (lines[i].entries[j].t);

	    CriticalPoint cp = CriticalPoint (lines[i].entries[0].c, lines[i].type);
	    int kernel = ScaleSpace::time2window (lines[i].elixir);

	    d->averhood_max (cp, kernel, &strength, &kernel_max);

	    lines[i].strength = strength;
	    lines[i].scale = ScaleSpace::window2scale (kernel_max);
	}
    }
}

void Track::drink_elixir ()
{
    intoxicated = 0;

    for (unsigned i = 0; i < lines.size(); i++)
    {
	if (is_born (i))
	{
	    lines[i].elixir = 0.0;
	    continue;
	}

	if (is_alive (i))
	{
	    lines[i].elixir = time_of_life;
	    continue;
	}

	int cur = i;
	CriticalType start_type = original_type (i);
	double longest = 0.0;

	while (true)
	{
	    if (longest < death_time (cur))
		longest = death_time (cur);

	    int od = get_other_death (cur); // od is companion type

	    if (is_original (od))
		break;

	    int ob = get_other_birth (od); // ob should be same type

	    // different type
	    if ((original_type (ob) == MAX || original_type (ob) == MIN) && start_type != original_type (ob))
	    {
		// eprint ("Elixir for %d, found different type\n", i);
		intoxicated++;
		break;
	    }
		
	    if (is_alive (ob))
	    {
		longest = time_of_life;
		break;
	    }
	    
	    cur = ob;
	}
	
	lines[i].elixir = longest;
    }    
}

// Track* Track::afterlife ()
// {
//     int originals_num = 0;
//     Track* af = new Track();
    
//     // init afterlife
//     /////////////////////
//     af->time_of_life = time_of_life;

//     for (int i = 0; i < lines.size(); i++)
//     {
// 	TrackLine tl;
// 	tl.type = lines[i].type;

// 	if (is_original (i))
// 	{
// 	    originals_num++;
// 	    for (int j = 0; j < lines[i].entries.size(); j++)
// 	    {
// 		TrackEntry te;
// 		te.c = lines[i].entries[j].c;
// 		te.t = lines[i].entries[j].t;
// 		te.mate = lines[i].entries[j].mate;
// 		te.life = lines[i].entries[j].life;
// 		tl.entries.push_back (te);
// 	    }
// 	}

// 	if (is_born (i))
// 	{
// 	    // put placeholder to store afterlife travel information
// 	    TrackEntry te ();
// 	    tl.entries.push_back (te);
// 	}

// 	af->entries.push_back (tl);
//     }


//     // let's transmigrate
//     /////////////////////
    
//     for (int i = 0; i < originals_num; i++)
//     {
// 	if (is_alive (i))
// 	    continue;

// 	// find farthest point in afterlife
// 	double far_death_time = death_time (i);
// 	int far_point = i;
// 	int current_point = i;

// 	while (true)
// 	{
	    
// 	    ////////////// hic sunt leones


// 	    int next_death = get_other_death (current_point);
// 	    int next_death_time = death_time (current_point);

// 	    if (next_death_time > current_death_time)
// 	    {
// 		next_death_time = current_death_time;
// 	    }
	    
// 	    if (is_original (next_death))
		
// 		///////// leones

// 	}

// 	//// dracos
	
//     }
    
// }

void Track::write (FILE *f)
{
    fprintf (f, "#lines: %zu\n", lines.size());
    fprintf (f, "tol: %.80lf\n\n\n", time_of_life);

    for (unsigned i = 0; i < lines.size(); i++)
    {
	fprintf(f, "type: %c\n"
		"eli: %.80lf\nstr: %.80lf\nsca: %.80lf\n"
		"#entries: %zu\n\n",
		critical2char (lines[i].type),
		lines[i].elixir, lines[i].strength, lines[i].scale,
		lines[i].entries.size());

	for (unsigned j = 0; j < lines[i].entries.size(); j++)
	    fprintf (f, "x:%d y:%d ## mate:%d life:%d\ntime: %.80lf\n", 
		     lines[i].entries[j].c.x,
		     lines[i].entries[j].c.y,
		     lines[i].entries[j].mate,
		     lines[i].entries[j].life,
		     lines[i].entries[j].t);
	fprintf (f, "\n\n");
    }
}

void Track::query (double t, std::vector<TrackRenderingEntry>& v, bool verbose)
{
    TrackRenderingEntry r;

    v.resize (0);
    for (unsigned i = 0; i < lines.size(); i++)
    {
	lines[i].mark = false;
	int j = lines[i].get_entry (t);
	if (j != -1)
	{
	    r.type = lines[i].get_type(j);
	    r.tc = lines[i].entries[j].c;
	    r.oc = lines[i].entries[0].c;
	    r.idx = j;
	    // r.tol = lines[i].lifetime();
	    r.tol = lifetime(i);
	    r.tob = lines[i].entries[0].t;
	    r.is_born = lines[i].is_born ();
	    r.is_alive = lines[i].is_alive ();
	    Point p = final_point(i);
	    r.fx = p.x; r.fy = p.y;
	    v.push_back (r);
	    lines[i].mark = true;
	}
    }
    if (verbose)
	tprintp ("!!!", "Found %zu criticals\n", v.size());
}

// void TrackOrder::assign (Track* tr)
// {
//     track = tr;
//     current_event = 0;
//     current_time = 0.0;
//     previous_event = 0;
    
//     int lines_num = track->lines.size();

//     // initialize current_pos at 0
//     for (int i = 0; i < lines_num; i++)
//     {
// 	TrackOrderEntry toe;
// 	toe.type = track->lines[i].type;
// 	toe.c = track->lines[i].entries[0].c;
// 	toe.active = !track->lines[i].is_born ();
// 	toe.birth = false;
// 	toe.death = false;
// 	current_pos.push_back (toe);
//     }

//     // collect all events and order them
//     for (int i = 0; i < lines_num; i++)
// 	for (unsigned j = 0; j < track->lines[i].entries.size(); j++)
// 	{
// 	    TrackEvent te;
// 	    te.t = track->lines[i].entries[j].t;
// 	    te.line = i;
// 	    te.pos = j;
// 	    te.birth = (j == 0 && track->lines[i].is_born ());
// 	    te.death = (j == track->lines[i].entries.size() - 1 && track->lines[i].is_dead ());
// 	    events.push_back (te);
// 	}

//     // GG should sort in exactly same order as flips. (is that levels and coord?)
//     std::sort (events.begin(), events.end());
// }

TrackOrdering::TrackOrdering (FILE *f)
{
    unsigned events_size;
    fscanf (f, "#events %u\n\n\n", &events_size);

    tprints (SCOPE_TRACKING, "reading %d events\n", events_size);

    assert (events_size > 0);

    for (unsigned i = 0; i < events_size; i++)
    {
	FlippingEvent fe;
	char ts[3] = {'\0','\0','\0'};
	unsigned tps_size;
	fscanf(f, "event: %c%c\ntime: %lf\n#entries: %u\n\n",
	       &ts[0], &ts[1], &fe.time, &tps_size);
	fe.type = string2flip_event (ts);

	for (unsigned j = 0; j < tps_size; j++)
	{
	    TrackEntryPointer tep;
	    fscanf (f, "  line: %d __ idx: %d\n", &tep.line, &tep.idx);
	    fe.track_ptrs.push_back (tep);
	}

	events.push_back (fe);
	fscanf (f, "\n\n");
    }
}

void TrackOrdering::write (FILE *f)
{
    fprintf (f, "#events %zu\n\n\n", events.size ());

    

    for (unsigned i = 0; i < events.size(); i++)
    {
	fprintf(f, "event: %s\ntime: %.80lf\n#entries: %zu\n\n",
		flip_event2string (events[i].type), events[i].time,
		events[i].track_ptrs.size());
	for (unsigned j = 0; j < events[i].track_ptrs.size(); j++)
	    fprintf (f, "  line: %d __ idx: %d\n",
		     events[i].track_ptrs[j].line,
		     events[i].track_ptrs[j].idx);
	fprintf (f, "\n\n");
    }
}


void TrackOrdering::init (Track* t)
{
    track = t;
    current_event = 0;

    for (unsigned i = 0; i < track->lines.size(); i++)
	track_positions.push_back (0);

    previous_forward = false;
}

void TrackOrdering::update_positions (bool forward)
{
    if (forward == previous_forward)
	current_event += forward? 1 : -1;
	
    assert (current_event >= 0 && current_event < (int) events.size());

    std::vector<TrackEntryPointer>& ptrs = events[current_event].track_ptrs;
    for (unsigned i = 0; i < ptrs.size (); i++)
    {
	if (is_current_birth ())
	    track_positions[ptrs[i].line] = ptrs[i].idx;
	else
	    track_positions[ptrs[i].line] += forward? 1 : -1;// = ptrs[i].idx;

	assert (track_positions[ptrs[i].line] < (int) track->lines[ptrs[i].line].entries.size()
		&& track_positions[ptrs[i].line] >= 0
		);
    }

    previous_forward = forward;
}

bool TrackOrdering::seek (int new_event)
{
    if (new_event == current_event)
	return true;

    bool r = true;

    if (new_event < 0)
    {
	if (current_event == 0)
	    return false;

	new_event = 0;
	r = false;
    }

    if (new_event >= (int) events.size())
    {
	if (current_event == (int) events.size() - 1)
	    return -1;

	new_event = events.size() - 1;
	r = false;
    }

    bool forward = current_event < new_event;

    while (current_event != new_event)
	update_positions (forward);

    return r;
}

bool TrackOrdering::seek (bool forward = true)
{
    if (!forward && current_event <= 0)
	return false;

    if (forward && current_event >= (int) events.size()-1)
	return false;

    update_positions (forward);
    return true;
}

bool TrackOrdering::seek_bb (bool forward, BoundingBox bb)
{
    if (!forward && current_event <= 0)
	return false;

    if (forward && current_event >= (int) events.size()-1)
	return false;

    int i = 0;
    do
    {
	update_positions (forward);

	i++;
	oprints (SCOPE_SEEK, "SEEK_BB. Moved %d. cur_ev %d."
		 "bb((%lf,%lf),(%lf,%lf)). cbb ((%lf,%lf),(%lf,%lf))\n",
		 i, current_event, bb.a.x, bb.a.y, bb.b.x, bb.b.y,
		 current_bb ().a.x, current_bb ().a.y, current_bb ().b.x, current_bb ().b.y);
    }
    while (current_event > 0 &&
	   current_event < (int) events.size()-1 &&
	   bb.is_outside (current_bb ()) );

    return true;
}

bool TrackOrdering::seek_life_bb (bool forward, BoundingBox bb)
{
    if (!forward && current_event <= 0)
	return false;

    if (forward && current_event >= (int) events.size()-1)
	return false;

    int i = 0;
    do 
    {
	update_positions (forward);

	i++;
	oprints (SCOPE_SEEK, "SEEK_LIFE_BB. Moved %d. cur_ev %d."
		 "bb((%lf,%lf),(%lf,%lf)). cbb ((%lf,%lf),(%lf,%lf))\n",
		 i, current_event, bb.a.x, bb.a.y, bb.b.x, bb.b.y,
		 current_bb ().a.x, current_bb ().a.y, current_bb ().b.x, current_bb ().b.y);
    }
    while (current_event > 0 &&
	   current_event < (int) events.size()-1 &&
	   ! ( (is_current_death () || is_current_birth ()) && bb.is_inside (current_bb()) ) );

    return true;
}


double TrackOrdering::current_time ()
{
    return events[current_event].time;
}


FlippingEventType TrackOrdering::current_type ()
{
    return events[current_event].type;    
}


BoundingBox TrackOrdering::current_bb ()
{
    Point min = Point::highest ();
    Point max = Point::lowest ();

    std::vector<Point> ps = current_coords ();
    for (unsigned i = 0; i < ps.size (); i++)
    {
	if (ps[i] < min)
	    min = ps[i];
	if (ps[i] > max)
	    max = ps[i];
    }
    return BoundingBox (min, max);
}


bool TrackOrdering::is_current_death ()
{
    return current_type () == DEATH_1 || current_type () == DEATH_2;
}


bool TrackOrdering::is_current_birth ()
{
    return current_type () == BIRTH_1 || current_type () == BIRTH_2;
}

Point TrackOrdering::current_final ()
{
    std::vector<TrackEntryPointer>& ptrs = events[current_event].track_ptrs;
    for (unsigned i = 0; i < ptrs.size (); i++)
	if (track->lines[ptrs[i].line].is_dead (ptrs[i].idx))
	    return track->final_point (ptrs[i].line);

    eprintx (42, "%s", "No current point is dead.\n");
    return Point ();
}

Point TrackOrdering::current_start ()
{
    std::vector<TrackEntryPointer>& ptrs = events[current_event].track_ptrs;
    for (unsigned i = 0; i < ptrs.size (); i++)
	if (track->lines[ptrs[i].line].is_getting_born (ptrs[i].idx))
	    return track->start_point (ptrs[i].line);

    eprintx (42, "%s", "No current point is getting born.\n");
    return Point ();
}

std::vector<Point> TrackOrdering::current_coords ()
{
    std::vector<Point> ps;
    std::vector<TrackEntryPointer>& ptrs = events[current_event].track_ptrs;
    for (unsigned i = 0; i < ptrs.size (); i++)
	ps.push_back (point_coord (ptrs[i].line));
    return ps;
}

std::vector<CriticalType> TrackOrdering::current_types ()
{
    std::vector<CriticalType> ts;
    std::vector<TrackEntryPointer>& ptrs = events[current_event].track_ptrs;
    for (unsigned i = 0; i < ptrs.size (); i++)
	ts.push_back (point_type (ptrs[i].line));
    return ts;
}


bool TrackOrdering::is_point_active (int line)
{
    return
	track->lines[line].is_alive (track_positions[line]) && 
	(track->lines[line].is_original () ||
	 (track->lines[line].is_born () &&
	  track->lines[line].entries[0].t >= current_time ()));
}


Point TrackOrdering::point_coord (int line)
{
    Coord c = track->lines[line].entries[track_positions[line]].c;
    return Point ((double) c.x, (double) c.y);
}


CriticalType TrackOrdering::point_type (int line)
{
    return track->lines[line].get_type (track_positions[line]);
}


void track_writer (const char *filename, Track* track, TrackOrdering* order)
{
    FILE *f = fopen (filename, "w");

    if (f == NULL)
	eprintx (54, "Could not open file %s.\n", filename);

    track->write (f);
    order->write (f);

    fclose (f);
}

void track_reader (const char *filename, Track** track, TrackOrdering** order)
{
    FILE *f = fopen (filename, "r");

    if (f == NULL)
	eprintx (54, "Could not open file %s.\n", filename);

    *track = new Track (f);
    *order = new TrackOrdering (f);
    (*order)->init (*track);

    fclose (f);
}
