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
#include "debug.h"


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


void TW_CALL BreakOnError(const char *errorMessage)
{ 
    if (errorMessage == NULL)
	assert (0); 
    else
    {
	eprintx (3, "%s\n", errorMessage);
    }
}


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

char *path = NULL;
DataOrganization org;

void print_help (FILE* f)
{
    fprintf (f, "Usage: smoother -m DIR | -s DIR | -p FILE\n"
	     "[-m DIR]: load multiple datasets from DIR.\n"
	     "[-s DIR]: load single dataset in DIR.\n"
	     "[-p FILE]: load single plane from FILE.\n"
	     );
}

void app_init(int argc, char *argv[])
{
    for (argc--, argv++; argc--; argv++)
    {
        if( (*argv)[0] == '-')
        {
            switch( (*argv)[1] )
            {
	    case 'h':
		print_help (stdout);
		exit (0);

	    case 'm':
		org = MULTIPLE;
                path = *++argv;
                argc--;
		break;

	    case 's':
		org = SINGLE;
                path = *++argv;
                argc--;
		break;

	    case 'p':
		org = PLANE;
                path = *++argv;
                argc--;
		break;

	    }
	}
    }

    if (path == NULL)
    {
	print_help (stderr);
	exit (-1);
    }
}


int main (int argc, char *argv[])
{
    app_init (argc, argv);

    data_m = new DataManager (path, org);
    
    if (_TRACE_TEST && SCOPE_BOUNDINGBOX)
    {
	BoundingBox lbb,wbb;
	for (unsigned i = 0; i < data_m->datasets.size(); i++)
	{
	    lbb = data_m->datasets[i]->getbb_local ();
	    wbb = data_m->datasets[i]->getbb_world ();
	    tprints (SCOPE_BOUNDINGBOX, "\nDataset: %s. Dir: %s. lbb((%lf,%lf),(%lf,%lf)) wbb((%lf,%lf),(%lf,%lf))\n",
		     data_m->datasets[i]->name.c_str(), data_m->datasets[i]->dir.c_str(),
		     lbb.a.x, lbb.a.y, lbb.b.x, lbb.b.y, wbb.a.x, wbb.a.y, wbb.b.x, wbb.b.y);

	    for (unsigned j = 0; (data_m->datasets[i]->planes.size()) > j; j++)
	    {
		lbb = data_m->datasets[i]->planes[j]->getbb ();
		tprints (SCOPE_BOUNDINGBOX, "\tPlane: %s. Path: %s. Type: %s. bb ((%lf,%lf),(%lf,%lf))\n",
			 data_m->datasets[i]->planes[j]->filename.c_str(),
			 data_m->datasets[i]->planes[j]->pathname.c_str(),
			 (ds_type2string (data_m->datasets[i]->planes[j]->type)).c_str(),
			 lbb.a.x, lbb.a.y, lbb.b.x, lbb.b.y
			 );
	    }
	}

    }

    string exename = get_base (argv[0]);

    BoundingBox bb = data_m->getbb ();

    tprints (SCOPE_BOUNDINGBOX, "FINAL BB: (%lf, %lf), (%lf, %lf)\n", bb.a.x, bb.a.y, bb.b.x, bb.b.y);

    camera.set (W, H, bb);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W, H);
    glutCreateWindow("gggis");

    TwInit(TW_OPENGL, NULL);
    TwWindowSize(W, H);
    TwHandleErrors(BreakOnError);

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
