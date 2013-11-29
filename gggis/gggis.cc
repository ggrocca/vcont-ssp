#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#define __ESC_GLUT 27
#include <AntTweakBar.h>

#include <sstream>

#include "../common/utils.hh"
#include "camera.hh"
#include "datamanager.hh"
#include "displaymanager.hh"



/////////////
// GLOBALS //
/////////////

#define W 1260
#define H 760

DataManager* data_m;
DisplayManager* disp_m;
Camera camera;

int width = W;
int height = H;



//////////
// QUIT //
//////////

void quit()
{
    // insert here eventual clean up code
    exit(0);
}




///////////////////
// GLUT CALLBACK //
///////////////////

bool test = false;
void display()
{
    glClearColor(1.0f, 1.0f, 1.0f, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable (GL_DEPTH_TEST);
    
    camera.display_begin();

    disp_m->display ();


    camera.display_end();

    TwDraw();

    glFlush();
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    camera.reshape(w,h);

    width = w;
    height = h;
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);

    TwWindowSize(w, h);

    glutPostRedisplay();
}

void mouse_click(int button, int state, int x, int y)
{
    if (TwEventMouseButtonGLUT(button, state, x, y))
    {
        glutPostRedisplay();
        return;
    }

    camera.mouse(button, state, x, y);

    glutPostRedisplay();
}

void mouse_move(int x, int y)
{
    if (TwEventMouseMotionGLUT(x, y))
    {
        glutPostRedisplay();
        return;
    }

    camera.mouse_move(x,y);
    glutPostRedisplay();
}

void keyboard_up(unsigned char k, int x, int y)
{
    // printf ("UP! %c\n", k);
    camera.key_up (k, x, y);

}


void keyboard(unsigned char k, int x, int y)
{
    if (k == __ESC_GLUT)
	quit();

    if (TwEventKeyboardGLUT(k, x, y))
	return;

    // printf ("DOWN! %c\n", k);
    camera.key (k, x, y);

    // if (k == 't')
    // 	test = !test;

    glutPostRedisplay();
}

void special(int k, int x, int y)
{
    if (!TwEventSpecialGLUT(k, x, y))
	camera.special (k, x, y);

    glutPostRedisplay();
}



//////////
// MAIN //
//////////



int main (int argc, char *argv[])
{
    if (argc != 2)
    {
	fprintf (stderr, "Usage: ./viewer datasets-directory\n");
	exit (-1);
    }

    data_m = new DataManager (argv[1]);
    
    for (int i = 0; i < data_m->datasets.size(); i++)
    {
	printf ("\nDataset: %s. Dir: %s\n", data_m->datasets[i]->name.c_str(), data_m->datasets[i]->dir.c_str());

	for (int j = 0; (data_m->datasets[i]->planes.size()) > j; j++)
	{
	    printf ("\tPlane: %s. Path: %s. Type: %s\n",
		    data_m->datasets[i]->planes[j]->filename.c_str(),
		    data_m->datasets[i]->planes[j]->pathname.c_str(),
		    (ds_type2string (data_m->datasets[i]->planes[j]->type)).c_str()
		    );
	}
    }
    string exename = get_base (argv[0]);



    Point a,b;
    data_m->getbb (&a, &b);
    camera.set (W, H, a, b);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W, H);
    glutCreateWindow("gggis");

    TwInit(TW_OPENGL, NULL);
    TwWindowSize(W, H);

    disp_m = new DisplayManager (data_m);

    glClearColor(0, 0, 0, 0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    //glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse_click);
    glutMotionFunc(mouse_move);
    glutPassiveMotionFunc(mouse_move);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboard_up);
    glutSpecialFunc(special);
    glutIgnoreKeyRepeat(1);
    TwGLUTModifiersFunc(glutGetModifiers);

    glutMainLoop();
    exit (-1);
}
