#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
 
#include "camera.hh"
#include "debug.h"

Camera::Camera(){}


Camera::Camera(int w, int h, BoundingBox box)
{
    set (w, h, box);
    
    zooming_in = false;
    zooming_out = false;
    panning = false;
}


void Camera::set (int w, int h, BoundingBox box)
{
    reshape (w, h);
    set_bb (box);
    reset ();
}

Camera::~Camera(){}


void Camera::set_bb (BoundingBox box) 
{
    this->box = box;

    x_dim = box.b.x - box.a.x;
    y_dim = box.b.y - box.a.y;

    short_dim = (x_dim < y_dim? x_dim : y_dim);
    long_dim = (x_dim > y_dim? x_dim : y_dim);
    box_center = box.a + Point (x_dim/2.0, y_dim/2.0);
}

void Camera::reset ()
{
    zooming_in = false;
    zooming_out = false;
    panning = false;

    double start_zoom_out = 1.0; // GG WARN

    // virtual_width = virtual_height = short_dim * start_zoom_out;
    virtual_width = virtual_height = long_dim * start_zoom_out;

    virtual_center = box_center;
    // virtual_center.x -= (((virtual_width * aspect) - virtual_width) / 2.0) / aspect;
    //virtual_center.x -= (((virtual_width * ) - virtual_width) / 2.0);

    // virtual_center.x += (long_dim - short_dim) * start_zoom_out;
}

void Camera::reshape (int w, int h)
{
    width = w;
    height = h;
    aspect = ((double) width) / (double) height; 

    glutPostRedisplay();
}


void Camera::display_begin (void)
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    // glOrtho ((virtual_center.x - ( virtual_width / 2.0)) * aspect,
    // 	     (virtual_center.x + ( virtual_width / 2.0)) * aspect,
    glOrtho ((virtual_center.x - (( virtual_width / 2.0) * aspect)),
	     (virtual_center.x + (( virtual_width / 2.0) * aspect)),
	      virtual_center.y - ( virtual_height / 2.0),
	      virtual_center.y + ( virtual_height / 2.0),
	      -1, 1);

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

    tprints (SCOPE_CAMERA, "virtual dim %lf, vcent(%lf, %lf), aspect %lf\n",
	     virtual_width, virtual_center.x, virtual_center.y, aspect);

    tprints (SCOPE_CAMERA, "GLORTHO: %lf, %lf, %lf, %lf\n\n\n",
	    (virtual_center.x - (( virtual_width / 2.0) * aspect)),
	    (virtual_center.x + (( virtual_width / 2.0) * aspect)),
	    // (virtual_center.x - ( virtual_width / 2.0)) * aspect,
	    // (virtual_center.x + ( virtual_width / 2.0)) * aspect,
	    virtual_center.y - ( virtual_height / 2.0),
	    virtual_center.y + ( virtual_height / 2.0) );

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

    case 'q':
	reset ();
	goto post;

    case 'w':
	goto post;
     }

    return;

 post:
    glutPostRedisplay();
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
