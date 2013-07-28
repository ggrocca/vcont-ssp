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

class Camera {
public:

    Camera();
    ~Camera();

    void reset (double cx, double cy, double d);

    void display_begin (void);
    void display_end (void);

    void reshape (int w, int h);
    void mouse (int button, int state, int x, int y);
    void mouse_move (int x, int y);
    void key(unsigned char inkey, int px, int py);
    void special(int key, int px, int py);

    int width, height;
    bool zoom, pan;

    double center_x, center_y, diameter;
    
private:
    double scale, scale_key;
    double trans_x, trans_y, trans_x_key, trans_y_key;
    double scale_key_factor, trans_key_factor;
};


#endif // _CAMERA_HH

