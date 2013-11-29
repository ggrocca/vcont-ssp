#include "displaydem.hh"

// class DisplayDem : public DisplayPlane
// {
//     // TwBar* bar;
//     // string bar_name;
//     // GeoMapping* map;
//     // int order;
    
//     DisplayDem (Plane* p);
//     ~DisplayDem ();

//     void display ();
// }


DisplayDem::DisplayDem (Plane* p, GeoMapping* m, int idx) : DisplayPlane (idx)
{
    // data
    
    map = m;

    if (p->type != DEM)
	eprintx (6, "Abort, %s wrong type for me.\n", (ds_type2string(p->type)).c_str());

    dr = (DEMReader*) p->data;


    // interface

    clip_black = 0.0;
    TwAddVarRW(bar, "clip_black", TW_TYPE_DOUBLE, &clip_black,
	       "min=0.0 max=65536.0 step=10.0");
    clip_white = 65535.0;
    TwAddVarRW(bar, "clip_white", TW_TYPE_DOUBLE, &clip_white,
	       "min=0.0 max=65536.0 step=10.0");
    clip_white = 65535.0;
    TwAddVarRW(bar, "mult factor", TW_TYPE_DOUBLE, &multiply,
	       "min=1.0 max=256.0 step=1.0");
    multiply = 1.0;
    
    order = idx? -1 : 0;
}

DisplayDem::~DisplayDem () {}


double __clip (double v, double min, double max, double mul)
{
    v *= mul;

    v = v < min? min : v;
    v = v > max? max : v;

    return (v - min) / (max - min);
}

void DisplayDem::display ()
{
    // static int yay = 1;
    // if (yay)
    // 	tprint ("Here I AM %d\n", yay = !yay);
    
    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();

    double min = clip_black;
    double max = clip_white;
    double mul = multiply;

    glBegin (GL_TRIANGLES);
    for (unsigned int i = 0; i < dr->width - 1; i++)
	for (unsigned int j = 0; j < dr->height - 1; j++)
	{
	    double vij = dr->get_pixel (i, j);
	    double vipj = dr->get_pixel (i+1, j);
	    double vijp = dr->get_pixel (i, j+1);
	    double vipjp = dr->get_pixel (i+1, j+1);
	    vij = __clip (vij, min, max, mul);
	    vipj = __clip (vipj, min, max, mul);
	    vijp = __clip (vijp, min, max, mul);
	    vipjp = __clip (vipjp, min, max, mul);

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


    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix ();

}
