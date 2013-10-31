// opengl
// #include <GL/glew.h>
#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif
#define __ESC_GLUT 27

#include <AntTweakBar.h>

#include "camera.hh"
#include "trackdisplay.hh"
#include "scalespace.hh"

/////////////
// GLOBALS //
/////////////

#define W 1260
#define H 760

Camera camera;
TrackDisplay td;

int width = W;
int height = H;

int draw_dem_idx = -1;
// VF::draw_mode_t mesh_draw_mode = VF::FLAT;


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

    // td.draw(mesh_draw_mode);
    td.draw (draw_dem_idx+1);

    if (test)
	td.draw_line_test (camera.diameter);

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

void keyboard(unsigned char k, int x, int y)
{
    if (k == __ESC_GLUT)
	quit();

    if (!TwEventKeyboardGLUT(k, x, y))
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


///////////////////////////
// ANTTWEAKBAR CALLBACKS //
///////////////////////////

double scale_value = 0.0;
double time_value = 0.0;

void TW_CALL setScaleValue (const void *value, void *)
{
    scale_value = *(const double *) value;
    time_value = ScaleSpace::scale2time (scale_value);
    glutPostRedisplay();
}

void TW_CALL getScaleValue (void *value, void *)
{
    *(double *) value = scale_value;
}

void TW_CALL setTimeValue (const void *value, void *)
{
    time_value = *(const double *) value;
    scale_value = ScaleSpace::time2scale (time_value);
    glutPostRedisplay();
}

void TW_CALL getTimeValue (void *value, void *)
{
    *(double *) value = time_value;
}

void TW_CALL do_query(void *)
{ 
    td.query (time_value);
}

void TW_CALL call_quit(void *clientData)
{ 
    (void) clientData;

    quit();
}

//////////
// MAIN //
//////////

void read_dems (char* name, int num)
{
#define __FNLEN 512
    char dem_in[__FNLEN] = {'\0'};
    snprintf (dem_in, __FNLEN, "%s-base.%s" , name, "tif");
    td.read_dem (dem_in);
    
    for (int i = 0; i < num; i++)
    {
	snprintf (dem_in, __FNLEN, "%s-%d.%s" , name, i, "tif");
	td.read_dem (dem_in);
    }
#undef __FNLEN
}

int main (int argc, char *argv[])
{
    if (argc != 4)
    {
	fprintf (stderr, "Usage: ./viewer trackfile demnames N\n");
	fprintf (stderr, "  brutal hack: demname-base.tif must exist. "
		 "then load demname-0.tif ... demname-N.tif\n");
	exit (-1);
    }

    td.read_track (argv[1]);
    int dem_num;
    read_dems (argv[2], dem_num = atoi (argv[3]));
    // m.bb();
    // m.initL();

    double cx, cy, diam;
    td.getbb (&cx, &cy, &diam);
    camera.reset (cx, cy, diam);
    camera.reshape (W, H);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W, H);
    glutCreateWindow("viewer");

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

    TwBar *cBar;
    cBar = TwNewBar("Camera_Rendering");
    TwDefine("Camera_Rendering size='250 350'");
    TwDefine("Camera_Rendering valueswidth=80");
    TwDefine("Camera_Rendering color='192 255 192' text=dark ");

    TwAddButton(cBar, "quit", call_quit, NULL, "");

    // TwEnumVal draw_modeEV[VF_DRAW_MODE_NUM] = {
    //     { VF::FLAT, "Flat" }, { VF::POINTS, "Points" }, { VF::WIRE, "Wire" } }; 
    //     // { VF::HIDDEN, "Hidden" }, { VF::FLAT, "Flat" }};
    // TwType draw_modeT = TwDefineEnum("drawModeType", draw_modeEV, VF_DRAW_MODE_NUM);
    // TwAddVarRW(cBar, "DrawMode", draw_modeT, &mesh_draw_mode,
    // 	       " keyIncr='<' keyDecr='>'"); 

    TwAddVarRW(cBar, "show terrain", TW_TYPE_BOOLCPP, &(td.draw_terrain), "");
#define __FNLEN 512
    char dem_num_bounds[__FNLEN] = {'\0'};
    snprintf (dem_num_bounds, __FNLEN, "min=-1 max=%d step=1 "
	      "keydecr='<' keyincr='>'", dem_num-1);
    TwAddVarRW(cBar, "dem index", TW_TYPE_INT32, &draw_dem_idx, dem_num_bounds);
#undef __FNLEN
    TwAddVarRW(cBar, "clip_black", TW_TYPE_DOUBLE, &(td.clip_black),
	       "min=0.0 max=65536.0 step=10.0");
    TwAddVarRW(cBar, "clip_white", TW_TYPE_DOUBLE, &(td.clip_white),
	       "min=0.0 max=65536.0 step=10.0");

    TwAddSeparator (cBar, 0, 0);

    TwAddVarRW(cBar, "show tracking", TW_TYPE_BOOLCPP, &(td.draw_track), "");
    // TwAddVarRW(cBar, "scale criticals coarse", TW_TYPE_DOUBLE,
    // 	       &(td.scale_criticals), "min=0.1 max=100.00 step=0.1");
    TwAddVarRW(cBar, "track scale", TW_TYPE_DOUBLE,
	       &(td.track_scale), "min=0.001 max=100.00 step=0.001");
    TwAddVarRW(cBar, "track life mult", TW_TYPE_DOUBLE, &(td.track_mult),
	       "min=0.0 max=100.00 step=0.05");

    TwAddSeparator (cBar, 0, 0);

    TwAddVarRW(cBar, "show query", TW_TYPE_BOOLCPP, &(td.draw_query), "");
    TwAddVarCB(cBar, "query scale value", TW_TYPE_DOUBLE,
    	       setScaleValue, getScaleValue,
    	       NULL, "min=1.0000 max=4096.0000 step=1.0000");
    TwAddVarCB(cBar, "query time value", TW_TYPE_DOUBLE,
    	       setTimeValue, getTimeValue,
    	       NULL, "min=0.0000 max=64.0000 step=0.25");
    // TwAddVarRW(cBar, "query_value", TW_TYPE_DOUBLE, &(query_value),
    // 	       "min=0.0000 max=20.0000 step=0.2500");
    TwAddButton(cBar, "DO QUERY", do_query, NULL, "");
    TwAddVarRW(cBar, "query scale", TW_TYPE_DOUBLE,
	       &(td.query_scale), "min=0.001 max=100.00 step=0.001");
    TwAddVarRW(cBar, "query life mult", TW_TYPE_DOUBLE, &(td.query_mult),
	       "min=0.0 max=100.00 step=0.05");
    TwAddVarRW(cBar, "draw current pos", TW_TYPE_BOOLCPP, &(td.query_cur_pos), "");
    TwAddVarRW(cBar, "draw death point", TW_TYPE_BOOLCPP, &(td.query_death), "");

    TwAddSeparator (cBar, 0, 0);

    TwAddVarRW(cBar, "show lines", TW_TYPE_BOOLCPP, &(td.draw_lines), "");
    TwAddVarRW(cBar, "lines width", TW_TYPE_DOUBLE, &(td.lines_width),
	       "min=0.1 max=100.00 step=0.1");
    TwAddVarRW(cBar, "lines size clip", TW_TYPE_UINT32, &(td.lines_size_clip),
	       "min=0 max=1000 step=1");
    TwAddVarRW(cBar, "lines life clip", TW_TYPE_DOUBLE, &(td.lines_life_clip),
	       "min=0.0 max=100.00 step=0.1");
    TwAddVarRW(cBar, "lines query", TW_TYPE_BOOLCPP, &(td.lines_query), "");
    // // focus distance
    // TwAddVarCB(cBar, "camera focus", TW_TYPE_DOUBLE, setFocalLength, getFocalLength,
    // 	       NULL, "min=0.00 max=100.00 step=0.1");

    glutMainLoop();
    exit (-1);
}