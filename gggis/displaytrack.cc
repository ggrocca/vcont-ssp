#include "displaytrack.hh"
#include "scalespace.hh"

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


void TW_CALL tw_setScaleValue (const void *value, void* cd)
{
    DisplayTrack* dt = (DisplayTrack*) cd;
    dt->scale_value = *(const double *) value;
    dt->time_value = ScaleSpace::scale2time (dt->scale_value);
    glutPostRedisplay();
}

void TW_CALL tw_getScaleValue (void *value, void* cd)
{
    DisplayTrack* dt = (DisplayTrack*) cd;
    *(double *) value = dt->scale_value;
}

void TW_CALL tw_setTimeValue (const void *value, void* cd)
{
    DisplayTrack* dt = (DisplayTrack*) cd;
    dt->time_value = *(const double *) value;
    dt->scale_value = ScaleSpace::time2scale (dt->time_value);
    glutPostRedisplay();
}

void TW_CALL tw_getTimeValue (void *value, void* cd)
{
    DisplayTrack* dt = (DisplayTrack*) cd;
    *(double *) value = dt->time_value;
}

void TW_CALL tw_do_query(void* cd)
{ 
    DisplayTrack* dt = (DisplayTrack*) cd;

    dt->query (dt->time_value);
}


void TW_CALL tw_set_order_step (const void *value, void* cd)
{
    DisplayTrack* dt = (DisplayTrack*) cd;
    dt->order_step = *(const int *) value;
    dt->track_order->seek (dt->order_step);
    dt->order_time = dt->track_order->current_time ();
    dt->order_event = dt->track_order->current_event;
    glutPostRedisplay();
}

void TW_CALL tw_get_order_step (void *value, void* cd)
{
    DisplayTrack* dt = (DisplayTrack*) cd;
    *(int *) value = dt->order_step;
}

// void TW_CALL tw_do_order(void* cd)
// { 
//     DisplayTrack* dt = (DisplayTrack*) cd;

//     if (dt->track_order != NULL)
// 	delete dt->track_order;

//     dt->track_order = new TrackOrdering ();
//     dt->track_order->assign (dt->track);

//     fprintf (stderr, "ORDER %zu\n", dt->track_order->current_pos.size());
//     glutPostRedisplay();
// }


void TW_CALL tw_fw(void* cd)
{ 
    DisplayTrack* dt = (DisplayTrack*) cd;

    dt->track_order->seek (true);
    dt->order_time = dt->track_order->current_time ();
    dt->order_step = dt->track_order->current_event;

    glutPostRedisplay();
}

void TW_CALL tw_bw(void* cd)
{ 
    DisplayTrack* dt = (DisplayTrack*) cd;

    dt->track_order->seek (false);
    dt->order_time = dt->track_order->current_time ();
    dt->order_step = dt->track_order->current_event;

    glutPostRedisplay();
}

void TW_CALL tw_bb_fw(void* cd)
{ 
    DisplayTrack* dt = (DisplayTrack*) cd;

    dt->track_order->seek_bb (true, dt->bb);
    dt->order_time = dt->track_order->current_time ();
    dt->order_step = dt->track_order->current_event;

    glutPostRedisplay();
}

void TW_CALL tw_bb_bw(void* cd)
{ 
    DisplayTrack* dt = (DisplayTrack*) cd;

    dt->track_order->seek_bb (false, dt->bb);
    dt->order_time = dt->track_order->current_time ();
    dt->order_step = dt->track_order->current_event;

    glutPostRedisplay();
}

void TW_CALL tw_life_bb_fw(void* cd)
{ 
    DisplayTrack* dt = (DisplayTrack*) cd;

    dt->track_order->seek_life_bb (true, dt->bb);
    dt->order_time = dt->track_order->current_time ();
    dt->order_step = dt->track_order->current_event;

    glutPostRedisplay();
}

void TW_CALL tw_life_bb_bw(void* cd)
{ 
    DisplayTrack* dt = (DisplayTrack*) cd;

    dt->track_order->seek_life_bb (false, dt->bb);
    dt->order_time = dt->track_order->current_time ();
    dt->order_step = dt->track_order->current_event;

    glutPostRedisplay();
}



DisplayTrack::DisplayTrack (Plane* p, GeoMapping* m, int sidx, int pidx) : DisplayPlane (sidx, pidx)
{
    map = m;

    if (p->type != TRK)
	eprintx (6, "Abort, %s wrong type for me.\n", (ds_type2string(p->type)).c_str());

    track = (Track*) p->data[0];
    track_order = (TrackOrdering*) p->data[1];

    tprints (SCOPE_TRACKING, "HERE: %zu\n", track_order->events.size());

    draw_track = false;
    draw_query = false;
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
    scale_value = 0.0;
    time_value = 0.0;
    color_newborns = false;

    // track_order = new TrackOrdering ();
    draw_order = false;
    order_mult = 1.0;
    order_step = 0;

    TwAddVarRW (bar, "show tracking", TW_TYPE_BOOLCPP, &(draw_track), "");
    TwAddVarRW (bar, "track scale", TW_TYPE_DOUBLE,
	       &(track_scale), "min=0.001 max=100.00 step=0.001");
    TwAddVarRW (bar, "track life mult", TW_TYPE_DOUBLE, &(track_mult),
	       "min=0.0 max=100.00 step=0.05");
    TwAddVarRW (bar, "color newborns", TW_TYPE_BOOLCPP, &(color_newborns), "");

    TwAddSeparator (bar, 0, 0);

    TwAddVarRW (bar, "show query", TW_TYPE_BOOLCPP, &(draw_query), "");
    TwAddVarCB (bar, "query scale value", TW_TYPE_DOUBLE,
    	       tw_setScaleValue, tw_getScaleValue,
    	       this, "min=1.0000 max=4096.0000 step=1.0000");
    TwAddVarCB (bar, "query time value", TW_TYPE_DOUBLE,
    	       tw_setTimeValue, tw_getTimeValue,
    	       this, "min=0.0000 max=64.0000 step=0.25");
    TwAddButton (bar, "DO QUERY", tw_do_query, this, "");
    TwAddVarRW (bar, "query scale", TW_TYPE_DOUBLE,
	       &(query_scale), "min=0.001 max=100.00 step=0.001");
    TwAddVarRW (bar, "query life mult", TW_TYPE_DOUBLE, &(query_mult),
	       "min=0.0 max=100.00 step=0.05");
    TwAddVarRW (bar, "draw current pos", TW_TYPE_BOOLCPP, &(query_cur_pos), "");
    TwAddVarRW (bar, "draw death point", TW_TYPE_BOOLCPP, &(query_death), "");

    TwAddSeparator (bar, 0, 0);

    TwAddVarRW (bar, "show lines", TW_TYPE_BOOLCPP, &(draw_lines), "");
    TwAddVarRW (bar, "lines width", TW_TYPE_DOUBLE, &(lines_width),
	       "min=0.1 max=100.00 step=0.1");
    TwAddVarRW (bar, "lines size clip", TW_TYPE_UINT32, &(lines_size_clip),
	       "min=0 max=1000 step=1");
    TwAddVarRW (bar, "lines life clip", TW_TYPE_DOUBLE, &(lines_life_clip),
	       "min=0.0 max=100.00 step=0.1");
    TwAddVarRW (bar, "lines query", TW_TYPE_BOOLCPP, &(lines_query), "");

    TwAddSeparator (bar, 0, 0);

    // TwAddButton (bar, "DO ORDER", tw_do_order, this, "");
    TwAddVarRW (bar, "show order", TW_TYPE_BOOLCPP, &(draw_order), "");
    TwAddVarRW (bar, "order mult", TW_TYPE_DOUBLE, &(order_mult),
	       "min=0.0 max=100.00 step=0.05");
    TwAddVarCB (bar, "order step", TW_TYPE_INT32,
    	       tw_set_order_step, tw_get_order_step,
    	       this, "step=1");
    TwAddVarRO (bar, "step time", TW_TYPE_DOUBLE, &(order_time), "");
    TwAddVarRO (bar, "step event", TW_TYPE_INT32, &(order_event), "");
    TwAddButton (bar, "seek-fw", tw_fw, this, "");
    TwAddButton (bar, "seek-bw", tw_bw, this, "");
    TwAddButton (bar, "seek-bb-fw", tw_bb_fw, this, "");
    TwAddButton (bar, "seek-bb-bw", tw_bb_bw, this, "");
    TwAddButton (bar, "seek-life-fw", tw_life_bb_fw, this, "");
    TwAddButton (bar, "seek-life-bw", tw_life_bb_bw, this, "");

    order = 10;
};

DisplayTrack::~DisplayTrack () {}

void DisplayTrack::query (double t)
{
    track->query (t, vquery);
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

void __draw_critical_newborn (bool is_new)
{
    double qs = 10.0;
    double qh = (qs / 2.0);
    double st = 1.0;

    if (is_new)
	glColor3dv (yellow);
    else
	glColor3dv (cyan);

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
			    double scale, double tol_mult, bool color_newborns)
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

    if (color_newborns)
	__draw_critical_newborn (special);
    else
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

void __draw_order (Point d, CriticalType type, double mult)
{
    glPushMatrix ();
    glTranslated ((double) d.x, (double) d.y, 0.0);
    glScaled (mult, mult, 1.0);
    __draw_critical_color (type);
    glPopMatrix();
}

void __draw_order (Point d, bool birth, double mult)
{
    glPushMatrix ();
    glTranslated (d.x, d.y, 0.0);
    glScaled (mult, mult, 1.0);
    __draw_death (birth);
    glPopMatrix();
}


void DisplayTrack::display ()
{
    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();

    // GG transform viewport in bb using gluunproject, facendo la viewport un po' piu' stretta
    GLint m_viewport[4];
    glGetIntegerv (GL_VIEWPORT, m_viewport);
    GLdouble matrix_m[16]; 
    glGetDoublev (GL_MODELVIEW_MATRIX, matrix_m);
    GLdouble matrix_p[16]; 
    glGetDoublev (GL_PROJECTION_MATRIX, matrix_p);
    double foo;

    gluUnProject (viewport.a.x, viewport.a.y, 0.0,
		  matrix_m, matrix_p, m_viewport,
		  &bb.a.x, &bb.a.y, &foo);
    gluUnProject (viewport.b.x, viewport.b.y, 0.0,
		  matrix_m, matrix_p, m_viewport,
		  &bb.b.x, &bb.b.y, &foo);

    if (draw_track)
    {
	for (unsigned i = 0; i < track->lines.size(); i++)
	{
	    CriticalType t = track->lines[i].type;
	    __draw_critical_track (track->lines[i].entries[0].c, t, true,
				   track->lines[i].is_born(),
				   track->lifetime(i),
				   track_scale, track_mult, color_newborns);
	    if (!color_newborns)
		__draw_critical_track (track->lines[i].entries.back().c, t, false,
				       track->lines[i].is_dead(),
				       track->lifetime(i),
				       track_scale, track_mult, color_newborns);
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

	    if (track->lifetime(i) < lines_life_clip)
		continue;

	    if (lines_query && !track->lines[i].mark)
		continue;

	    glBegin (GL_LINES);
	    for (unsigned j = 1; j < track->lines[i].entries.size(); j++)
	    {
		TrackEntry pte = track->lines[i].entries[j-1];
		TrackEntry te = track->lines[i].entries[j];
		double color[3];
		time2color (pte.t, 0.0, track->time_of_life, color);
		glColor3dv (color);
		glVertex2d (pte.c.x, pte.c.y);
		time2color (te.t, 0.0, track->time_of_life, color);
		glColor3dv (color);
		glVertex2d (te.c.x, te.c.y);
		// draw last stretch to die point
		if (j == track->lines[i].entries.size() - 1)
		{
		    glVertex2d (te.c.x, te.c.y);
		    Point p = track->final_point (i);
		    glVertex2d (p.x, p.y); 
		}
	    }
	    glEnd();
	}
    }

    if (track_order != NULL && draw_order)
    {
	for (unsigned i = 0; i < track_order->track_positions.size(); i++)
	    if (track_order->is_point_active(i))
		__draw_order (track_order->point_coord (i), track_order->point_type (i), order_mult);
	
	if (track_order->is_current_death () || track_order->is_current_birth ())
	{
	    vector<Point> cs = track_order->current_coords ();
	    vector<CriticalType> ts = track_order->current_types ();

	    for (unsigned i = 0; i < cs.size(); i++)
		__draw_order (cs[i], ts[i], order_mult);

	    if (track_order->is_current_death ())
		__draw_order (track_order->current_final (), false, order_mult);
	    if (track_order->is_current_birth ())
		__draw_order (track_order->current_start (), true, order_mult);
	}
    }

 	    // __draw_order (track_order->current_pos[i], order_mult);

    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix ();
}

