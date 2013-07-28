#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
 
#include "camera.hh"

bool do_print = true;

Camera::Camera()
{
    width = height = 0;
    zoom = pan = false;

    scale = scale_key = 2.0;
    trans_y = trans_y_key = 0.0;
    trans_x = trans_x_key = 0.0;
}

Camera::~Camera(){}

void Camera::reset (double cx, double cy, double d)
{
    center_x = -cx;
    center_y = -cy;
    diameter = d;

    scale_key_factor = 2.0;
    trans_key_factor = diameter / 2.0;
    do_print = true;
}

void Camera::display_begin (void)
{
    double aspect = ((double) width) / (double) height; 

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    glOrtho(-diameter * aspect,
	    diameter * aspect,
	    -diameter,
	    diameter,
	    -1, 1);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

    // scale
    glScaled (scale, scale, 1.0);
    // translate
    glTranslated (center_x + ((1.0 / scale) * trans_x),
		  center_y + ((1.0 / scale) * trans_y),
		  0.0);

    if (do_print)
    {
	double model[16];
	double proj[16];
	int view[4];
	glGetDoublev (GL_MODELVIEW_MATRIX, model);
	glGetDoublev (GL_PROJECTION_MATRIX, proj);
	glGetIntegerv (GL_VIEWPORT, view);
	double x,y,z, u, v, k;
	gluProject (0,0,0,model,proj,view, &x, &y, &z);
	printf ("%lf, %lf, %lf, ==== ", x, y, z);
	gluProject (1,1,0,model,proj,view, &u, &v, &k);
	printf ("%lf, %lf, %lf !!!! ", u, v, k);
	printf ("!! 1px: %lf _ 1py: %lf ## 1/1px: %lf _ 1/1py: %lf\n",
		u - x, v - y, 1.0/(u - x), 1.0/(v - y));
	do_print = false;
    }
}


void Camera::display_end (void)
{

}


void Camera::reshape (int w, int h)
{
    width = w;
    height = h;
    do_print = true;
}


void Camera::mouse (int button, int state, int x, int y)
{
    (void) button;
    (void) state;
    (void) x;
    (void) y;
}


void Camera::mouse_move (int x, int y)
{
    (void) x;
    (void) y;
}


void Camera::key(unsigned char inkey, int px, int py)
{
    (void) px; (void) py;
    switch (inkey)
    {
    case 27:
	exit(0);
	break;
    case 'a':
	scale_key *= scale_key_factor;
	scale = scale_key;
	break;

    case 'z':
	scale_key /= scale_key_factor;
	scale = scale_key;
	break;

    case 'q':
	scale = scale_key = 2.0;
	break;

    case 'w':
	trans_y = trans_y_key = 0.0;
	trans_x = trans_x_key = 0.0;
	break;
     }

    glutPostRedisplay();
    do_print = true;
}


void Camera::special(int key, int px, int py)
{
    (void) px; (void) py;

    switch (key)
    {
    case GLUT_KEY_UP:
	trans_y_key -= trans_key_factor;
	trans_y = trans_y_key;
	break;

    case GLUT_KEY_DOWN:
	trans_y_key += trans_key_factor;
	trans_y = trans_y_key;
	break;

    case GLUT_KEY_LEFT:
	trans_x_key += trans_key_factor;
	trans_x = trans_x_key;
	break;

    case GLUT_KEY_RIGHT:
	trans_x_key -= trans_key_factor;
	trans_x = trans_x_key;
	break;
    }

    glutPostRedisplay();
    do_print = true;
}
