#include "trackdisplay.hh"

double white[3] =   {1.0,1.0,1.0};
double black[3] =   {0.0,0.0,0.0};
double red[3] =     {1.0,0.0,0.0};
double green[3] =   {0.0,1.0,0.0};
double blue[3] =    {0.0,0.0,1.0};
double yellow[3] =  {1.0,1.0,0.0};
double cyan[3] =    {0.0,1.0,1.0};
double magenta[3] = {1.0,0.0,1.0};

// Scale colore se ne trovano tante in giro, una che di solito
// funziona bene, su un intervallo [0,1] e':
// - R a zero fino a 0.5 e poi a salire lineare fino a 1 da li' in poi
// - B esattamente l'opporto (scende fino a 0.5 poi resta zero)
// - G parte da 0, sale fino a 1 a 0.5 e poi ridiscende fino a 0

void time2color (double t, double tmin, double tmax, double color[])
{
    double st = (t - tmin) / tmax;

    color[0] = st < 0.5? 0.0 : (st - 0.5) * 2.0;
    color[1] = st < 0.5? st * 2.0 : 1.0 - ((st - 0.5) * 2.0);
    color[2] = st < 0.5? (0.5 - st) * 2.0 : 0.0;
    
    // printf ("@ st: %.2lf @ color: {%.2lf , %.2lf , %.2lf}\n",
    // 	    st, color[0], color[1], color[2]);
}

TrackDisplay::TrackDisplay()
{
    // draw_time_labels = false;
    draw_terrain = true;
    draw_track = false;
    draw_query = false;
    clip_black = 0.0;
    clip_white = 65535.0;
    query_scale = 0.1;
    query_mult = 1.0;
    query_cur_pos = true;
    query_death = true;
    track_scale = 0.1;
    track_mult = 1.0;
    vquery = std::vector<TrackRenderingEntry>(0);
    draw_lines = false;
    lines_width = 1.0;
    lines_size_clip = 0;
    lines_life_clip = 0.0;
    lines_query = false;
};


void TrackDisplay::read_dem (char *file)
{
    dems.push_back(DEMSelector::get (file));
}

void TrackDisplay::read_track (char *file)
{
    track = new Track (file);
}

void TrackDisplay::query (double t)
{
    track->query (t, vquery);
}


void TrackDisplay::getbb (double* cx, double* cy, double* diam)
{
    *cx = dems[0]->width / 2.0;
    *cy = dems[0]->height / 2.0;
    *diam = dems[0]->width * 1.2;
}

double __map (double v, double min, double max)
{
    v = v < min? min : v;
    v = v > max? max : v;

    return (v - min) / (max - min);
}

void __draw_critical_color (CriticalType t)
{
    double qs = 10.0;
    double qh = (qs / 2.0);
    double st = 1.0;

    switch (t)
    {
    case MIN:
	glColor3dv (blue);
 	break;

    case MAX:
	glColor3dv(red);
	break;

    case SA2:
	glColor3dv(green);
	break;

    case SA3:
	glColor3dv(yellow);
	break;
	
    default:
	fprintf (stderr, "__draw_critical_color() problem\n");
    }

    glBegin (GL_QUADS);
    glVertex2f (-qh, -qh);
    glVertex2f (-qh,  qh);
    glVertex2f ( qh,  qh);
    glVertex2f ( qh, -qh);
    glEnd ();

    glColor3f (0.0, 0.0, 0.0);

    glBegin (GL_QUADS);
    // 1 left
    glVertex2f (-qh     , -qh);
    glVertex2f (-qh     ,  qh);
    glVertex2f (-qh + st,  qh);
    glVertex2f (-qh + st, -qh);
    // 2 top
    glVertex2f (-qh,  qh - st);
    glVertex2f (-qh,  qh     );
    glVertex2f ( qh,  qh     );
    glVertex2f ( qh,  qh - st);
    // 3 right
    glVertex2f ( qh - st, -qh);
    glVertex2f ( qh - st,  qh);
    glVertex2f ( qh     ,  qh);
    glVertex2f ( qh     , -qh);
    // 4 bottom
    glVertex2f (-qh, -qh);
    glVertex2f (-qh, -qh + st);
    glVertex2f ( qh, -qh + st);
    glVertex2f ( qh, -qh);
    glEnd ();
 }

void __draw_critical_sym (CriticalType t)
{
    double qs = 10.0;
    double qh = (qs / 2.0);

    double ss = 8.0;
    double sg = 2.0;
    double sd = 1.5;
    double st = 1.0;
    double sh = (ss / 2.0);

    glBegin (GL_QUADS);
    glVertex2f (-qh, -qh);
    glVertex2f (-qh,  qh);
    glVertex2f ( qh,  qh);
    glVertex2f ( qh, -qh);
    glEnd ();

    glColor3f (0.0, 0.0, 0.0);

    // char sym[5][5];

    switch (t)
    {
    case MIN:
	glBegin (GL_QUADS);
	// 1 left
	glVertex2f (-sh     , -sh);
	glVertex2f (-sh     ,  sh);
	glVertex2f (-sh + sg,  sh);
	glVertex2f (-sh + sg, -sh);
	// 2 top
	glVertex2f (-sh,  sh - sg);
	glVertex2f (-sh,  sh     );
	glVertex2f ( sh,  sh     );
	glVertex2f ( sh,  sh - sg);
	// 3 right
	glVertex2f ( sh - sg, -sh);
	glVertex2f ( sh - sg,  sh);
	glVertex2f ( sh     ,  sh);
	glVertex2f ( sh     , -sh);
	// 4 bottom
	glVertex2f (-sh, -sh);
	glVertex2f (-sh, -sh + sg);
	glVertex2f ( sh, -sh + sg);
	glVertex2f ( sh, -sh);
	glEnd ();
 	break;

    case MAX:
	glBegin (GL_QUADS);
	glVertex2f (-sh+st, -sh+st);
	glVertex2f (-sh+st,  sh-st);
	glVertex2f ( sh-st,  sh-st);
	glVertex2f ( sh-st, -sh+st);
	glEnd ();
	break;

    case SA3:
	glBegin (GL_QUADS);
	glVertex2f (-sh, -st);
	glVertex2f (-sh,  st);
	glVertex2f ( sh,  st);
	glVertex2f ( sh, -st);
	glEnd ();	
	glBegin (GL_QUADS);
	glVertex2f (-sh+sd, -sh);
	glVertex2f (-sh   , -sh+sd);
	glVertex2f ( sh-sd,  sh);
	glVertex2f ( sh   ,  sh-sd);
	glEnd ();	
	glBegin (GL_QUADS);
	glVertex2f (-sh+sd,  sh);
	glVertex2f (-sh   ,  sh-sd);
	glVertex2f ( sh-sd, -sh);
	glVertex2f ( sh   , -sh+sd);
	glEnd ();
	break;
	
    case SA2:
	glBegin (GL_QUADS);
	glVertex2f (-sh+sg, -sh);
	glVertex2f (-sh   , -sh+sg);
	glVertex2f ( sh-sg,  sh);
	glVertex2f ( sh   ,  sh-sg);
	glEnd ();	
	glBegin (GL_QUADS);
	glVertex2f (-sh+sg,  sh);
	glVertex2f (-sh   ,  sh-sg);
	glVertex2f ( sh-sg, -sh);
	glVertex2f ( sh   , -sh+sg);
	glEnd ();	
	break;

    default:
	fprintf (stderr, "__draw_critical_sym() problem\n");
    }
}

void __draw_death (bool is_alive)
{
    double qs = 10.0;
    double qh = (qs / 2.0);

    double ss = 8.0;
    double sg = 2.0;
    // double sd = 1.5;
    // double st = 1.0;
    double sh = (ss / 2.0);

    glColor3dv (is_alive? cyan : magenta);

    glBegin (GL_QUADS);
    glVertex2f (-qh, -qh);
    glVertex2f (-qh,  qh);
    glVertex2f ( qh,  qh);
    glVertex2f ( qh, -qh);
    glEnd ();

    glColor3dv (black);

    glBegin (GL_QUADS);
    glVertex2f (-sh+sg, -sh);
    glVertex2f (-sh   , -sh+sg);
    glVertex2f ( sh-sg,  sh);
    glVertex2f ( sh   ,  sh-sg);
    glEnd ();	
    glBegin (GL_QUADS);
    glVertex2f (-sh+sg,  sh);
    glVertex2f (-sh   ,  sh-sg);
    glVertex2f ( sh-sg, -sh);
    glVertex2f ( sh   , -sh+sg);
    glEnd ();	

}

void __draw_critical_simple (Coord c, CriticalType t, bool dead, double scale)
{
    if (dead)
	glColor3f (1.0, 0.0, 0.0);
    else
	glColor3f (0.0, 1.0, 0.0);

    glPushMatrix ();

    glTranslated ((double) c.x, (double) c.y, 0.0);
    glScaled (scale, scale, 1.0);

    __draw_critical_sym (t);

    glPopMatrix();
}

void __draw_critical_track (Coord c, CriticalType t,
			    bool start, bool special, double tol,
			    double scale, double tol_mult)
{
    double scale_tol = tol_mult * (1.0 + tol);

    // initial position original
    if (start && !special)
	glColor3dv (green);
    // initial position born
    else if (start && special)
	glColor3dv (yellow);
    // final position alive
    else if (!start && !special)
	glColor3dv (cyan);
    // final position dead
    else if (!start && special)
	glColor3dv (blue);

    glPushMatrix ();

    glTranslated ((double) c.x, (double) c.y, 0.0);
    glScaled (scale, scale, 1.0);

    if (scale_tol != 0.0)
	glScaled (scale_tol, scale_tol, 1.0);

    __draw_critical_sym (t);

    glPopMatrix();
}

void __draw_critical_query (TrackRenderingEntry r,  double scale, double tol_mult,
			    bool cur_pos, bool draw_death)
{
    double x = r.oc.x;
    double y = r.oc.y;

    if (cur_pos)
    {
	x = r.tc.x;
	y = r.tc.y;
    }

    double scale_tol = tol_mult * (1.0 + r.tol);

    glPushMatrix ();
    glTranslated ((double) x, (double) y, 0.0);
    glScaled (scale, scale, 1.0);
    if (scale_tol != 0.0)
	glScaled (scale_tol, scale_tol, 1.0);
    __draw_critical_color (r.type);
    glPopMatrix();

    if (draw_death)
    {
	glPushMatrix ();
	glTranslated ((double) r.fx, (double) r.fy, 0.0);
	glScaled (scale, scale, 1.0);
	if (scale_tol != 0.0)
	    glScaled (scale_tol, scale_tol, 1.0);
	__draw_death (r.is_alive);
	glPopMatrix();
    }
}


void TrackDisplay::draw (int dem_idx)
{
    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();

    if (draw_terrain)
    {
	DEMReader* dem = dems[dem_idx];
	double min = clip_black;
	double max = clip_white;

	glBegin (GL_TRIANGLES);
	for (int i = 0; i < dem->width - 1; i++)
	    for (int j = 0; j < dem->height - 1; j++)
	    {
		double vij = dem->get_pixel (i, j);
		double vipj = dem->get_pixel (i+1, j);
		double vijp = dem->get_pixel (i, j+1);
		double vipjp = dem->get_pixel (i+1, j+1);
		vij = __map (vij, min, max);
		vipj = __map (vipj, min, max);
		vijp = __map (vijp, min, max);
		vipjp = __map (vipjp, min, max);

		glColor3f (vij, vij, vij);
		glVertex2f (i, j);
		glColor3f (vipj, vipj, vipj);
		glVertex2f (i+1, j);
		glColor3f (vipjp, vipjp, vipjp);
		glVertex2f (i+1, j+1);

		glColor3f (vij, vij, vij);
		glVertex2f (i, j);
		glColor3f (vijp, vijp, vijp);
		glVertex2f (i, j+1);
		glColor3f (vipjp, vipjp, vipjp);
		glVertex2f (i+1, j+1);
	    }
	glEnd();
    }

    if (draw_track)
    {
	for (unsigned i = 0; i < track->lines.size(); i++)
	{
	    CriticalType t = track->lines[i].type;
	    __draw_critical_track (track->lines[i].entries[0].c, t, true,
				   track->lines[i].is_born(),
				   track->lines[i].lifetime(),
				   track_scale, track_mult);
	    __draw_critical_track (track->lines[i].entries.back().c, t, false,
				   track->lines[i].is_dead(),
				   track->lines[i].lifetime(),
				   track_scale, track_mult);
	}
    }

    if (draw_query)
    {
	for (unsigned i = 0; i < vquery.size(); i++)
	    __draw_critical_query (vquery[i], query_scale, query_mult,
				   query_cur_pos, query_death);
    }

    if (draw_lines)
    {
	glEnable(GL_LINE_SMOOTH);
 	glLineWidth (lines_width);

	for (unsigned i = 0; i < track->lines.size(); i++)
	{
	    if (track->lines[i].entries.size() < lines_size_clip)
		continue;

	    if (track->lines[i].lifetime() < lines_life_clip)
		continue;

	    if (lines_query && !track->lines[i].mark)
		continue;

	    glBegin (GL_LINES);
	    for (unsigned j = 1; j < track->lines[i].entries.size(); j++)
	    {
		TrackEntry pte = track->lines[i].entries[j-1];
		TrackEntry te = track->lines[i].entries[j];
		double color[3];
		time2color (pte.t, 0.0, Track::time_of_life, color);
		glColor3dv (color);
		glVertex2d (pte.c.x, pte.c.y);
		time2color (te.t, 0.0, Track::time_of_life, color);
		glColor3dv (color);
		glVertex2d (te.c.x, te.c.y);
		// draw last stretch to die point
		if (j == track->lines[i].entries.size() - 1)
		{
		    glVertex2d (te.c.x, te.c.y);
		    double fx, fy;
		    track->lines[i].final_point (track->lines, &fx, &fy);
		    glVertex2d (fx, fy); 
		}
	    }
	    glEnd();
	}
    }

    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix ();
}

void TrackDisplay::draw_line_test (double d)
{
    double color[3];
    glLineWidth (lines_width);

    d /= 1.2;
    glEnable(GL_LINE_SMOOTH);
    glBegin (GL_LINES);
    // glBegin (GL_POINTS);
    for (int i = 0; i < 100; i++)
    {
	double ti = Track::time_of_life/100.0 * ((double)i);
	time2color (ti, 0.0, Track::time_of_life, color);
	glColor3dv (color);
	glVertex2d (d/100.0 * ((double)i), d/100.0 * ((double)i));
	
	double tip1 = Track::time_of_life/100.0 * ((double)i+1.0);
	time2color (tip1, 0.0, Track::time_of_life, color);
	glColor3dv (color);
	glVertex2d (d/100.0 * ((double)i+1.0), d/100.0 * ((double)i+1.0));
	// printf ("$$$ %d: %lf %lf -- %lf\n", i, ti, tip1, Track::time_of_life);
    }
    glEnd();
    //   exit (0);
}
