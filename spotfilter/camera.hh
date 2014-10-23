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

    Point wllc ();
    Point whrc ();
    
    int width, height;
    double aspect;

    Point box_a,box_b;
    Point box_center;
    double x_dim, y_dim;
    double short_dim, long_dim;

    double virtual_width, virtual_height;
    Point virtual_center;
    bool zooming_out, zooming_in, panning;
    
private:
    double _ortho_left ();
    double _ortho_right ();
    double _ortho_bottom ();
    double _ortho_top ();

    double move_x, move_y;
};


#endif // _CAMERA_HH

