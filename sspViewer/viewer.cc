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

//////////
// QUIT //
//////////

void quit()
{
    exit(0);
}

void display()
{
    glClearColor(1.0f, 1.0f, 1.0f, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable (GL_DEPTH_TEST);
    
    camera.display_begin();

    td.draw ();

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


///////////////////////////
// ANTTWEAKBAR CALLBACKS //
///////////////////////////

void TW_CALL call_quit (void *clientData)
{ 
    (void) clientData;

    quit();
}

void TW_CALL setTrackBalance (const void *value, void *)
{
	td.weightBalance = 1.0f-(*(const double *) value);
}

void TW_CALL getTrackBalance (void *value, void *)
{
    *(double *) value = 1.0f-td.weightBalance;
}

void TW_CALL call_saveViewed (void *clientData)
{ 
    (void) clientData;
	td.save( "viewedSave.txt", true);
}

void TW_CALL call_saveAll (void *clientData)
{ 
    (void) clientData;
	td.save( "allSave.txt", false);
}

// Set Get - Window
// ******** GAUSS ********
void TW_CALL setGaussWindowFrom (const void *value, void *)
{
	td.gaussWFrom = *(const double *) value;
	if(td.gaussWTo <= td.gaussWFrom) td.gaussWTo = td.gaussWFrom + 0.01;
}
void TW_CALL getGaussWindowFrom (void *value, void *)
{
    *(double *) value = td.gaussWFrom;
}

void TW_CALL setGaussWindowTo (const void *value, void *)
{
	td.gaussWTo = *(const double *) value;
	if(td.gaussWFrom >= td.gaussWTo) td.gaussWFrom = td.gaussWTo - 0.01;
}
void TW_CALL getGaussWindowTo (void *value, void *)
{
    *(double *) value = td.gaussWTo;
}

// ******** BAUER ********
void TW_CALL setBauerWindowFrom (const void *value, void *)
{
	td.bauerWFrom = *(const double *) value;
	if(td.bauerWTo <= td.bauerWFrom) td.bauerWTo = td.bauerWFrom + 0.01;
}
void TW_CALL getBauerWindowFrom (void *value, void *)
{
    *(double *) value = td.bauerWFrom;
}

void TW_CALL setBauerWindowTo (const void *value, void *)
{
	td.bauerWTo = *(const double *) value;
	if(td.bauerWFrom >= td.bauerWTo) td.bauerWFrom = td.bauerWTo - 0.01;
}
void TW_CALL getBauerWindowTo (void *value, void *)
{
    *(double *) value = td.bauerWTo;
}


//////////
// MAIN //
//////////

int main (int argc, char *argv[])
{
    if (argc != 4){
		fprintf (stderr, "Usage: ./viewer gaussSSP gaussTRK bauerTRK\n");
		exit (-1);
    }

    td.loadTerrain(argv[1]);
	td.loadTracks(argv[2], argv[3]);

    Point a,b;
    td.getBounds(&a, &b);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W, H);
    glutCreateWindow("viewer");

    TwInit(TW_OPENGL, NULL);
    TwWindowSize(W, H);

    glClearColor(0, 0, 0, 0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
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

	camera.set (W, H, a, b);

    TwBar *cBar;
    cBar = TwNewBar("Camera_Rendering");
    TwDefine("Camera_Rendering size='280 350'");
    TwDefine("Camera_Rendering valueswidth=90");
    TwDefine("Camera_Rendering color='192 255 192' text=dark ");

    TwAddVarRW(cBar, "Show Terrain", TW_TYPE_BOOLCPP, &(td.draw_terrain), "");
    TwAddVarRW(cBar, "Show Criticals", TW_TYPE_BOOLCPP, &(td.draw_criticals), "");
    TwAddVarRW(cBar, "Show Boundary Criticals", TW_TYPE_BOOLCPP, &(td.draw_boundary_criticals), "");
    TwAddVarRW(cBar, "Show as Spheres", TW_TYPE_BOOLCPP, &(td.draw_as_spheres), "");
    TwAddVarRW(cBar, "Base Radius", TW_TYPE_DOUBLE, &(td.baseRadius), "min=0.0 max=5.0 step=0.1");
    TwAddVarRW(cBar, "Weighted Radius", TW_TYPE_DOUBLE, &(td.weightedRadius), "min=0.0 max=10.0 step=0.1");

    TwEnumVal draw_radiusPrintModes[3] = {{TrackDisplay::R_DEFAULT, "Default" }, {TrackDisplay::R_GAUSS, "As Gauss" }, {TrackDisplay::R_BAUER, "As Bauer" }};
    TwType radiusPrintModeChoosing = TwDefineEnum("RadiusPrintMode", draw_radiusPrintModes, 3);
    TwAddVarRW(cBar, "Radius print mode", radiusPrintModeChoosing, &(td.printMode), "");

    TwAddSeparator (cBar, 0, 0);
	TwAddVarCB(cBar, "Gauss Window (from %)", TW_TYPE_DOUBLE, setGaussWindowFrom, getGaussWindowFrom, NULL, "min=0.0 max=0.99 step=0.01");
	TwAddVarCB(cBar, "Gauss Window (to %)", TW_TYPE_DOUBLE, setGaussWindowTo, getGaussWindowTo, NULL, "min=0.01 max=1.0 step=0.01");
	TwAddVarCB(cBar, "Bauer Window (from %)", TW_TYPE_DOUBLE, setBauerWindowFrom, getBauerWindowFrom, NULL, "min=0.0 max=0.99 step=0.01");
	TwAddVarCB(cBar, "Bauer Window (to %)", TW_TYPE_DOUBLE, setBauerWindowTo, getBauerWindowTo, NULL, "min=0.01 max=1.0 step=0.01");

    TwAddSeparator (cBar, 0, 0);
    TwAddVarRW(cBar, "Weight Threshold (%)", TW_TYPE_DOUBLE, &(td.weightThreshold), "min=0.0 max=1.0 step=0.01");

    TwEnumVal criticalThresholdTypes[2] = {{TrackDisplay::ON_NUMBER, "Criticals Num." }, {TrackDisplay::ON_LIFE, "Criticals Life" }};
    TwType criticalsThresholdChoosing = TwDefineEnum("ThresholdWork", criticalThresholdTypes, 2);
    TwAddVarRW(cBar, "Threshold work on", criticalsThresholdChoosing, &(td.thresholdMode), "");

    TwEnumVal draw_weightTypeChoosingEV[3] = {{TrackDisplay::W_MIN, "Minimum" }, {TrackDisplay::W_MAX, "Maximum" }, {TrackDisplay::W_AVG, "W. Average" }};
    TwType weightTypeChoosing = TwDefineEnum("WeightType", draw_weightTypeChoosingEV, 3);
    TwAddVarRW(cBar, "Weight Type", weightTypeChoosing, &(td.choosedWeightType), "");

	TwAddVarCB(cBar, "Gauss Weight (%)", TW_TYPE_DOUBLE, setTrackBalance, getTrackBalance, NULL, "min=0.0 max=1.0 step=0.01");
	TwAddVarRO(cBar, "Bauer Weight (%)", TW_TYPE_DOUBLE, &(td.weightBalance), "");
    TwAddSeparator (cBar, 0, 0);

    TwAddButton(cBar, "Save Viewed", call_saveViewed, NULL, "");
    TwAddButton(cBar, "Save All", call_saveAll, NULL, "");

    TwAddSeparator (cBar, 0, 0);
    TwAddButton(cBar, "Quit", call_quit, NULL, "");

    glutMainLoop();
    exit (-1);
}
