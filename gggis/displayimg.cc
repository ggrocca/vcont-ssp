#include "displayimg.hh"

// class DisplayIMG : public DisplayPlane
// {
//     // TwBar* bar;
//     // string bar_name;
//     // GeoMapping* map;
//     // int order;
    
//     DisplayIMG (Plane* p);
//     ~DisplayIMG ();

//     void display ();
// }


DisplayIMG::DisplayIMG (Plane* p, GeoMapping* m, int sidx, int pidx) : DisplayPlane (sidx, pidx)
{
    // data
    
    map = m;

    if (p->type != IMG)
	eprintx (6, "Abort, %s wrong type for me.\n", (ds_type2string(p->type)).c_str());

    img = (cimg_library::CImg<unsigned char>*) p->data[0];

    clip_black = 0.0;
    TwAddVarRW(bar, "clip_black", TW_TYPE_DOUBLE, &clip_black,
	       "min=0.0 max=65536.0 step=10.0");
    clip_white = 255.0;
    TwAddVarRW(bar, "clip_white", TW_TYPE_DOUBLE, &clip_white,
	       "min=0.0 max=65536.0 step=10.0");
    multiply = 1.0;
    TwAddVarRW(bar, "mult factor", TW_TYPE_DOUBLE, &multiply,
	       "min=1.0 max=256.0 step=1.0");
}

DisplayIMG::~DisplayIMG () {}


static double __clip (double v, double min, double max, double mul)
{
    v *= mul;

    v = v < min? min : v;
    v = v > max? max : v;

    return (v - min) / (max - min);
}

static inline int __invert (int y, int h)
{
    return h - y - 1;
}

void DisplayIMG::display ()
{
    double min = clip_black;
    double max = clip_white;
    double mul = multiply;

    unsigned width = img->width ();
    unsigned height = img->height ();

    if (img->spectrum() == 1)
    {
	glBegin (GL_TRIANGLES);
	for (unsigned int i = 0; i < width - 1; i++)
	    for (unsigned int j = 0; j < height - 1; j++)
	    {
		double vij =   (*img) (i,   __invert (j,   height));
		double vipj =  (*img) (i+1, __invert (j,   height));
		double vijp =  (*img) (i,   __invert (j+1, height));
		double vipjp = (*img) (i+1, __invert (j+1, height));

		// double vij =   (*img) (i,   j);
		// double vipj =  (*img) (i+1, j);
		// double vijp =  (*img) (i,   j+1);
		// double vipjp = (*img) (i+1, j+1);

		// double min = 0.0;
		// double max = 255.0;
		// double mul = 1.0;

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
    }
    else if (img->spectrum() == 3)
    {
	glBegin (GL_TRIANGLES);
	for (unsigned int i = 0; i < width - 1; i++)
	    for (unsigned int j = 0; j < height - 1; j++)
	    {
		double vij0 =   (*img) (i,   __invert (j,   height), 0, 0);
		double vij1 =   (*img) (i,   __invert (j,   height), 0, 1);
		double vij2 =   (*img) (i,   __invert (j,   height), 0, 2);

		double vipj0 =  (*img) (i+1, __invert (j,   height), 0, 0);
		double vipj1 =  (*img) (i+1, __invert (j,   height), 0, 1);
		double vipj2 =  (*img) (i+1, __invert (j,   height), 0, 2);

		double vijp0 =  (*img) (i,   __invert (j+1, height), 0, 0);
		double vijp1 =  (*img) (i,   __invert (j+1, height), 0, 1);
		double vijp2 =  (*img) (i,   __invert (j+1, height), 0, 2);

		double vipjp0 = (*img) (i+1, __invert (j+1, height), 0, 0);
		double vipjp1 = (*img) (i+1, __invert (j+1, height), 0, 1);
		double vipjp2 = (*img) (i+1, __invert (j+1, height), 0, 2);

		// double min = 0.0;
		// double max = 256.0;
		// double mul = 1.0;

		vij0 = __clip (vij0, min, max, mul);
		vij1 = __clip (vij1, min, max, mul);
		vij2 = __clip (vij2, min, max, mul);

		vipj0 = __clip (vipj0, min, max, mul);
		vipj1 = __clip (vipj1, min, max, mul);
		vipj2 = __clip (vipj2, min, max, mul);

		vijp0 = __clip (vijp0, min, max, mul);
		vijp1 = __clip (vijp1, min, max, mul);
		vijp2 = __clip (vijp2, min, max, mul);

		vipjp0 = __clip (vipjp0, min, max, mul);
		vipjp1 = __clip (vipjp1, min, max, mul);
		vipjp2 = __clip (vipjp2, min, max, mul);

		glColor3f (vij0, vij1, vij2);
		glVertex2f (i, j);
		glColor3f (vipj0, vipj1, vipj2);
		glVertex2f (i+1, j);
		glColor3f (vipjp0, vipjp1, vipjp2);
		glVertex2f (i+1, j+1);

		glColor3f (vij0, vij1, vij2);
		glVertex2f (i, j);
		glColor3f (vijp0, vijp1, vijp2);
		glVertex2f (i, j+1);
		glColor3f (vipjp0, vipjp1, vipjp2);
		glVertex2f (i+1, j+1);
	    }
	glEnd();
    }
}
