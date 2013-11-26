#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
 
#include "camera.hh"

Camera::Camera(){}

Camera::Camera(int w, int h, Point a, Point b)
{
    set (w, h, a, b);
    
    // width = w;
    // height = h;

    // zoom = pan = false;

    // scale = scale_key = 2.0;
    // trans_y = trans_y_key = 0.0;
    // trans_x = trans_x_key = 0.0;

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

// void Camera::reset (double cx, double cy, double d)
// {
//     center_x = -cx;
//     center_y = -cy;
//     diameter = d;

//     // scale_key_factor = 2.0;
//     // trans_key_factor = diameter / 2.0;
//     // do_print = true;
// }

void Camera::set_bb (Point a, Point b)
{
    box_a = a;
    box_b = b;

    x_dim = b.x - a.x;
    y_dim = b.y - a.y;

    hs_dim = (x_dim < y_dim? x_dim : y_dim) / 2.0;
    // Point suca = Point (x_dim, y_dim);
    // Point gne = box_a + suca;
    // box_center = gne;
    box_center = box_a + Point (x_dim/2.0, y_dim/2.0);

    // reset ();
}

void Camera::reset ()
{
    // zoomTranslateX = 0.0;
    // zoomTranslateY = 0.0;
    // totalMovedX = 0.0;
    // totalMovedY = 0.0;
    // currentScale = 1.0;
    // zoomFactor = 2.0;
    // moveFactor = 50.0;

    zooming_in = false;
    zooming_out = false;
    panning = false;


    virtual_width = hs_dim * 2.0; // GG WARN
    virtual_height = hs_dim * 2.0;    
    virtual_center = box_center;
}

void Camera::reshape (int w, int h)
{
    width = w;
    height = h;
    // do_print = true;

    aspect = ((double) width) / (double) height; 

    glutPostRedisplay();
}




// void Camera::zoom_in ()
// {
//   currentScale = currentScale + zoomFactor;
//   double xAdjust = x_dim * zoomFactor/2;
//   double yAdjust = y_dim * zoomFactor/2;
//   zoomTranslateX += xAdjust;
//   zoomTranslateY += yAdjust;
// }

// void Camera::zoom_out ()
// {
//     if (currentScale - zoomFactor >= 1.0)
//     {
// 	currentScale = currentScale - zoomFactor;
// 	double xAdjust = x_dim * zoomFactor/2;
// 	double yAdjust = y_dim * zoomFactor/2;
// 	zoomTranslateX -= xAdjust;
// 	zoomTranslateY -= yAdjust;
//     }
// }


void Camera::display_begin (void)
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    glOrtho ((virtual_center.x - ( virtual_width / 2.0)) * aspect,
	     (virtual_center.x + ( virtual_width / 2.0)) * aspect,
	      virtual_center.y - ( virtual_height / 2.0),
	      virtual_center.y + ( virtual_height / 2.0),
	      -1, 1);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();


    // glMatrixMode (GL_PROJECTION);
    // glLoadIdentity ();

    // glOrtho(-diameter * aspect,
    // 	    diameter * aspect,
    // 	    -diameter,
    // 	    diameter,
    // 	    -1, 1);

    // // translate
    // glTranslated (center_x + ((1.0 / scale) * trans_x),
    // 		  center_y + ((1.0 / scale) * trans_y),
    // 		  0.0);
    // // scale
    // glScaled (scale, scale, 1.0);

    // glOrtho(-hs_dim * aspect,
    // 	    hs_dim * aspect,
    // 	    -hs_dim,
    // 	    hs_dim,
    // 	    -1, 1);


    // glMatrixMode (GL_MODELVIEW);
    // glLoadIdentity ();

    // glTranslated(-zoomTranslateX, -zoomTranslateY, 0);    
    // glTranslated(totalMovedX, totalMovedY, 0);
    // glScaled(currentScale, currentScale, 1);

    // if (do_print)
    // {
	// double model[16];
	// double proj[16];
	// int view[4];
	// glGetDoublev (GL_MODELVIEW_MATRIX, model);
	// glGetDoublev (GL_PROJECTION_MATRIX, proj);
	// glGetIntegerv (GL_VIEWPORT, view);
	// double x,y,z, u, v, k;
	// gluProject (0,0,0,model,proj,view, &x, &y, &z);
	// printf ("%lf, %lf, %lf, ==== ", x, y, z);
	// gluProject (1,1,0,model,proj,view, &u, &v, &k);
	// printf ("%lf, %lf, %lf !!!! ", u, v, k);
	// printf ("!! 1px: %lf _ 1py: %lf ## 1/1px: %lf _ 1/1py: %lf\n",
	// 	u - x, v - y, 1.0/(u - x), 1.0/(v - y));

	//printf ()

	// do_print = false;
    // }
}


void Camera::display_end (void)
{

}


void Camera::mouse (int button, int state, int mx, int my)
{
    // (void) button;
    // (void) state;
    // (void) x;
    // (void) y;

    // flip mouse y axis so up is +y
    my = height - my;

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
    // (void) x;
    // (void) y;

    if (panning)
    {
	my = height - my;

	// convert mouse coords to (-1/2,-1/2)-(1/2, 1/2) box
	double x = ((double) mx / (double) width) - 0.5;
	double y = ((double) my / (double) height) - 0.5;
    
	// virtual_center.x += (x - move_x) * (virtual_width * 1.0); // GG WARN
	// virtual_center.y += (y - move_y) * (virtual_height * 1.0);
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
	// scale_key *= scale_key_factor;
	// scale = scale_key;

	// zoom_in ();

	zooming_in = true;
	goto post;

    case 'z':
	// scale_key /= scale_key_factor;
	// scale = scale_key;

	// zoom_out ();

	zooming_out = true;
	goto post;

    case 'q':
	// scale = scale_key = 2.0;
	reset ();
	goto post;

    case 'w':
	// trans_y = trans_y_key = 0.0;
	// trans_x = trans_x_key = 0.0;
	goto post;
     }

    return;

 post:
    glutPostRedisplay();
    // do_print = true;
}


void Camera::special(int key, int mx, int my)
{
    (void) mx; (void) mx;

    switch (key)
    {
    case GLUT_KEY_UP:
	// trans_y_key -= trans_key_factor;
	// trans_y = trans_y_key;

	// totalMovedY -= moveFactor;
	goto post;

    case GLUT_KEY_DOWN:
	// trans_y_key += trans_key_factor;
	// trans_y = trans_y_key;

	// totalMovedY += moveFactor;
	goto post;

    case GLUT_KEY_LEFT:
	// trans_x_key += trans_key_factor;
	// trans_x = trans_x_key;

	// totalMovedX -= moveFactor;
	goto post;

    case GLUT_KEY_RIGHT:
	// trans_x_key -= trans_key_factor;
	// trans_x = trans_x_key;

	// totalMovedX += moveFactor;
	goto post;
    }

    return;

 post:
    glutPostRedisplay();
    // do_print = true;
}
