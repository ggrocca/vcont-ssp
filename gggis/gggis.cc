#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#define __ESC_GLUT 27
#include <AntTweakBar.h>

#include <sstream>

#include "datamanager.hh"
#include "../common/utils.hh"

#define W 1260
#define H 760


DataManager* dm;





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

// bool test = false;
void display()
{
    glClearColor(1.0f, 1.0f, 1.0f, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable (GL_DEPTH_TEST);
    
    // camera.display_begin();

    // // td.draw(mesh_draw_mode);
    // td.draw (draw_dem_idx+1);

    // if (test)
    // 	td.draw_line_test (camera.diameter);

    // camera.display_end();

    TwDraw();

    glFlush();
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    //     camera.reshape(w,h);

    //     width = w;
    //     height = h;
    //     glViewport(0, 0, (GLsizei) w, (GLsizei) h);

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

    // camera.mouse(button, state, x, y);

    // glutPostRedisplay();
}

void mouse_move(int x, int y)
{
    if (TwEventMouseMotionGLUT(x, y))
    {
        glutPostRedisplay();
        return;
    }

    // camera.mouse_move(x,y);
    // glutPostRedisplay();
}

void keyboard(unsigned char k, int x, int y)
{
    if (k == __ESC_GLUT)
	quit();

    // if (!TwEventKeyboardGLUT(k, x, y))
    // 	camera.key (k, x, y);

    // if (k == 't')
    // 	test = !test;

    glutPostRedisplay();
}

void special(int k, int x, int y)
{
    // if (!TwEventSpecialGLUT(k, x, y))
    // 	camera.special (k, x, y);

    glutPostRedisplay();
}



int bar_num = 0;

void TW_CALL set_bar_num (const void *value, void *)
{
    int new_bar_num = *(const int *) value;

    printf ("BBBBBBBBBBB %d\n", new_bar_num);
   
    std::stringstream name;
    name << "Plane_" << bar_num;
    TwDefine ((name.str() + " visible=false").c_str());
 
    std::stringstream new_name;
    new_name << "Plane_" << new_bar_num;
    TwDefine ((new_name.str() + " visible=true").c_str());
 
    bar_num = new_bar_num;

    glutPostRedisplay();
}

void TW_CALL get_bar_num (void *value, void *)
{
    *(int *) value = bar_num;
}



int main (int argc, char *argv[])
{
    if (argc != 2)
    {
	fprintf (stderr, "Usage: ./viewer trackfile demnames N\n");
	exit (-1);
    }

    dm = new DataManager (argv[1]);
    
    for (int i = 0; i < dm->datasets.size(); i++)
    {
	printf ("\nDataset: %s. Dir: %s\n", dm->datasets[i]->name.c_str(), dm->datasets[i]->dir.c_str());

	for (int j = 0; (dm->datasets[i]->planes.size()) > j; j++)
	{
	    printf ("\tPlane: %s. Path: %s. Type: %s\n",
		    dm->datasets[i]->planes[j]->filename.c_str(),
		    dm->datasets[i]->planes[j]->pathname.c_str(),
		    (ds_type2string (dm->datasets[i]->planes[j]->type)).c_str()
		    );
	}
    }
    string exename = get_base (argv[0]);

    int planes_num = dm->datasets[0]->planes.size();


    // double cx, cy, diam;
    // td.getbb (&cx, &cy, &diam);
    // camera.reset (cx, cy, diam);
    // camera.reshape (W, H);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W, H);
    glutCreateWindow(exename.c_str());

    TwInit(TW_OPENGL, NULL);
    TwWindowSize(W, H);

    glClearColor(0, 0, 0, 0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    //glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    // camera.SetLighting(4);
    // camera.gCameraReset(m.diagonal, m.center);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse_click);
    glutMotionFunc(mouse_move);
    glutPassiveMotionFunc(mouse_move);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    TwGLUTModifiersFunc(glutGetModifiers);

    TwBar* cBar;

    cBar = TwNewBar("Controller");
    TwDefine("Controller size='250 350'");
    TwDefine("Controller valueswidth=80");
    TwDefine("Controller color='192 255 192' text=dark ");

    vector<TwBar*> bars;

    for (int i = 0; i < planes_num; i++)
    {
	std::stringstream name;
	name << "Plane_" << i;

	bars.push_back (TwNewBar(name.str().c_str()));

	if (i != 0)
	    TwDefine ((name.str() + " visible=false").c_str());
    }



#define __FNLEN 512
    char plane_num_bounds[__FNLEN] = {'\0'};
    snprintf (plane_num_bounds, __FNLEN, "min=0 max=%d step=1 "
	      "keydecr='<' keyincr='>'", planes_num-1);
    TwAddVarCB(cBar, "select plane", TW_TYPE_INT32,
    	       set_bar_num, get_bar_num,
    	       NULL, plane_num_bounds);
#undef __FNLEN



    glutMainLoop();
    exit (-1);


}
