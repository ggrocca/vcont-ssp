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
// void final_point (std::vector<critical_line_t>& critical_points,
// 		  double* fx, double* fy)
// {
//     double x =  line.back().x;
//     double y =  line.back().y;
//     if (is_alive())
//     {
// 	*fx = x;
// 	*fy = y;
//     }
//     else
//     {
// 	int life = line.back().life;
// 	// if (life == -1)
// 	// 	fprintf (stderr, "AIUTO: %d.life = %d", i, life);
// 	double lx = critical_points[life].line.back().x;
// 	double ly = critical_points[life].line.back().y;
// 	*fx = x + ((lx - x)/2.0);
// 	*fy = y + ((ly - y)/2.0);
//     }
// }


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
Track::Track (char *filename)
{
    FILE *f = fopen (filename, "r");

    int critical_points_num = 0;
    fscanf (f, "%d\n", &critical_points_num);
    fscanf (f, "%lf\n", &time_of_life);

    printf ("Tracking::Tracking(): reading %d criticals, lifetime %lf\n",
	    critical_points_num, time_of_life);

    for (int i = 0; i < critical_points_num; i++)
    {
	char ctype;
	int lines_num = 0;
	fscanf (f, "%c %d\n", &ctype, &lines_num);
		// critical2char (critical_points[i].type), 
		// critical_points[i].line.size());
	TrackLine newline;
	newline.type = char2critical (ctype);
	lines.push_back (newline);

	for (int j = 0; j < lines_num; j++)
	{
	    TrackEntry newentry;
	    fscanf (f, "%d %d ## %d %d\n%lf\n", 
		    &(newentry.c.x),
		    &(newentry.c.y),
		    &(newentry.mate),
		    &(newentry.life),
		    &(newentry.t));
	    lines[i].entries.push_back (newentry);
	}
    }
}

void Track::write (char *filename)
{
    FILE *f = fopen (filename, "w");

    fprintf (f, "%zu\n", lines.size());
    fprintf (f, "%.80lf\n", time_of_life);

    for (unsigned i = 0; i < lines.size(); i++)
    {
	fprintf(f, "%c %zu\n",
		critical2char (lines[i].type), 
		lines[i].entries.size());

	for (unsigned j = 0; j < lines[i].entries.size(); j++)
	    fprintf (f, "%d %d ## %d %d\n%.80lf\n", 
		     lines[i].entries[j].c.x,
		     lines[i].entries[j].c.y,
		     lines[i].entries[j].mate,
		     lines[i].entries[j].life,
		     lines[i].entries[j].t);
    }
}

// void Track::query (double t, std::vector<critical_rend_t>& v, bool verbose = true)
// {
//     critical_rend_t r;

//     v.resize (0);
//     for (unsigned i = 0; i < critical_points.size(); i++)
//     {
// 	critical_points[i].mark = false;
// 	int j = critical_points[i].get_entry (t);
// 	if (j != -1)
// 	{
// 	    r.type = critical_points[i].get_type(j);
// 	    r.tx = critical_points[i].line[j].x;
// 	    r.ty = critical_points[i].line[j].y;
// 	    r.ox = critical_points[i].line[0].x;
// 	    r.oy = critical_points[i].line[0].y;
// 	    r.idx = j;
// 	    r.tol = critical_points[i].lifetime();
// 	    r.tob = critical_points[i].line[0].t;
// 	    r.is_born = critical_points[i].is_born ();
// 	    r.is_alive = critical_points[i].is_alive ();
// 	    critical_points[i].final_point(critical_points, &(r.fx), &(r.fy));

// 	    v.push_back (r);
// 	    critical_points[i].mark = true;
// 	}
//     }
//     if (verbose)
// 	printf ("!!! Tracking::query(): found %zu criticals\n", v.size());
// }
