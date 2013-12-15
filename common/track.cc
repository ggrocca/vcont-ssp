#include "track.hh"

#include <math.h>

// returns index to first alive entry occurring before t.
// -1 if not found
int TrackLine::get_entry (double t)
{
    double pt = 0.0;
    double nt = 0.0;

    for (unsigned j = 0; j < entries.size(); j++)
    {
	nt = entries[j].t;

	if (t >= pt && t <= nt)
	    return is_alive (j)? j : -1;

	pt = nt;
    }

    return -1;
}

CriticalType TrackLine::get_type ()
{
    if (entries.size() < 1)
    {
	eprint ("%s", "Empty line.\n");
	return EQU;
    }
    if (type == MAX || type == MIN)
	return type;

    if (type == SA2 || type == SA3)
	return (entries.back().mate == -1)? SA2 : SA3;

    eprint ("%s", "Returning wrong type.\n");
    return EQU;
}

CriticalType TrackLine::get_type (unsigned j)
{
    if (j >= entries.size())
    {
	eprint ("%s", "Out of bounds.\n");
	return EQU;
    }
    if (type == MAX || type == MIN)
	return type;

    if (type == SA2 || type == SA3)
	return (entries[j].mate == -1)? SA2 : SA3;

    eprint ("%s", "Returning wrong type.\n");
    return EQU;
}

bool TrackLine::is_dead ()
{
    if (entries.size() < 1)
    {
	eprint ("%s", "Empty line.\n");
	return true;
    }

    return (entries.size() > 1 && entries.back().life != -1);
}

bool TrackLine::is_dead (unsigned j)
{
    if (j >= entries.size())
    {
	eprint ("%s", "Out of bounds.\n");
	return true;
    }

    return (j > 1 && entries[j].life != -1);
}

bool TrackLine::is_alive () { return !is_dead(); }

bool TrackLine::is_alive (unsigned j) { return !is_dead(j); }

// bool TrackLine::is_active ()
// {
//     if (entries.size() < 1)
//     {
// 	eprint ("%s", "Empty line.\n");
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
	eprint ("%s", "Empty line\n");
	return false;
    }
    return (entries[0].life == -1);
}

bool TrackLine::is_born () { return !is_original(); }

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
	    eprint ("%s", "Wrong travel\n");
    }
    return t;
}

double TrackLine::lifetime ()
{
    if (is_dead())
	return entries.back().t - entries[0].t; 
    if (is_alive() && is_original())
	return Track::time_of_life;
    if (is_alive() && is_born())
	return Track::time_of_life - entries[0].t;
    eprint ("%s", "Time travel.\n");
    return 0.0;
}

// GG should be implemented in Track
void TrackLine::final_point (std::vector<TrackLine>& lines,
		  double* fx, double* fy)
{
    double x =  entries.back().c.x;
    double y =  entries.back().c.y;

    if (is_alive())
    {
	*fx = x;
	*fy = y;
    }

    else
    {
	int life = entries.back().life;
	double lx = lines[life].entries.back().c.x;
	double ly = lines[life].entries.back().c.y;
	*fx = x + ((lx - x)/2.0);
	*fy = y + ((ly - y)/2.0);
    }
}
void TrackLine::start_point (std::vector<TrackLine>& lines, Point* d)
{
    double x =  entries[0].c.x;
    double y =  entries[0].c.y;

    if (!is_born ())
    {
	d->x = x;
	d->y = y;
    }
    else
    {
	int life = entries[0].life;
	double lx = lines[life].entries[0].c.x;
	double ly = lines[life].entries[0].c.y;
	d->x = x + ((lx - x)/2.0);
	d->y = y + ((ly - y)/2.0);
    }
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

double Track::time_of_life;

Track::Track () {}
Track::~Track () {}
// Track (ScaleSpace& ss); // should be implemented as flipper algorithm
Track::Track (const char *filename)
{
    FILE *f = fopen (filename, "r");

    if (f == NULL)
	eprintx (54, "Could not open file %s.\n", filename);

    int critical_points_num = 0;
    fscanf (f, "#lines: %d\n", &critical_points_num);
    fscanf (f, "tol: %lf\n\n\n", &time_of_life);

    printf ("Tracking::Tracking(): reading %d criticals, lifetime %lf\n",
	    critical_points_num, time_of_life);

    for (int i = 0; i < critical_points_num; i++)
    {
	char ctype;
	int lines_num = 0;
	fscanf (f, "type: %c\n#entries: %d\n\n", &ctype, &lines_num);
		// critical2char (critical_points[i].type), 
		// critical_points[i].line.size());
	TrackLine newline;
	newline.type = char2critical (ctype);
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

void Track::write (char *filename)
{
    FILE *f = fopen (filename, "w");

    if (f == NULL)
	eprintx (54, "Could not open file %s.\n", filename);

    fprintf (f, "#lines: %zu\n", lines.size());
    fprintf (f, "tol: %.80lf\n\n\n", time_of_life);

    for (unsigned i = 0; i < lines.size(); i++)
    {
	fprintf(f, "type: %c\n#entries: %zu\n\n",
		critical2char (lines[i].type), 
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
	    r.tol = lines[i].lifetime();
	    r.tob = lines[i].entries[0].t;
	    r.is_born = lines[i].is_born ();
	    r.is_alive = lines[i].is_alive ();
	    lines[i].final_point(lines, &(r.fx), &(r.fy));

	    v.push_back (r);
	    lines[i].mark = true;
	}
    }
    if (verbose)
	tprintp ("!!!", "Found %zu criticals\n", v.size());
}

void TrackOrder::assign (Track* tr)
{
    track = tr;
    current_event = 0;
    current_time = 0.0;
    previous_event = 0;
    
    int lines_num = track->lines.size();

    // initialize current_pos at 0
    for (int i = 0; i < lines_num; i++)
    {
	TrackOrderEntry toe;
	toe.type = track->lines[i].type;
	toe.c = track->lines[i].entries[0].c;
	toe.active = !track->lines[i].is_born ();
	toe.birth = false;
	toe.death = false;
	current_pos.push_back (toe);
    }

    // collect all events and order them
    for (int i = 0; i < lines_num; i++)
	for (unsigned j = 0; j < track->lines[i].entries.size(); j++)
	{
	    TrackEvent te;
	    te.t = track->lines[i].entries[j].t;
	    te.line = i;
	    te.pos = j;
	    te.birth = (j == 0 && track->lines[i].is_born ());
	    te.death = (j == track->lines[i].entries.size() - 1 && track->lines[i].is_dead ());
	    events.push_back (te);
	}

    // GG should sort in exactly same order as flips. (is that levels and coord?)
    std::sort (events.begin(), events.end());
}

// returns true if it has handled life event.
bool TrackOrder::update ()
{
    TrackEvent te = events[current_event];
    TrackEvent lte = events[previous_event];
    current_time = te.t;
	
    TrackLine* tl = &(track->lines[te.line]);
    current_pos[te.line].type = tl->get_type (te.pos);
    current_pos[te.line].c = tl->entries[te.pos].c;
    current_pos[te.line].idx = te.pos;

    current_pos[te.line].active = tl->is_alive (te.pos) || te.birth || te.death;
    current_pos[te.line].birth = te.birth;
    current_pos[te.line].death = te.death;

    if (current_pos[te.line].birth)
	tl->start_point (track->lines, &(current_pos[te.line].d));
    if (current_pos[te.line].death)
	tl->final_point (track->lines, &(current_pos[te.line].d.x), &(current_pos[te.line].d.y));

    // update last event birth, active and death
    if (previous_event - current_event == 1) // backward
    {
	if (current_pos[lte.line].birth)
	    current_pos[lte.line].active = false;
    }
    else if (previous_event - current_event == -1) // forward 
    {
	if (current_pos[lte.line].death)
	    current_pos[lte.line].active = false;
    }
    else if (previous_event != current_event)
	eprintx (42, "%s", "Something wrong with previous and current event\n");

    current_pos[lte.line].birth = false;
    current_pos[lte.line].death = false;

    previous_event = current_event;
    last_event_c = current_pos[te.line].c;

    return te.birth || te.death;
}

bool TrackOrder::seek (int new_event)
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

    do 
    {
	update ();
	current_event += forward? 1 : -1;
    }
    while (current_event != new_event);

    return r;
}

bool TrackOrder::seek (bool forward = true)
{
    return seek (forward? current_event+1 : current_event-1);
}

bool TrackOrder::seek_bb (bool forward, BoundingBox bb)
{
    if (forward && current_event <= 0)
	return false;

    if (!forward && current_event >= (int) events.size()-1)
	return false;

    int i = 0;
    do 
    {
	i++;
	update ();
	current_event += forward? 1 : -1;

	oprints (SCOPE_SEEK, "SEEKBB. Moved %d. cur_ev %d."
		 "bb((%lf,%lf),(%lf,%lf)). lc(%d,%d)\n",
		 i, current_event, bb.a.x, bb.a.y, bb.b.x, bb.b.y,
		 last_event_c.x, last_event_c.y);
    }
    while (current_event >= 0 &&
	   current_event < (int) events.size()
	   &&
	   bb.is_outside (last_event_c)
	   );


    return true;
}

bool TrackOrder::seek_life_bb (bool forward, BoundingBox bb)
{
    if (forward && current_event <= 0)
	return false;

    if (!forward && current_event >= (int) events.size()-1)
	return false;

    bool life;
    do 
    {
	life = update ();
	current_event += forward? 1 : -1;
    }
    while (current_event >= 0 &&
	   current_event < (int) events.size() &&
	   // bb.is_outside (last_event_c) &&
	   !life
	   );
    return true;
}
