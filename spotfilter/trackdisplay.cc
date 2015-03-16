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
    draw_csv = false;
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
    lines_width = 2.0;
    lines_size_clip = 0;
    lines_life_clip = 0.0;
    lines_query = false;

    draw_elixir = false;
    elixir_mult = 1.0;
    elixir_scale = 0.5;
    elixir_cut = 0.0;
    importance_mult = 0.05;
    importance_scale = 0.3;
    importance_cut = 0.0;
    normal_lives = false;

    //density_maxima_num = 0;
    draw_spots = true;
    multiply_elix_spots = false;
    draw_final = true;
    draw_always_selected = false;
    draw_density_selected = false;
    draw_density_pool = false;
    
    spot_scale = 0.2;
    density_maxima_val = 300.0;
    // spots_maxima_life_cut = 0;
    // spots_minima_life_cut = 0;
    // spots_sellae_life_cut = 0;
    // spots_maxima_life_add = 0;
    // spots_minima_life_add = 0;
    // spots_sellae_life_add = 0;

    maxima_always_selected_num = 12;
    maxima_density_pool_num = 60;
    spots_maxima_imp_cut = 0.0;
    minima_always_selected_num = 2;
    minima_density_pool_num = 10;
    spots_minima_imp_cut = 0.0;
    sellae_always_selected_num = 6;
    sellae_density_pool_num = 30;
    spots_sellae_imp_cut = 0.0;

    swpts_display = false;
    swpts_active = false;

    do_draw_border_cut = false;
    border_cut  = 0.2;
    do_draw_crop = false;
    do_draw_crop_cut = false;
    crop_cut = 0.2;

    draw_critical_circle = false;
    draw_current_point = false;
    set_life_cut_on_index = false;
};


// void TrackDisplay::read_dem (char *file)
// {
//     dems.push_back(DEMSelector::get (file));
// }

void TrackDisplay::set_boundaries ()
{
    if (ssp)
    {
	clip_white = ssp->dem[0]->max;
	clip_black = ssp->dem[0]->min;
	width = ssp->dem[0]->width;
	height = ssp->dem[0]->height;
    }

    if (asc)
    {
	clip_white = asc->max;
	clip_black = asc->min;	
	width = asc->width;
	height = asc->height;
    }

    crop_llc.x = crop_llc.y = 0;
    crop_hrc.x = width;
    crop_hrc.y = height;
}

void TrackDisplay::read_ssp (char *file)
{
    ssp = new ScaleSpace (file, ScaleSpaceOpts());
}

void TrackDisplay::read_plt (char *file)
{
    FILE* fp = fopen (file, "r");
    if (fp == NULL)
	eprintx (2, "Could not open file `%s'. %s\n", file, strerror (errno));

    int levels;
    int plats;
    int maxpixels;
    int minpixels;
    fscanf (fp, "#levels: %d\n", &levels);

    for (int i = 0; i < levels; i++)
    {
	plateaus.push_back (std::vector < std::vector <Coord> > (0));

	maxpixels = 0;
	minpixels = INT_MAX;

	fscanf (fp, "#plateaus: %d\n", &plats);
	for (int j = 0; j < plats; j++)
	{
	    plateaus[i].push_back (std::vector <Coord> (0));

	    int pixels;
	    fscanf (fp, "#flatpixels: %d e\n", &pixels);
	    for (int k = 0; k < pixels; k++)
	    {
		plateaus[i][j].push_back (Coord (0.0, 0.0));
		fscanf (fp, " %d %d\n",
			 &plateaus[i][j][k].x,
			 &plateaus[i][j][k].y);
	    }

	    if (maxpixels < pixels)
		maxpixels = pixels;
	    if (minpixels > pixels)
		minpixels = pixels;
	}
	
	printf ("level: %d, read %d flat areas. maxsize: %d, minsize: %d\n",
		i, plats, maxpixels, minpixels);
    }
    
    fclose (fp);    
}
void TrackDisplay::read_track (char *file)
{
    track_reader (file, &track, &track_order);
    // track = new Track (file);

    // for (unsigned i = 0; i < track->lines.size(); i++)
    // 	if (track->is_original (i))
    // 	    track->lines[i].strength = track->lines[i].strength * multiply;
}

void TrackDisplay::query (double t)
{
    track->query (t, vquery);
}


void TrackDisplay::getbb (double* cx, double* cy, double* diam)
{
    // *cx = dems[0]->width / 2.0;
    // *cy = dems[0]->height / 2.0;
    // *diam = dems[0]->width * 1.2;
    *cx = width / 2.0;
    *cy = height / 2.0;
    *diam = width * 1.2;    
}

void TrackDisplay::getbb (Point* a, Point* b)
{
    // *a = Point (0.0, 0.0);
    // b->x = (double) dems[0]->width;
    // b->y = (double) dems[0]->height;
    *a = Point (0.0, 0.0);
    b->x = (double) width;
    b->y = (double) height;
}

void __draw_circle() 
{
    float cx = 0.0;
    float cy = 0.0;
    float r = 5.0;
    int num_segments = 10 * sqrtf(r);//change the 10 to a smaller/bigger number as needed

    double st = 1.0;

    float theta = 2 * 3.1415926 / float(num_segments); 
    float tangetial_factor = tanf(theta);//calculate the tangential factor 

    float radial_factor = cosf(theta);//calculate the radial factor 
	
    float x = r;//we start at angle = 0 

    float y = 0; 
    
    glBegin(GL_POLYGON); 
    for(int ii = 0; ii < num_segments; ii++) 
    { 
	glVertex2f(x + cx, y + cy);//output vertex 
        
	//calculate the tangential vector 
	//remember, the radial vector is (x, y) 
	//to get the tangential vector we flip those coordinates and negate one of them 

	float tx = -y; 
	float ty = x; 
        
	//add the tangential vector 

	x += tx * tangetial_factor; 
	y += ty * tangetial_factor; 
        
	//correct using the radial factor 

	x *= radial_factor; 
	y *= radial_factor; 
    } 
    glEnd();


    x = r;//we start at angle = 0 
    y = 0; 
    float px, py;
    float sx = r -st; //we start at angle = 0 
    float sy = 0; 
    float psx, psy;

    // black border around circle
    glColor3f (0.0, 0.0, 0.0);
    glBegin(GL_QUADS); 
    for(int ii = 0; ii < num_segments+1; ii++) 
    { 
 	if (ii != 0)
	{
	    glVertex2f(x + cx, y + cy);
	    glVertex2f(sx + cx, sy + cy);
	    glVertex2f(psx + cx, psy + cy);
	    glVertex2f(px + cx, py + cy);
	}

	// bigger outer circle
	px = x;
	py = y;
	float tx = -y; 
	float ty = x; 
	x += tx * tangetial_factor; 
	y += ty * tangetial_factor; 
	x *= radial_factor; 
	y *= radial_factor; 
	// smaller inner circle
	psx = sx;
	psy = sy;
	float tsx = -sy; 
	float tsy = sx; 
	sx += tsx * tangetial_factor; 
	sy += tsy * tangetial_factor; 
	sx *= radial_factor; 
	sy *= radial_factor; 
    } 
    glEnd(); 
}

void __draw_quad ()
{
    double qs = 10.0;
    double qh = (qs / 2.0);
    double st = 1.0;
    
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

void __draw_critical_color (CriticalType t, bool __circle=false)
{
    switch (t)
    {
    case MIN:
	glColor3dv (blue);
 	break;

    case MAX:
	glColor3dv(red);
	break;

    case SA2:
    case SA3:
	glColor3dv(green);
	break;

    // case SA3:
    // 	glColor3dv(yellow);
    // 	break;
	
    default:
	fprintf (stderr, "__draw_critical_color() problem\n");
    }

    if (__circle)
	__draw_circle ();
    else
	__draw_quad ();
    
 }

void __draw_cross ()
{
    // double qs = 10.0;
    // double qh = (qs / 2.0);

    double ss = 8.0;
    double sg = 2.0;
    // double sd = 1.5;
    // double st = 1.0;
    double sh = (ss / 2.0);

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

void TrackDisplay::__draw_critical_query (TrackRenderingEntry r,  double scale, double tol_mult,
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
    __draw_critical_color (r.type, draw_critical_circle);
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

void TrackDisplay::__draw_critical_elixir (Coord c, CriticalType type, double elixir,  double scale, double tol_mult)
{
    double x = c.x;
    double y = c.y;

    double scale_tol;

    if (multiply_exp)
	scale_tol = pow (tol_mult, 1.0 + elixir);
    else if (multiply_pow)
	scale_tol = pow (1.0 + elixir, tol_mult);
    else
	scale_tol = tol_mult * (1.0 + elixir);

    glPushMatrix ();
    glTranslated ((double) x, (double) y, 0.0);
    glScaled (scale, scale, 1.0);
    if (scale_tol != 0.0)
	glScaled (scale_tol, scale_tol, 1.0);
    __draw_critical_color (type, draw_critical_circle);
    glPopMatrix();
}

// std::vector<int> spots_maxima;
// std::vector<int> spots_minima;
// std::vector<int> spots_sellae;

void TrackDisplay::init_spots ()
{
    // track->drink_elixir ();

    for (unsigned i = 0; i < track->lines.size(); i++)
    {
	if (track->is_original (i))
	{
	    if (track->original_type (i) == MAX)
		spots_maxima.push_back (CritElix (i, track->lifetime_elixir (i)));
	    if (track->original_type (i) == MIN)
		spots_minima.push_back (CritElix (i, track->lifetime_elixir (i)));
	    if (track->original_type (i) == SA2 ||
		track->original_type (i) == SA3)
		spots_sellae.push_back (CritElix (i, track->lifetime_elixir (i)));
	}
    }

    std::sort (spots_maxima.begin(), spots_maxima.end());
    std::sort (spots_minima.begin(), spots_minima.end());
    std::sort (spots_sellae.begin(), spots_sellae.end());
}

// double density_distance (Point a, Point b)
// {
//     return sqrt ((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y));
// }

bool TrackDisplay::is_density (double val, int idx, vector<int>& spots_current)
{
    Point pa = track->start_point (idx);
    for (unsigned i = 0; i < spots_current.size (); i++)
    {
    	Point pb = track->start_point (spots_current[i]);
    	if (point_distance (pa, pb) < val)
    	    return false;
    }
    return true;
}

// double __map (double v, double min, double max)
// {
//     v = v < min? min : v;
//     v = v > max? max : v;

//     return (v - min) / (max - min);
// }

void __draw_box_lines (Coord a, Coord b, double ca[], double cb[], double lines_width)
{
    glLineWidth (lines_width);

    glEnable(GL_LINE_SMOOTH);
    glBegin (GL_LINES);

    glColor3dv (ca);
    glVertex2d (a.x, a.y);
    glVertex2d (a.x, b.y);
    glVertex2d (a.x, a.y);
    glVertex2d (b.x, a.y);

    glColor3dv (cb);    
    glVertex2d (b.x, b.y);
    glVertex2d (a.x, b.y);
    glVertex2d (b.x, b.y);
    glVertex2d (b.x, a.y);

    glEnd();
}
    
static double __clip (double v, double min, double max, double mul)
{
    v *= mul;

    v = v < min? min : v;
    v = v > max? max : v;

    return (v - min) / (max - min);
}

static inline double __random_value (double fmin, double fmax, unsigned* seedp)
{
    double f = ((double) rand_r (seedp)) / RAND_MAX;
    return fmin + f * (fmax - fmin);
}

void TrackDisplay::draw (int dem_idx, Point llc, Point hrc)
{
    int level = dem_idx;
    double enlarge_box = 10.0;
    llc.x += -enlarge_box;
    llc.y += -enlarge_box;
    hrc.x += enlarge_box;
    hrc.y += enlarge_box;

    double width = asc? asc->width: ssp->dem[level]->width;
    double height = asc? asc->height: ssp->dem[level]->height;
    
    Coord bllc, bhrc;
    bllc.x = llc.x < 0 ? 0 : floor (llc.x);
    bllc.y = llc.y < 0 ? 0 : floor (llc.y);
    bhrc.x = hrc.x > width ? width : floor (hrc.x);
    bhrc.y = hrc.y > height ? height : floor (hrc.y);
    // was there a reason for -1 or was it a bug?
    // bhrc.x = hrc.x > width-1 ? width-1 : floor (hrc.x);
    // bhrc.y = hrc.y > height-1 ? height-1 : floor (hrc.y);
    
    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();

    if (draw_terrain)
    {
	// DEMReader* dem = dems[dem_idx];
	// double min = clip_black;
	// double max = clip_white;

	// glBegin (GL_TRIANGLES);
	// for (unsigned int i = 0; i < dem->width - 1; i++)
	//     for (unsigned int j = 0; j < dem->height - 1; j++)
	//     {
	// 	double vij = dem->get_pixel (i, j);
	// 	double vipj = dem->get_pixel (i+1, j);
	// 	double vijp = dem->get_pixel (i, j+1);
	// 	double vipjp = dem->get_pixel (i+1, j+1);
	// 	vij = __map (vij, min, max);
	// 	vipj = __map (vipj, min, max);
	// 	vijp = __map (vijp, min, max);
	// 	vipjp = __map (vipjp, min, max);

	// 	glColor3f (vij, vij, vij);
	// 	glVertex2f (i, j);
	// 	glColor3f (vipj, vipj, vipj);
	// 	glVertex2f (i+1, j);
	// 	glColor3f (vipjp, vipjp, vipjp);
	// 	glVertex2f (i+1, j+1);

	// 	glColor3f (vij, vij, vij);
	// 	glVertex2f (i, j);
	// 	glColor3f (vijp, vijp, vijp);
	// 	glVertex2f (i, j+1);
	// 	glColor3f (vipjp, vipjp, vipjp);
	// 	glVertex2f (i+1, j+1);
	//     }
	// glEnd();

	glEnable(GL_FLAT);
	glBegin (GL_QUADS);
	for (unsigned i = bllc.x; i < bhrc.x; i++)
	    for (unsigned j = bllc.y; j < bhrc.y; j++)
	    {
		double min = clip_black;
		double max = clip_white;
		double mul = multiply;

		double vij = asc? asc->get_pixel (i, j) : (*ssp->dem[level]) (i, j);
		vij = __clip (vij, min, max, mul);
 
		glColor3f (vij, vij, vij);
		glVertex2f (i, j+1);
		glVertex2f (i+1, j+1);
		glVertex2f (i+1, j);
		glVertex2f (i, j);
	    }
	glEnd();
    }

	// if (asc)
	// {
	//     glBegin (GL_QUADS);
	//     for (unsigned i = 0; i < asc->width; i++)
	// 	for (unsigned j = 0; j < asc->height; j++)
	// 	{
	// 	    double min = clip_black;
	// 	    double max = clip_white;
	// 	    double mul = multiply;

	// 	    double vij = asc->get_pixel (i, j);
	// 	    vij = __clip (vij, min, max, mul);
 
	// 	    glColor3f (vij, vij, vij);
	// 	    glVertex2f (i, j+1);
	// 	    glVertex2f (i+1, j+1);
	// 	    glVertex2f (i+1, j);
	// 	    glVertex2f (i, j);
	// 	}
	// }
	// else
	// {
	//     glBegin (GL_QUADS);

	//     for (int i = 0; i < ssp->dem[level]->width; i++)
	// 	for (int j = 0; j < ssp->dem[level]->height; j++)
	// 	{
	// 	    double min = clip_black;
	// 	    double max = clip_white;
	// 	    double mul = multiply;

	// 	    double vij = (*ssp->dem[level]) (i, j);
	// 	    vij = __clip (vij, min, max, mul);
 
	// 	    glColor3f (vij, vij, vij);
	// 	    glVertex2f (i, j+1);
	// 	    glVertex2f (i+1, j+1);
	// 	    glVertex2f (i+1, j);
	// 	    glVertex2f (i, j);
	// 	}
	// }

	    // glBegin (GL_TRIANGLES);
	    // int level = dem_idx;

	    // for (int i = 0; i < ssp->dem[level]->width - 1; i++)
	    // 	for (int j = 0; j < ssp->dem[level]->height - 1; j++)
	    // 	{
	    // 	    double min = clip_black;
	    // 	    double max = clip_white;
	    // 	    double mul = multiply;

	    // 	    double vij = (*ssp->dem[level]) (i, j);
	    // 	    double vipj = (*ssp->dem[level]) (i+1, j);
	    // 	    double vijp = (*ssp->dem[level]) (i, j+1);
	    // 	    double vipjp = (*ssp->dem[level]) (i+1, j+1);
	    // 	    vij = __clip (vij, min, max, mul);
	    // 	    vipj = __clip (vipj, min, max, mul);
	    // 	    vijp = __clip (vijp, min, max, mul);
	    // 	    vipjp = __clip (vipjp, min, max, mul);

	    // 	    glColor3f (vij, vij, vij);
	    // 	    glVertex2f (i, j);
	    // 	    glColor3f (vipj, vipj, vipj);
	    // 	    glVertex2f (i+1, j);
	    // 	    glColor3f (vipjp, vipjp, vipjp);
	    // 	    glVertex2f (i+1, j+1);

	    // 	    glColor3f (vij, vij, vij);
	    // 	    glVertex2f (i, j);
	    // 	    glColor3f (vijp, vijp, vijp);
	    // 	    glVertex2f (i, j+1);
	    // 	    glColor3f (vipjp, vipjp, vipjp);
	    // 	    glVertex2f (i+1, j+1);
	    // 	}

    if (draw_plateaus)
    {
	int i = dem_idx;
	//srand(0);
	static unsigned seedp;
	static int pivot;
	seedp = pivot = 0;
	
	glBegin (GL_QUADS);
	for (unsigned j = 0; j < plateaus[i].size(); j++)
	{
	    // get random color
	    float rc[3];
	    for (int r = 0; r < 3; r++)
		rc[r] = r == pivot?
		    __random_value (0.8, 1.0, &seedp) :
		    __random_value (0.4, 0.8, &seedp) ;

	    pivot++;
	    pivot %= 3;
		    
	    for (unsigned k = 0; k < plateaus[i][j].size(); k++)
	    {
		glColor3f (rc[0], rc[1], rc[2]);
		Coord c = plateaus[i][j][k];

		if (c.is_inside (bllc, bhrc))
		{
		    glVertex2f (c.x,   c.y+1);
		    glVertex2f (c.x+1, c.y+1);
		    glVertex2f (c.x+1, c.y);
		    glVertex2f (c.x,   c.y);
		}
	    }
	}
	
	glEnd();
    }

    // GG WARN old mode to be deleted?
    if (draw_track)
    {
	for (unsigned i = 0; i < track->lines.size(); i++)
	{
	    CriticalType t = track->lines[i].type;
	    __draw_critical_track (track->lines[i].entries[0].c, t, true,
				   track->lines[i].is_born(),
				   track->lifetime(i),
				   track_scale, track_mult);
	    __draw_critical_track (track->lines[i].entries.back().c, t, false,
				   track->lines[i].is_dead(),
				   track->lifetime(i),
				   track_scale, track_mult);
	}
    }

    // GG WARN old mode to be deleted?
    if (draw_query)
    {
	for (unsigned i = 0; i < vquery.size(); i++)
	    __draw_critical_query (vquery[i], query_scale, query_mult,
				   query_cur_pos, query_death);
    }


    // GG WARN old mode to be deleted?
    if (draw_elixir)
    {
	for (unsigned i = 0; i < track->lines.size(); i++)
	{
	    double life_multiplier = normal_lives? track->lifetime (i) : track->lifetime_elixir (i);
	    if (track->is_original (i) && life_multiplier > elixir_cut)
		__draw_critical_elixir (track->lines[i].entries[0].c,
					track->original_type (i),
					life_multiplier,
					elixir_scale, elixir_mult);
	}
    }

    // GG WARN old mode to be deleted?
    if (draw_importance)
    {
	for (unsigned i = 0; i < track->lines.size(); i++)
	{
	    if (track->is_original (i) && track->lines[i].strength > importance_cut)
		__draw_critical_elixir (track->lines[i].entries[0].c,
					track->original_type (i),
					track->lines[i].strength,
					importance_scale, importance_mult);
	}
    }

    if (draw_inspector)
    {
	inspector_maxima_total = inspector_maxima_rendered =
	    inspector_minima_total = inspector_minima_rendered =
	    inspector_sellae_total = inspector_sellae_rendered = 0;

	if (set_life_cut_on_index)
	    maxima_life_cut = minima_life_cut = sellae_life_cut = dem_idx;
	
	for (unsigned i = 0; i < track->lines.size(); i++)
	{
	    if (!track->is_original (i))
		continue;

	    if (!track->lines[i].entries[0].c.is_inside (bllc, bhrc))
		continue;

	    if (track->original_type (i) == MAX)
		inspector_maxima_total++;
	    if (track->original_type (i) == MIN)
		inspector_minima_total++;
	    if (track->original_type (i) == SA2 || track->original_type (i) == SA3)
		inspector_sellae_total++;
		
	    if ((track->original_type (i) == MAX && !inspector_maxima)
		||
		(track->original_type (i) == MIN && !inspector_minima)
		||
		((track->original_type (i) == SA2 || track->original_type (i) == SA3)
		 && !inspector_sellae)
		)
		continue;
		
	    if (track->original_type (i) == MAX &&
		(track->lines[i].strength < maxima_strength_cut ||
		 track->lifetime_elixir (i) < maxima_life_cut))
		continue;

	    if (track->original_type (i) == MIN &&
		(track->lines[i].strength < minima_strength_cut ||
		 track->lifetime_elixir (i) < minima_life_cut))
		continue;

	    if ((track->original_type (i) == SA2 || track->original_type (i) == SA3)
	    	&&
	    	(track->lines[i].strength < sellae_strength_cut
		 ||
		 track->lifetime_elixir (i) < sellae_life_cut))
	    	continue;

	    double vvv, mmm;		
	    if (elixir_mult > 0.0)
	    {
		vvv = track->lifetime_elixir (i);
		mmm = elixir_mult;
	    }
	    else if (importance_mult > 0.0)
	    {
		vvv = track->lines[i].strength;
		mmm = importance_mult;
	    }	
	    else
		vvv = mmm = 0.0;

	    if (track->original_type (i) == MAX)
		inspector_maxima_rendered++;
	    if (track->original_type (i) == MIN)
		inspector_minima_rendered++;
	    if (track->original_type (i) == SA2 || track->original_type (i) == SA3)
		inspector_sellae_rendered++;

	    int ei = draw_current_point? track->get_entry (i, dem_idx) : 0 ;
	    if (ei == -1)
	    {
		printf ("Warning: ei==-1\n");
		ei = 0;
	    }
	    __draw_critical_elixir (track->lines[i].entries[ei].c,
				    track->original_type (i),
				    vvv, spot_scale, mmm);
	}
    }    
    
    if (draw_spots)
    {
	vector<CritElix> spots_cut;
	vector<int> spots_add;
	// vector<int> spots_current;
	spots_current.clear();

	total_num = spots_maxima.size() + spots_minima.size() + spots_sellae.size();
	maxima_total_num = spots_maxima.size();
	minima_total_num = spots_minima.size();
	sellae_total_num = spots_sellae.size();
	
	// maxima
	for (int i = 0; i < maxima_always_selected_num; i++)
	{
	    int k = spots_maxima.size() - (i+1);
	    if (k < 0)
	    {
		maxima_always_selected_num = spots_maxima.size();
		break;
	    }
	    spots_add.push_back (spots_maxima[k].crit);
	    maxima_always_life = spots_maxima[k].elix;
	}
	int i = 0;
	for (int ii = 0; ii < maxima_density_pool_num; ii++)
	{
	    int offset = spots_maxima.size() - maxima_always_selected_num;
	    int k;
	    do
	    {
		k = offset - (++i);
		maxima_always_discarded_num = k >= 0? k : spots_maxima.size();
		if (k < 0)
		    break;
	    }
	    while (track->lines[spots_maxima[k].crit].strength * multiply <
		   spots_maxima_imp_cut);

	    if (k < 0)
	    {
		maxima_density_pool_num = offset - (i - ii-1);
		break;
	    }

	    spots_cut.push_back (spots_maxima[k]);
	    maxima_excluded_life = spots_maxima[k].elix;
	}

	// minima
	for (int i = 0; i < minima_always_selected_num; i++)
	{
	    int k = spots_minima.size() - (i+1);
	    if (k < 0)
	    {
		minima_always_selected_num = spots_minima.size();
		break;
	    }
	    spots_add.push_back (spots_minima[k].crit);
	    minima_always_life = spots_minima[k].elix;
	}
	i = 0;
	for (int ii = 0; ii < minima_density_pool_num; ii++)
	{
	    int offset = spots_minima.size() - minima_always_selected_num;
	    int k;
	    do
	    {
		k = offset - (++i);
		minima_always_discarded_num = k >= 0? k : 0;
		if (k < 0)
		    break;
	    }
	    while (track->lines[spots_minima[k].crit].strength * multiply <
		   spots_minima_imp_cut);

	    if (k < 0)
	    {
		minima_density_pool_num = offset - (i - ii-1);
		break;
	    }

	    spots_cut.push_back (spots_minima[k]);
	    minima_excluded_life = spots_minima[k].elix;
	}

	// sellae
	for (int i = 0; i < sellae_always_selected_num; i++)
	{
	    int k = spots_sellae.size() - (i+1);
	    if (k < 0)
	    {
		sellae_always_selected_num = spots_sellae.size();
		break;
	    }
	    spots_add.push_back (spots_sellae[k].crit);
	    sellae_always_life = spots_sellae[k].elix;
	}
	i = 0;
	for (int ii = 0; ii < sellae_density_pool_num; ii++)
	{
	    int offset = spots_sellae.size() - sellae_always_selected_num;
	    int k;
	    do
	    {
		k = offset - (++i);
		sellae_always_discarded_num = k >= 0? k : 0;
		if (k < 0)
		    break;
	    }
	    while (track->lines[spots_sellae[k].crit].strength * multiply <
		   spots_sellae_imp_cut);

	    if (k < 0)
	    {
		sellae_density_pool_num = offset - (i - ii-1);
		break;
	    }

	    spots_cut.push_back (spots_sellae[k]);
	    sellae_excluded_life = spots_sellae[k].elix;
	}

	maxima_importance_discarded_num =
	    maxima_total_num - maxima_always_discarded_num -
	    maxima_always_selected_num - maxima_density_pool_num;
	minima_importance_discarded_num =
	    minima_total_num - minima_always_discarded_num -
	    minima_always_selected_num - minima_density_pool_num;
	sellae_importance_discarded_num =
	    sellae_total_num - sellae_always_discarded_num -
	    sellae_always_selected_num - sellae_density_pool_num;
	
	if ((int) spots_cut.size() != sellae_density_pool_num +
	    maxima_density_pool_num + minima_density_pool_num)
	    fprintf (stderr, "Warning: spots_cut.size()=%zu, sum mx/mn/sd=%d\n",
		     spots_cut.size(), sellae_density_pool_num +
		     maxima_density_pool_num + minima_density_pool_num);
	
	// sort spots_cut
	std::sort (spots_cut.begin(), spots_cut.end());
	
	// add from spots_add to spots_current
	for (unsigned i = 0; i < spots_add.size(); i++)
	    spots_current.push_back (spots_add[i]);
	
	// add from spots_cut to spots_current density-based
	maxima_density_selected_num = 0;
	minima_density_selected_num = 0;
	sellae_density_selected_num = 0;
	for (int i = (int)spots_cut.size()-1; i >= 0 ; i--)
	    if (is_density (density_maxima_val, spots_cut[i].crit, spots_current))
	    {
		spots_current.push_back (spots_cut[i].crit);

		switch (track->get_type (spots_cut[i].crit))
		{
		case MAX:
		    maxima_density_selected_num++;
		    break;
		case MIN:
		    minima_density_selected_num++;
		    break;
		case SA2:
		case SA3:
		    sellae_density_selected_num++;
		    break;
		default:
		    break;
		}
	    }
	

	always_selected_num = spots_add.size();
	density_selected_num = spots_current.size() - spots_add.size();
	density_pool_num = spots_cut.size();

	double mmm = multiply_elix_spots? 0.5 : 0.0; // 0.0); elixir_mult);
	
	if (draw_final)
	    for (unsigned i = 0; i < spots_current.size(); i++)
	    {
		int idx = spots_current[i];
		__draw_critical_elixir (track->lines[idx].entries[0].c,
					track->original_type (idx),
					track->lifetime_elixir (idx),
					spot_scale, mmm);
	    }

	if (draw_always_selected)
	    for (unsigned i = 0; i < spots_add.size(); i++)
	    {
		int idx = spots_add[i];
		__draw_critical_elixir (track->lines[idx].entries[0].c,
					track->original_type (idx),
					track->lifetime_elixir (idx),
					spot_scale, mmm);
	    }	

	if (draw_density_selected)
	    for (unsigned i = spots_add.size(); i < spots_current.size(); i++)
	    {
	    	int idx = spots_current[i];
	    	__draw_critical_elixir (track->lines[idx].entries[0].c,
	    				track->original_type (idx),
	    				track->lifetime_elixir (idx),
	    				spot_scale, mmm);
	    }

	if (draw_density_pool)
	    for (unsigned i = 0; i < spots_cut.size(); i++)
	    {
	    	int idx = spots_cut[i].crit;
	    	__draw_critical_elixir (track->lines[idx].entries[0].c,
	    				track->original_type (idx),
	    				track->lifetime_elixir (idx),
	    				spot_scale, mmm);
	    }	

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
		    Point f = track->final_point (i);
		    glVertex2d (f.x, f.y); 
		}
	    }
	    glEnd();
	}
    }

    if (draw_csv)
	swpts_draw ();

    if (do_draw_border_cut)
    {
	double w = width;
	double h = height;
	double wc = w * (border_cut / 2.0);
	double hc = h * (border_cut / 2.0);
	double color[3] = {0.0, 1.0, 1.0};
	__draw_box_lines (Coord (0+wc, 0+hc), Coord (w-wc, h-hc),
			  color, color, lines_width);
	
	// glLineWidth (lines_width);

	// glEnable(GL_LINE_SMOOTH);
	// glBegin (GL_LINES);

	// glColor3f (0.0, 1.0, 1.0);
    
	// glVertex2d (0+wc, 0+hc);
	// glVertex2d (0+wc, h-hc);

	// glVertex2d (0+wc, 0+hc);
	// glVertex2d (w-wc, 0+hc);

	// glVertex2d (w-wc, h-hc);
	// glVertex2d (0+wc, h-hc);

	// glVertex2d (w-wc, h-hc);
	// glVertex2d (w-wc, 0+hc);

	// glEnd();
    }

    if (do_draw_crop)
    {
	double ca[3] = {0.8, 1.0, 0.1};
	double cb[3] = {1.0, 0.7, 0.0};
	__draw_box_lines (crop_llc, crop_hrc, ca, cb, lines_width);
	
	// glLineWidth (lines_width);

	// glEnable(GL_LINE_SMOOTH);
	// glBegin (GL_LINES);

	// glColor3f (0.8, 1.0, 0.1);
	// glVertex2d (crop_llc.x, crop_llc.y);
	// glVertex2d (crop_llc.x, crop_hrc.y);
	// glVertex2d (crop_llc.x, crop_llc.y);
	// glVertex2d (crop_hrc.x, crop_llc.y);

	// glColor3f (1.0, 0.7, 0.0);    
	// glVertex2d (crop_hrc.x, crop_hrc.y);
	// glVertex2d (crop_llc.x, crop_hrc.y);
	// glVertex2d (crop_hrc.x, crop_hrc.y);
	// glVertex2d (crop_hrc.x, crop_llc.y);

	// glEnd();
    }

    if (do_draw_crop_cut)
    {
	double w = crop_hrc.x - crop_llc.x;
	double h = crop_hrc.y - crop_llc.y;
	Coord e = Coord((w / (1.0 - crop_cut)) * crop_cut,
			(h / (1.0 - crop_cut)) * crop_cut);
	crop_cut_llc = crop_llc - e;
	crop_cut_hrc = crop_hrc + e;
	double ca[3] = {0.7, 0.6, 0.1};
	double cb[3] = {0.9, 0.3, 0.0};
	__draw_box_lines (crop_cut_llc, crop_cut_hrc, ca, cb, lines_width);
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
	double ti = track->time_of_life/100.0 * ((double)i);
	time2color (ti, 0.0, track->time_of_life, color);
	glColor3dv (color);
	glVertex2d (d/100.0 * ((double)i), d/100.0 * ((double)i));
	
	double tip1 = track->time_of_life/100.0 * ((double)i+1.0);
	time2color (tip1, 0.0, track->time_of_life, color);
	glColor3dv (color);
	glVertex2d (d/100.0 * ((double)i+1.0), d/100.0 * ((double)i+1.0));
	// printf ("$$$ %d: %lf %lf -- %lf\n", i, ti, tip1, track->time_of_life);
    }
    glEnd();
    //   exit (0);
}

void TrackDisplay::read_asc (char* file)
{
    swpts_active = true;

    asc = new ASCReader (file);

    // swpts_xllcorner = asc->xllcorner;
    // swpts_yllcorner = asc->yllcorner;
    // swpts_cellsize = asc->cellsize;
}

void TrackDisplay::swpts_draw ()
{
    if (!swpts_display)
	return;

    // static bool check = true;
    
    for (unsigned i = 0; i < swpts_ground_truth.size(); i++)
    {
	double x = swpts_ground_truth[i].p.x;
	double y = swpts_ground_truth[i].p.y;

	// if (check)
	//     printf ("GRUNTH: %lf,%lf %s ", x, y,
	// 	    i < swpts_ground_truth.size()-1? "--" : "|\n");
	
	glPushMatrix ();
	glTranslated ((double) x, (double) y, 0.0);
	glScaled (spot_scale, spot_scale, 1.0);
	glColor3dv(magenta);	
	__draw_cross ();
	glPopMatrix();

	glPushMatrix ();
	glTranslated ((double) x, (double) y, 0.0);
	glScaled (spot_scale / 2.0, spot_scale / 2.0, 1.0);
	glColor3dv(black);
	__draw_cross ();

	glPopMatrix();

    }

    // check = false;
}


void TrackDisplay::swpts_load_csv (char* filename)
{
    // CSVReader csvio (asc->width, asc->height, asc->cellsize,
    // 		     asc->xllcorner, asc->yllcorner);
    CSVReader csvio (*asc);
    csvio.load (filename, swpts_ground_truth);
    
    swpts_display = true;
}

#include "../common/strlcat.h"

void TrackDisplay::swpts_save_csv (char* filename)
{
    if (!swpts_active)
	return;

    char cwd[2048] = "\0";
    
    // printf ("cwd: %s\n", cwd);
    // printf ("filename: %s\n", filename);

    if (filename != NULL && filename[0] != '/')
    {
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	    fprintf(stdout, "Could not get working dir: %s\n", strerror(errno));
	
	// printf ("cwd: %s\n", cwd);
	// printf ("filename: %s\n", filename);

	char *app_ending = ".app/Contents/Resources";
	if (ends_with (cwd, app_ending))
	{
	    cwd[0] = '\0';
	    strlcat (cwd, "../../../", 2048);
	}
	else
	    cwd[0] = '\0';	    
    }
    strlcat (cwd, filename, 2048);

    // printf ("final: %s\n", cwd);

    CSVReader csvio (*asc);
    csvio.save (cwd, spots_current, track, ssp);
}
