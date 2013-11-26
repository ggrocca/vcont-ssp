#ifndef _CAMERA_HH
#define _CAMERA_HH

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
 
#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#include "grid.hh"

class Camera {
public:

    Camera ();
    Camera (int w, int h, Point a, Point b);
    ~Camera ();

    // void reset (double cx, double cy, double d);
    void set (int w, int h, Point a, Point b);
    void set_bb (Point a, Point b);
    void reset ();
    void display_begin (void);
    void display_end (void);

    void reshape (int w, int h);
    void mouse (int button, int state, int x, int y);
    void mouse_move (int x, int y);
    void key (unsigned char inkey, int px, int py);
    void key_up (unsigned char upkey, int px, int py);
    void special (int key, int px, int py);

    int width, height;
    double aspect;

    // bool zoom, pan;
    // double center_x, center_y, diameter;

    Point box_a,box_b;
    Point box_center;
    double x_dim;
    double y_dim;
    double hs_dim;

    double virtual_width, virtual_height;
    Point virtual_center;
    bool zooming_out, zooming_in, panning;
    
private:
    // double scale, scale_key;
    // double trans_x, trans_y, trans_x_key, trans_y_key;
    // double scale_key_factor, trans_key_factor;

    // double zoomTranslateX, zoomTranslateY, totalMovedX, totalMovedY, currentScale;
    // double zoomFactor;
    // double moveFactor;

    // void zoom_in ();
    // void zoom_out ();

    double move_x, move_y;
};


#endif // _CAMERA_HH

