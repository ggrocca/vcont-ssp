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

    // if (test)
    // 	td.draw_line_test (camera.diameter);

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

void TW_CALL do_query (void *)
{ 
    td.query (time_value);
}

void TW_CALL do_elixir (void *)
{ 
    td.track->drink_elixir ();
}

void TW_CALL do_init_spots (void *)
{ 
    td.init_spots ();
}

void TW_CALL call_quit (void *clientData)
{ 
    (void) clientData;

    quit();
}

//////////
// MAIN //
//////////

// void read_dems (char* name, int num)
// {
// #define __FNLEN 512
//     char dem_in[__FNLEN] = {'\0'};
//     snprintf (dem_in, __FNLEN, "%s-base.%s" , name, "tif");
//     td.read_dem (dem_in);
    
//     for (int i = 0; i < num; i++)
//     {
// 	snprintf (dem_in, __FNLEN, "%s-%d.%s" , name, i, "tif");
// 	td.read_dem (dem_in);
//     }
// #undef __FNLEN
// }

int main (int argc, char *argv[])
{
    if (argc != 3)
    {
	fprintf (stderr, "Usage: ./viewer file.track file.ssp\n");
	// fprintf (stderr, "  brutal hack: demname-base.tif must exist. "
	// 	 "then load demname-0.tif ... demname-N.tif\n");
	exit (-1);
    }

    td.read_track (argv[1]);
    // int dem_num;
    // read_dems (argv[2], dem_num = atoi (argv[3]));
    td.read_ssp (argv[2]);
    int dem_num = td.ssp->levels;

    // m.bb();
    // m.initL();

    // double cx, cy, diam;
    // td.getbb (&cx, &cy, &diam);
    // camera.reset (cx, cy, diam);
    Point a,b;
    td.getbb (&a, &b);
    // camera.set_bb (a, b);
    // camera.reshape (W, H);
    
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
    glutKeyboardUpFunc(keyboard_up);
    glutSpecialFunc(special);
    glutIgnoreKeyRepeat(1);
    TwGLUTModifiersFunc(glutGetModifiers);

    TwBar *cBar;
    cBar = TwNewBar("Camera_Rendering");
    TwDefine("Camera_Rendering size='250 350'");
    TwDefine("Camera_Rendering valueswidth=80");
    TwDefine("Camera_Rendering color='192 255 192' text=dark ");

    TwAddButton(cBar, "quit", call_quit, NULL, "");

	camera.set (W, H, a, b);

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
	       /*"min=0.0 max=65536.0 step=10.0"*/"step=1.0");
    TwAddVarRW(cBar, "clip_white", TW_TYPE_DOUBLE, &(td.clip_white),
	       /*"min=0.0 max=65536.0 step=10.0"*/"step=1.0");
    td.multiply = 1.0;
    TwAddVarRW(cBar, "mult factor", TW_TYPE_DOUBLE, &td.multiply, "min=1.0 "/*max=256.0*/" step=1.0");

    TwAddSeparator (cBar, 0, 0);
    TwAddSeparator (cBar, 0, 0);

    // TwAddVarRW(cBar, "show tracking", TW_TYPE_BOOLCPP, &(td.draw_track), "");
    // // TwAddVarRW(cBar, "scale criticals coarse", TW_TYPE_DOUBLE,
    // // 	       &(td.scale_criticals), "min=0.1 max=100.00 step=0.1");
    // TwAddVarRW(cBar, "track scale", TW_TYPE_DOUBLE,
    // 	       &(td.track_scale), "min=0.001 max=100.00 step=0.001");
    // TwAddVarRW(cBar, "track life mult", TW_TYPE_DOUBLE, &(td.track_mult),
    // 	       "min=0.0 max=100.00 step=0.05");

    // TwAddSeparator (cBar, 0, 0);

    // TwAddVarRW(cBar, "show query", TW_TYPE_BOOLCPP, &(td.draw_query), "");
    // TwAddVarCB(cBar, "query scale value", TW_TYPE_DOUBLE,
    // 	       setScaleValue, getScaleValue,
    // 	       NULL, "min=1.0000 max=4096.0000 step=1.0000");
    // TwAddVarCB(cBar, "query time value", TW_TYPE_DOUBLE,
    // 	       setTimeValue, getTimeValue,
    // 	       NULL, "min=0.0000 max=64.0000 step=0.25");
    // // TwAddVarRW(cBar, "query_value", TW_TYPE_DOUBLE, &(query_value),
    // // 	       "min=0.0000 max=20.0000 step=0.2500");
    // TwAddButton(cBar, "DO QUERY", do_query, NULL, "");
    // TwAddVarRW(cBar, "query scale", TW_TYPE_DOUBLE,
    // 	       &(td.query_scale), "min=0.001 max=100.00 step=0.001");
    // TwAddVarRW(cBar, "query life mult", TW_TYPE_DOUBLE, &(td.query_mult),
    // 	       "min=0.0 max=100.00 step=0.05");
    // TwAddVarRW(cBar, "draw current pos", TW_TYPE_BOOLCPP, &(td.query_cur_pos), "");
    // TwAddVarRW(cBar, "draw death point", TW_TYPE_BOOLCPP, &(td.query_death), "");

    // TwAddSeparator (cBar, 0, 0);


    // GG use statistics
    
    TwAddVarRW(cBar, "show spots", TW_TYPE_BOOLCPP, &(td.draw_spots), "");
    TwAddButton(cBar, "DO INIT_SPOTS", do_init_spots, NULL, "");
    TwAddVarRW(cBar, "spot scale", TW_TYPE_DOUBLE,
	       &(td.spot_scale), "min=0.001 max=100.00 step=0.001");
    // TwAddVarRW(cBar, "spot elixir life mult", TW_TYPE_DOUBLE, &(td.elixir_mult),
    // 	       "min=0.0 max=100.00 step=0.05");

    TwAddVarRW(cBar, "show final selection", TW_TYPE_BOOLCPP, &(td.draw_final), "");
    TwAddVarRW(cBar, "show always selection", TW_TYPE_BOOLCPP, &(td.draw_always_selected), "");
    TwAddVarRW(cBar, "show density selection", TW_TYPE_BOOLCPP, &(td.draw_density_selected), "");
    TwAddVarRW(cBar, "show density pool", TW_TYPE_BOOLCPP, &(td.draw_density_pool), "");

    TwAddButton(cBar, "maxima", NULL, NULL, "");
    TwAddVarRO(cBar, "^ total", TW_TYPE_INT32, &(td.maxima_total_num), "");
    TwAddVarRW(cBar, "^ always add", TW_TYPE_INT32, &(td.maxima_always_selected_num),
	       "min=0 step=1");
    TwAddVarRO(cBar, "^ always life", TW_TYPE_DOUBLE, &(td.maxima_always_life), "");
    TwAddVarRW(cBar, "^ pool add", TW_TYPE_INT32, &(td.maxima_density_pool_num),
	       "min=0 step=1");
    TwAddVarRO(cBar, "^ pool life", TW_TYPE_DOUBLE, &(td.maxima_excluded_life), "");
    TwAddVarRO(cBar, "^ pool survived", TW_TYPE_INT32, &(td.maxima_density_selected_num), "");
    TwAddVarRW(cBar, "^ importance cut", TW_TYPE_DOUBLE, &(td.spots_maxima_imp_cut),
	       "min=0.0 step=0.0000001");
    TwAddVarRO(cBar, "^ always discarded", TW_TYPE_INT32, &(td.maxima_always_discarded_num), "");
    TwAddVarRO(cBar, "^ importance discarded", TW_TYPE_INT32, &(td.maxima_importance_discarded_num), "");


    TwAddButton(cBar, "minima", NULL, NULL, "");
    TwAddVarRO(cBar, ". total", TW_TYPE_INT32, &(td.minima_total_num), "");
    TwAddVarRW(cBar, ". always add", TW_TYPE_INT32, &(td.minima_always_selected_num),
	       "min=0 step=1");
    TwAddVarRO(cBar, ". always life", TW_TYPE_DOUBLE, &(td.minima_always_life), "");
    TwAddVarRW(cBar, ". pool add", TW_TYPE_INT32, &(td.minima_density_pool_num),
	       "min=0 step=1");
    TwAddVarRO(cBar, ". pool life", TW_TYPE_DOUBLE, &(td.minima_excluded_life), "");
    TwAddVarRO(cBar, ". pool survived", TW_TYPE_INT32, &(td.minima_density_selected_num), "");
    TwAddVarRW(cBar, ". importance cut", TW_TYPE_DOUBLE, &(td.spots_minima_imp_cut),
	       "min=0.0 step=0.0000001");
    TwAddVarRO(cBar, ". always discarded", TW_TYPE_INT32, &(td.minima_always_discarded_num), "");
    TwAddVarRO(cBar, ". importance discarded", TW_TYPE_INT32, &(td.minima_importance_discarded_num), "");
    
    TwAddButton(cBar, "sellae", NULL, NULL, "");
    TwAddVarRO(cBar, "+ total", TW_TYPE_INT32, &(td.sellae_total_num), "");
    TwAddVarRW(cBar, "+ always add", TW_TYPE_INT32, &(td.sellae_always_selected_num),
	       "min=0 step=1");
    TwAddVarRO(cBar, "+ always life", TW_TYPE_DOUBLE, &(td.sellae_always_life), "");
    TwAddVarRW(cBar, "+ pool add", TW_TYPE_INT32, &(td.sellae_density_pool_num),
	       "min=0 step=1");
    TwAddVarRO(cBar, "+ pool life", TW_TYPE_DOUBLE, &(td.sellae_excluded_life), "");
    TwAddVarRO(cBar, "+ pool survived", TW_TYPE_INT32, &(td.sellae_density_selected_num), "");    
    TwAddVarRW(cBar, "+ importance cut", TW_TYPE_DOUBLE, &(td.spots_sellae_imp_cut),
	       "min=0.0 step=0.0000001");
    TwAddVarRO(cBar, "+ always discarded", TW_TYPE_INT32, &(td.sellae_always_discarded_num), "");
    TwAddVarRO(cBar, "+ importance discarded", TW_TYPE_INT32, &(td.sellae_importance_discarded_num), "");
    
    // TwAddVarRW(cBar, "density num", TW_TYPE_INT32, &(td.density_maxima_num),
    // 	       "min=0 step=1");
    TwAddVarRW(cBar, "density val", TW_TYPE_DOUBLE, &(td.density_maxima_val),
	       "min=0.5 max=3000.0 step=0.5");



    TwAddSeparator (cBar, 0, 0);
    TwAddSeparator (cBar, 0, 0);
    TwAddSeparator (cBar, 0, 0);



    TwAddVarRW(cBar, "show life", TW_TYPE_BOOLCPP, &(td.draw_elixir), "");
    TwAddVarRW(cBar, "draw normal life", TW_TYPE_BOOLCPP, &(td.normal_lives), "");
    TwAddButton(cBar, "DO LIFE", do_elixir, NULL, "");
    TwAddVarRO(cBar, "intoxicated", TW_TYPE_INT32, &(td.track->intoxicated), "");
    TwAddVarRW(cBar, "life scale", TW_TYPE_DOUBLE,
	       &(td.elixir_scale), "min=0.001 max=100.00 step=0.001");
    TwAddVarRW(cBar, "life life mult", TW_TYPE_DOUBLE, &(td.elixir_mult),
	       "min=0.0 max=100.00 step=0.05");
    TwAddVarRW(cBar, "life cut strong", TW_TYPE_DOUBLE, &(td.elixir_cut),
	       "min=0.0 max=100.00 step=0.1");
    TwAddVarRW(cBar, "life cut fine", TW_TYPE_DOUBLE, &(td.elixir_cut),
	       "min=0.0 max=100.00 step=0.01");
    TwAddVarRW(cBar, "life cut micro", TW_TYPE_DOUBLE, &(td.elixir_cut),
	       "min=0.0 max=100.00 step=0.001");

    TwAddSeparator (cBar, 0, 0);

    TwAddVarRW(cBar, "show importance", TW_TYPE_BOOLCPP, &(td.draw_importance), "");
    TwAddVarRW(cBar, "importance scale", TW_TYPE_DOUBLE,
	       &(td.importance_scale), "min=0.001 max=100.00 step=0.001");
    TwAddVarRW(cBar, "importance mult", TW_TYPE_DOUBLE, &(td.importance_mult),
	       "min=0.0 max=100.00 step=0.005");
    TwAddVarRW(cBar, "importance cut strong", TW_TYPE_DOUBLE, &(td.importance_cut),
	       "min=0.0 step=0.1");
    TwAddVarRW(cBar, "importance cut fine", TW_TYPE_DOUBLE, &(td.importance_cut),
	       "min=0.0 step=0.01");
    TwAddVarRW(cBar, "importance cut micro", TW_TYPE_DOUBLE, &(td.importance_cut),
	       "min=0.0 step=0.000001");

    TwAddSeparator (cBar, 0, 0);


    
    // TwAddSeparator (cBar, 0, 0);

    // TwAddVarRW(cBar, "show lines", TW_TYPE_BOOLCPP, &(td.draw_lines), "");
    // TwAddVarRW(cBar, "lines width", TW_TYPE_DOUBLE, &(td.lines_width),
    // 	       "min=0.1 max=100.00 step=0.1");
    // TwAddVarRW(cBar, "lines size clip", TW_TYPE_UINT32, &(td.lines_size_clip),
    // 	       "min=0 max=1000 step=1");
    // TwAddVarRW(cBar, "lines life clip", TW_TYPE_DOUBLE, &(td.lines_life_clip),
    // 	       "min=0.0 max=100.00 step=0.1");
    // TwAddVarRW(cBar, "lines query", TW_TYPE_BOOLCPP, &(td.lines_query), "");
    // // // focus distance
    // // TwAddVarCB(cBar, "camera focus", TW_TYPE_DOUBLE, setFocalLength, getFocalLength,
    // // 	       NULL, "min=0.00 max=100.00 step=0.1");

    glutMainLoop();
    exit (-1);
}
