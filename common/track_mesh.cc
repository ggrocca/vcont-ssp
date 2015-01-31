#include "track_mesh.hh"

int TrackLine_M::get_entry (double t)
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

CriticalType TrackLine_M::get_type ()
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

CriticalType TrackLine_M::get_type (unsigned j)
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

bool TrackLine_M::is_dead ()
{
    if (entries.size() < 1)
    {
	eprintx (42, "%s", "Empty line.\n");
	return true;
    }

    return (entries.size() > 1 && entries.back().life != -1);
}

bool TrackLine_M::is_dead (unsigned j)
{
    if (j >= entries.size())
    {
	eprintx (42, "%s", "Out of bounds.\n");
	return true;
    }

    return (j >= 1 && entries[j].life != -1);
}

bool TrackLine_M::is_alive ()
{
    return !is_dead();
}

bool TrackLine_M::is_alive (unsigned j)
{
    return !is_dead(j);
}

bool TrackLine_M::is_original ()
{
    if (entries.size() < 1)
    {
	eprintx (42, "%s", "Empty line\n");
	return false;
    }
    return (entries[0].life == -1);
}

bool TrackLine_M::is_born ()
{
    return !is_original();
}

bool TrackLine_M::is_getting_born (unsigned j)
{
    return is_born () && j == 0;
}


Track_M::Track_M (FILE *f)
{
    int critical_points_num = 0;
    fscanf (f, "#lines: %d\n", &critical_points_num);
    fscanf (f, "tol: %lf\n", &time_of_life);
    fscanf (f, "size: %d\n\n\n", &size);

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
	TrackLine_M newline;
	newline.type = char2critical (ctype);
	newline.elixir = eli;
	newline.strength = str;
	newline.scale = sca;
	lines.push_back (newline);

	for (int j = 0; j < lines_num; j++)
	{
	    TrackEntry_M newentry;
	    fscanf (f, "v:%d ## mate:%d life:%d\ntime: %lf\n", 
		    &(newentry.v),
		    &(newentry.mate),
		    &(newentry.life),
		    &(newentry.t));
	    lines[i].entries.push_back (newentry);
	}
	fscanf (f, "\n\n");
    }
}

void Track_M::write (FILE *f)
{
    fprintf (f, "#lines: %zu\n", lines.size());
    fprintf (f, "tol: %.80lf\n", time_of_life);
    fprintf (f, "size: %d\n\n\n", size);

    for (unsigned i = 0; i < lines.size(); i++)
    {
	fprintf(f, "type: %c\n"
		"eli: %.80lf\nstr: %.80lf\nsca: %.80lf\n"
		"#entries: %zu\n\n",
		critical2char (lines[i].type),
		lines[i].elixir, lines[i].strength, lines[i].scale,
		lines[i].entries.size());

	for (unsigned j = 0; j < lines[i].entries.size(); j++)
	    fprintf (f, "v:%d ## mate:%d life:%d\ntime: %.80lf\n", 
		     lines[i].entries[j].v,
		     lines[i].entries[j].mate,
		     lines[i].entries[j].life,
		     lines[i].entries[j].t);
	fprintf (f, "\n\n");
    }
}

    

void Track_M::drink_elixir ()
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
void Track_M::get_strength (ScalarField* d)
{
    for (unsigned i = 0; i < lines.size(); i++)
    {
	lines[i].strength = 0.0;
	lines[i].scale = 0.0;
    }
}

void Track_M::get_relative_drop (ScalarField* d)
{
    for (unsigned i = 0; i < lines.size(); i++)
    {
	lines[i].strength = 0.0;
	lines[i].scale = 0.0;
    }   
}


int Track_M::get_entry (int i, double t)
{
    return lines[i].get_entry (t);
}

CriticalType Track_M::get_type (int i)
{
    return lines[i].get_type ();
}

CriticalType Track_M::get_type (int i, unsigned j)
{
    return lines[i].get_type (j);
}

bool Track_M::is_dead (int i)
{
    return lines[i].is_dead ();
}

bool Track_M::is_dead (int i, unsigned j)
{
    return lines[i].is_dead (j);
}

bool Track_M::is_alive (int i)
{
    return lines[i].is_alive ();
}

bool Track_M::is_alive (int i, unsigned j)
{
    return lines[i].is_alive (j);
}

bool Track_M::is_original (int i)
{
    return lines[i].is_original ();
}

bool Track_M::is_born (int i)
{
    return lines[i].is_born ();
}

bool Track_M::is_getting_born (int i, unsigned j)
{
    return lines[i].is_getting_born (j);
}


double Track_M::lifetime (int i)
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


int Track_M::get_other_death (int i)
{
    if (!is_dead (i))
	eprintx (42, "Critical %d not dead.\n", i);

    return lines[i].entries.back ().life;
}


int Track_M::get_other_birth (int i)
{
    if (!is_born (i))
	eprintx (42, "Critical %d not born.\n", i);

    return lines[i].entries[0].life;
}


double Track_M::death_time (int i)
{
    if (!is_dead (i))
	eprintx (42, "Critical %d not dead.\n", i);

    return lines[i].entries.back ().t;
}


double Track_M::birth_time (int i)
{
    if (!is_born (i))
	eprintx (42, "Critical %d not born.\n", i);

    return lines[i].entries[0].t;
}

CriticalType Track_M::original_type (int i)
{
    return lines[i].type;
}


double Track_M::lifetime_elixir (int i)
{
    return lines[i].elixir;
}


void track_writer_m (const char *filename, Track_M* track)
{
    FILE *f = fopen (filename, "w");
    if (f == NULL)
	eprintx (2, "Could not open file `%s'. %s\n", filename, strerror (errno));

    track->write (f);
    // order->write (f);

    fclose (f);
}

void track_reader_m (const char *filename, Track_M** track)
{
    FILE *f = fopen (filename, "r");
    if (f == NULL)
	eprintx (2, "Could not open file `%s'. %s\n", filename, strerror (errno));

    *track = new Track_M (f);
    // *order = new TrackOrdering (f);
    // (*order)->init (*track);

    fclose (f);
}
