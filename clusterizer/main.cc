#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "topologyIndex.hh"
#include <fstream>

//TODO togliere
#include <algorithm>

#define IGL_HEADER_ONLY
#include <igl/read.h>

//#include "visualizer.hh"
//#include "benchmark.hh"
#include "miniBall.hh"
/*#include "glut_trackball.hh"
#ifdef MACOS
#include <glut.h>
#else
#include <GL/glut.h>
#endif
#include <AntTweakBar.h>

#define W 800
#define H 600

Visualizer *visualizer;*/
//Benchmark *benchmark;
int tw_my_var = 0;
char* meshfile = 0;
float s1=2.0,s2=80.0,s3=1.0, queryRadiusFactor=10.0;
int int1=0, fusion_base=2, fusion_factor=2, pruneLevel=2, numberOfLevels=3;
bool start_gui = false;
bool start_benchmark = false;
char* benchfile;

// void display()
// {
//     visualizer->paintGL();
//     TwDraw();

//     glFlush();
//     glutSwapBuffers();
// }

// void reshape(int w, int h)
// {
//     visualizer->resizeGL(w, h);
//     TwWindowSize(w, h);
//     glutPostRedisplay();
// }

// void mouse_click(int button, int state, int x, int y)
// {
//     if (TwEventMouseButtonGLUT(button, state, x, y))
//     {
//         glutPostRedisplay();
//         return;
//     }

//     if ((button == WHEEL_UP || button == WHEEL_DOWN) && state == GLUT_DOWN)
//     {
//         visualizer->mouseWheel((button == WHEEL_UP) ? 1 : -1, GLUT2VCG(button));
//         return;
//     }

//     if (state == GLUT_DOWN) visualizer->mousePress(x, y, GLUT2VCG(button));

//     if (state == GLUT_UP) visualizer->mouseRelease(x, y, GLUT2VCG(button));

//     glutPostRedisplay();
// }

// void mouse_move(int x, int y)
// {
//     if (TwEventMouseMotionGLUT(x, y))
//     {
//         glutPostRedisplay();
//         return;
//     }

//     visualizer->mouseMove(x, y);

//     glutPostRedisplay();
// }

// void keyboard(unsigned char k, int x, int y)
// {
//     TwEventKeyboardGLUT(k, x, y);
//     glutPostRedisplay();
// }

// void special(int k, int x, int y)
// {
//     TwEventSpecialGLUT(k, x, y);
//     glutPostRedisplay();
// }

// void TW_CALL rebuildIndex(void *)
// {
//     visualizer->rebuildIndex();
// }

// void TW_CALL doTopologicQuery(void *)
// {
//     visualizer->launchSphereBreadthQuery();
// }

// void TW_CALL doSpaceQuery(void *)
// {
//     visualizer->launchSphereSpaceQuery();
// }

// void TW_CALL doClusterQuery(void *) //
// {
//     visualizer->launchClusterQuery();
// }


// void TW_CALL setFactor(const void *value, void *)
// {
//     visualizer->setSphereRadius(*(const float *) value);
// }

// void TW_CALL getFactor(void *value, void *)
// {
//     *(float *) value = visualizer->scaledSphereRadius;
// }

/*void TW_CALL doBenchmark(void *)
{
    visualizer->launchBenchmark();
    }*/


void print_help ()
{
    fprintf (stderr, "Usage: clusterizer -g|-b  -m input_file [-o output_file]\n"
             "\t\t [-r radius_start radius_stop radius_step] [-p prune_level] [-f base exp]\n"
	     "\t\t [-q query_factor] [-n level_number] [-v vertex_number]\n");
}


void app_init(int argc, char *argv[])
{
    for (argc--, argv++; argc--; argv++)
    {
        if( (*argv)[0] == '-')
        {
            switch( (*argv)[1] )
            {
            case 'm':
                meshfile = (*++argv);
                argc--;
                break;

            case 'r':
                s1 = atof (*++argv);
                s2 = atof (*++argv);
                s3 = atof (*++argv);
                argc--;
                argc--;
                argc--;
                break;

            case 'v':
                int1 = atoi (*++argv);
                argc--;
                break;

            case 'g':
                start_gui = true;
                break;

            case 'b':
                start_benchmark = true;
                break;

            case 'o':
                benchfile = (*++argv);
                argc--;
                break;

            case 'h':
                print_help();
                exit(0);

            case 'f':
                fusion_base = atoi(*++argv);
                fusion_factor = atoi(*++argv);
                argc--;
                argc--;
                break;

            case 'p':
                pruneLevel = atoi(*++argv);
                argc--;
                break;

            case 'q':
                queryRadiusFactor = atof(*++argv);
                argc--;
                break;

            case 'n':
                numberOfLevels = atoi(*++argv);
                argc--;
                break;

            default:
                goto die;
            }
        }
    }

    if (!meshfile)
	goto die;
    
    /*if (start_gui && start_benchmark)
	goto die;
    
    if (!start_gui && !start_benchmark)
    goto die;*/
    
    return;

    die:
    print_help();
    exit(1);
}


//int main_gui(int argc, char *argv[]);
int main_benchmark();

//TODO rimuovere: stampa mesh con attributo 1 su vv e 0 altrove.
void testFunc(Eigen::MatrixXd V, Eigen::MatrixXi F, std::vector<int> vv)
{
  std::sort(vv.begin(),vv.end());
  std::ofstream of("outres.ply");
  of <<"ply\nformat ascii 1.0\nelement vertex " << V.rows() << "\nproperty float x\nproperty float y\nproperty float z\n";
  of <<"property float quality\nelement face " << F.rows() << "\nproperty list uchar int vertex_indices\nend_header\n";

  int indexVV=0;
  for (int i=0; i<V.rows(); i++)
    {
      of << V.row(i)[0] << " " << V.row(i)[1] << " " << V.row(i)[2] << " ";
      if (i==vv[indexVV])
	{
	  of << "1\n";
	  indexVV++;
	}
      else of << "0\n";
    }
  for (int i=0; i<F.rows(); i++)
    {
      of << "3 " << F.row(i)[0] << " " << F.row(i)[1] << " " << F.row(i)[2] << "\n";
    }
}


double getAverageEdge(Eigen::MatrixXd vertices, Eigen::MatrixXi faces)
{
  double sum = 0;
  int count = 0;
  
  for (int i = 0; i<faces.rows(); i++)
  {
    for (short unsigned j=0; j<3; j++)
    {
      Eigen::Vector3d p1=vertices.row(faces.row(i)[j]);
      Eigen::Vector3d p2=vertices.row(faces.row(i)[(j+1)%3]);
      
      double l = (p1-p2).norm();
      
      sum+=l;
      ++count;
    }
  }
  
  return (sum/(double)count);
}


int main(int argc, char *argv[])
{
    app_init (argc, argv);

    std::cout << "Vertex to pick ? " << std::endl;

    int i;
    std::cin >> i;

    std::cout << "Sphere radius? " << std::endl;

    float s;
    std::cin >> s;

    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    
    igl::read(meshfile,V,F);
    TopologyIndex* topology_index = new TopologyIndex(V,F,4, 2,2, true, true,getAverageEdge(V,F) , false, 2, 3); //
    topology_index->gg_print_stats(meshfile);
    std::vector<int> * vv = new std::vector<int>;
    double ss=s*getAverageEdge(V,F);

    std::cout << "SS che passo: " << ss <<"\n";
    int n_false=0;
    for (int k=0; k<V.rows(); k++)
    {
      int l=topology_index->query((int)k,ss,vv);
      topology_index->getLevel(l)->markQuery(false);
      if (vv->size()<6)
	std::cerr << " vv : " << vv->size() << "\n";
    }

    //  testFunc(V,F,*vv);
    /*  if (start_gui)
	return main_gui (argc, argv);*/

    /*    if (start_benchmark)
	  return main_benchmark();*/
}

/*int main_benchmark()
{
    benchmark = new Benchmark(benchfile);
    benchmark->sphere_begin=s1;
    benchmark->sphere_end=s2;
    benchmark->sphere_step=s3;
    benchmark->intpick=int1;
    printf("Fusion base: %d Fusion exp: %d Query radius: %f Prune level: %d Sphere start: %f Sphere stop: %f Sphere step: %f High prune level: %d Picked vertex: %d\n",fusion_base,fusion_factor,queryRadiusFactor,pruneLevel,s1,s2,s3,numberOfLevels,int1);
    if (fusion_base == 0)
        benchmark->loadMesh(meshfile,0,0,pruneLevel,false, numberOfLevels);
    else
    {
        benchmark->loadMesh(meshfile,fusion_base,fusion_factor,pruneLevel,true,numberOfLevels);
    }
    benchmark->topology_index->queryRadiusFactor=queryRadiusFactor;
    benchmark->launchBenchmark();
    return 0;
}
*/
 /*int main_gui(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(W, H);
    glutCreateWindow("visualizer");

    visualizer = new Visualizer(W, H);
    visualizer->loadMesh(meshfile);

    visualizer->initializeGL();

    visualizer->meshEnabled = true;

    TwInit(TW_OPENGL, NULL);
    TwWindowSize(W, H);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse_click);
    glutMotionFunc(mouse_move);
    glutPassiveMotionFunc(mouse_move);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    TwGLUTModifiersFunc(glutGetModifiers);

    TwBar *cBar;
    cBar = TwNewBar("Clusterizer");
    TwDefine("Clusterizer size='300 400'");
    TwDefine("Clusterizer valueswidth=150");
    TwDefine("Clusterizer color='192 255 192' text=dark ");

    TwAddVarRW(cBar, "mesh", TW_TYPE_BOOLCPP, &visualizer->meshEnabled, "");

    TwEnumVal draw_modeEV[DRAW_MODE_NUM] = {
        { Visualizer::SMOOTH, "Smooth" }, { Visualizer::POINTS, "Points" },
        { Visualizer::WIRE, "Wire" }, { Visualizer::FLATWIRE, "Flatwire" },
        { Visualizer::HIDDEN, "Hidden" }, { Visualizer::FLAT, "Flat" }};
    TwType draw_modeT = TwDefineEnum("drawModeType", draw_modeEV, DRAW_MODE_NUM);
    TwAddVarRW(cBar, "DrawMode", draw_modeT, &visualizer->drawMode, " keyIncr='<' keyDecr='>'"); 




    TwAddVarRW(cBar, "fancyLight", TW_TYPE_BOOLCPP, &visualizer->fancyLight, "");

    TwAddSeparator(cBar, 0, 0); 
    TwEnumVal priority_modeEV[PRIORITY_MODE_NUM] = {
        { Visualizer::MediaFigli, "MediaFigli" },
        { Visualizer::PrioritaInvertita, "PrioritaInvertita" }, { Visualizer::PrioritaNormale, "Normale" },
        { Visualizer::MediaStimata, "MediaStimata" },{ Visualizer::Media, "Media" },
        { Visualizer::MediaStimataFigli, "MediaStimataFigli" } };
    TwType priority_modeT = TwDefineEnum("prioritYModeType", priority_modeEV, PRIORITY_MODE_NUM);
    TwAddVarRW(cBar, "PriorityMode", priority_modeT, &visualizer->priorityMode, " keyIncr='<' keyDecr='>'");


    TwAddVarRW(cBar, "Fusion", TW_TYPE_BOOLCPP, &visualizer->topology_index->fusionIsSet, "");

    TwAddVarRW(cBar, "Prune Level", TW_TYPE_INT32, &visualizer->topology_index->pruneLevel, "min=0 step=1");

    TwAddVarRW(cBar, "Fusion base factor", TW_TYPE_INT32, &visualizer->topology_index->fusion_base_factor, "");
    TwAddVarRW(cBar, "Fusion progression factor", TW_TYPE_INT32, &visualizer->topology_index->fusion_progression_factor, "");
    TwAddButton(cBar, "Rebuild topology index", rebuildIndex, NULL, "");

    TwAddVarRW(cBar, "Edge Query", TW_TYPE_BOOLCPP, &visualizer->topology_index->edgeQuery, "");

    TwAddVarRW(cBar, "query radius factor", TW_TYPE_DOUBLE, &visualizer->topology_index->queryRadiusFactor, "");
    TwAddVarRW(cBar, "number of levels", TW_TYPE_INT32, &visualizer->topology_index->numberOfLevels, "");

    TwAddVarRW(cBar, "Profiling", TW_TYPE_BOOLCPP, &visualizer->topology_index->profiling, "");

    TwAddSeparator(cBar, 0, 0);

    TwAddVarRW(cBar, "Picked Vertex", TW_TYPE_INT32, &visualizer->topology_index->pickedVertex, "");

    TwAddButton(cBar, "launch breadth sphere query", doTopologicQuery, NULL, "");
    TwAddButton(cBar, "launch space sphere query", doSpaceQuery, NULL, "");
    TwAddButton(cBar, "launch cluster query", doClusterQuery, NULL, ""); //


    TwAddSeparator(cBar, 0, 0);

    TwAddVarRW(cBar, "sphere breadth vertices", TW_TYPE_BOOLCPP, &visualizer->enableSphereBreadthV, "");
    TwAddVarRW(cBar, "sphere space vertices", TW_TYPE_BOOLCPP, &visualizer->enableSphereSpaceV, "");
    TwAddVarRW(cBar, "cluster query vertices", TW_TYPE_BOOLCPP, &visualizer->enableClusterV, ""); //
    TwAddVarRW(cBar, "picked vertex", TW_TYPE_BOOLCPP, &visualizer->enablePickV, "");

    TwAddSeparator(cBar, 0, 0);

    visualizer->setSphereRadius(5.0);
    TwAddVarCB(cBar, "scaled radius", TW_TYPE_FLOAT, setFactor, getFactor, NULL, "min=1.00 max=100.00 step=0.1");
    TwAddVarRO(cBar, "average edge", TW_TYPE_FLOAT, &visualizer->averageEdge, "");
    TwAddVarRO(cBar, "final radius", TW_TYPE_FLOAT, &visualizer->sphereRadius, "");

    TwAddSeparator(cBar, 0, 0);
    TwAddSeparator(cBar, 0, 0);

    TwAddVarRW(cBar, "sphere begin", TW_TYPE_FLOAT, &visualizer->sphere_begin, "");
    TwAddVarRW(cBar, "sphere end", TW_TYPE_FLOAT, &visualizer->sphere_end, "");
    TwAddVarRW(cBar, "sphere step", TW_TYPE_FLOAT, &visualizer->sphere_step, "");
    //TwAddButton(cBar, "launch benchmark", doBenchmark, NULL, "");

    glutMainLoop();
    return 0;
}*/
