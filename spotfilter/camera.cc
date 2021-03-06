#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
 
#include "camera.hh"


Camera::Camera(){}


Camera::Camera(int w, int h, Point a, Point b)
{
    set (w, h, a, b);
    
    zooming_in = false;
    zooming_out = false;
    panning = false;
}


void Camera::set (int w, int h, Point a, Point b)
{
    reshape (w, h);
    set_bb (a, b);
    reset ();
}

Camera::~Camera(){}


void Camera::set_bb (Point a, Point b)
{
    box_a = a;
    box_b = b;

    x_dim = b.x - a.x;
    y_dim = b.y - a.y;

    short_dim = (x_dim < y_dim? x_dim : y_dim);
    long_dim = (x_dim > y_dim? x_dim : y_dim);
    box_center = box_a + Point (x_dim/2.0, y_dim/2.0);
}

void Camera::reset ()
{
    zooming_in = false;
    zooming_out = false;
    panning = false;

    double start_zoom_out = 1.0; // GG WARN

    virtual_width = virtual_height = short_dim * start_zoom_out;
    virtual_center = box_center;
    virtual_center.x -= (((virtual_width * aspect) - virtual_width) / 2.0) / aspect;

    printf ("virtual dim %lf, vcent(%lf, %lf), aspect %lf\n", virtual_width, virtual_center.x, virtual_center.y, aspect);

    printf ("GLORTHO: %lf, %lf, %lf, %lf\n",
	    (virtual_center.x - ( virtual_width / 2.0)) * aspect,
	    (virtual_center.x + ( virtual_width / 2.0)) * aspect,
	     virtual_center.y - ( virtual_height / 2.0),
	     virtual_center.y + ( virtual_height / 2.0) );

    // virtual_center.x += (long_dim - short_dim) * start_zoom_out;
}

void Camera::reshape (int w, int h)
{
    width = w;
    height = h;
    aspect = ((double) width) / (double) height; 

    glutPostRedisplay();
}

double Camera::_ortho_left ()
{
    return (virtual_center.x - ( virtual_width / 2.0)) * aspect;
}
double Camera::_ortho_right ()
{
    return (virtual_center.x + ( virtual_width / 2.0)) * aspect;
}
double Camera::_ortho_bottom ()
{
    return virtual_center.y - ( virtual_height / 2.0);
}
double Camera::_ortho_top ()
{
    return virtual_center.y + ( virtual_height / 2.0);
}

void Camera::display_begin (void)
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    glOrtho (_ortho_left (), _ortho_right (), _ortho_bottom (), _ortho_top (), -1, 1);
    // glOrtho ((virtual_center.x - ( virtual_width / 2.0)) * aspect,
    // 	     (virtual_center.x + ( virtual_width / 2.0)) * aspect,
    // 	      virtual_center.y - ( virtual_height / 2.0),
    // 	      virtual_center.y + ( virtual_height / 2.0),
    // 	      -1, 1);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
}


void Camera::display_end (void)
{

}


void Camera::mouse (int button, int state, int mx, int mmy)
{
    (void) button;

    // flip mouse y axis so up is +y
    int my = height - mmy;

    if (info_mode)
    {
	if (state == GLUT_UP)
	{
	    double model[16];
	    double proj[16];
	    int view[4];
	    double ox, oy, oz;
	    glGetDoublev (GL_MODELVIEW_MATRIX, model);
	    glGetDoublev (GL_PROJECTION_MATRIX, proj);
	    glGetIntegerv (GL_VIEWPORT, view);
	    gluUnProject (mx, my, 0, model, proj, view, &ox, &oy, &oz);

	    printf ("image coords: %lf, %lf\n", ox, oy);
	}
	return;
    }
    
    // convert mouse coords to (-1/2,-1/2)-(1/2, 1/2) box
    double x = ((double) mx / (double) width ) - 0.5;
    double y = ((double) my / (double) height ) - 0.5;

    if (state == GLUT_DOWN && !(zooming_out || zooming_in))
    {
    	panning = true;
    	move_x = x;
    	move_y = y;
    }

    if (state == GLUT_UP && panning && !(zooming_out || zooming_in))
    	panning = false;

    if (state == GLUT_UP && (zooming_out || zooming_in) && !panning)
    // if (state == GLUT_UP && (zooming_out || zooming_in))
    {
        double preX = (x * virtual_width);
        double preY = (y * virtual_height);

        double zoomFactor = 1.5; // GG WARN
        if (zooming_in)
        {
            // zoom in
            virtual_width /= zoomFactor;
            virtual_height /= zoomFactor;
        }
        if (zooming_out)
        {
            // zoom out
            virtual_width *= zoomFactor;
            virtual_height *= zoomFactor;
        }

        double postX = (x * virtual_width);
        double postY = (y * virtual_height);

        // recenter
        virtual_center.x += (preX - postX);
        virtual_center.y += (preY - postY);
    }

    glutPostRedisplay();
}

void Camera::mouse_move (int mx, int my)
{
    if (panning)
    {
	my = height - my;

	// convert mouse coords to (-1/2,-1/2)-(1/2, 1/2) box
	double x = ((double) mx / (double) width) - 0.5;
	double y = ((double) my / (double) height) - 0.5;
    
	virtual_center.x += (move_x - x) * (virtual_width * 1.0); // GG WARN
	virtual_center.y += (move_y - y) * (virtual_height * 1.0);

	move_x = x;
	move_y = y;

	glutPostRedisplay();
    }
}


void Camera::key_up(unsigned char upkey, int mx, int my)
{
    (void) mx; (void) my;

    switch (upkey)
    {
    case 'a':
	zooming_in = false;
	break;

    case 'z':
	zooming_out = false;
	break;

    case 'i':
	info_mode = false;
	break;
    }

    glutPostRedisplay();
}


void Camera::key(unsigned char inkey, int mx, int my)
{
    (void) mx; (void) my;

    switch (inkey)
    {
    case 27:
	exit(0);
	goto post;
    case 'a':
	zooming_in = true;
	goto post;

    case 'z':
	zooming_out = true;
	goto post;

    case 'i':
	info_mode = true;
	break;
	
    case 'q':
	reset ();
	goto post;

    case 'w':
	printf ("c.x=%lf c.y=%lf w=%lf h=%lf\n",
		virtual_center.x, virtual_center.y, virtual_width, virtual_height);
	printf ("left=%lf, right=%lf, bottom=%lf, top=%lf\n",
		_ortho_left (), _ortho_right (), _ortho_bottom (), _ortho_top ());
	break;
     }

    return;

 post:
    glutPostRedisplay();
}

Point Camera::wllc ()
{
    return Point (_ortho_left (), _ortho_bottom ());
}

Point Camera::whrc ()
{
    return Point (_ortho_right (), _ortho_top ());
}

void Camera::special(int key, int mx, int my)
{
    (void) mx; (void) my;

    switch (key)
    {
    case GLUT_KEY_UP:
	goto post;

    case GLUT_KEY_DOWN:
	goto post;

    case GLUT_KEY_LEFT:
	goto post;

    case GLUT_KEY_RIGHT:
	goto post;
    }

    return;

 post:
    glutPostRedisplay();
}
