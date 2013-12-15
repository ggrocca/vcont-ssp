#ifndef _DISPLAYMANAGER_HH
#define _DISPLAYMANAGER_HH

#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#define __ESC_GLUT 27
#include <AntTweakBar.h>


#include <vector>
using std::vector;
#include <string>
using std::string;
#include <sstream>
using std::stringstream;

#include "geomapping.hh"
#include "datamanager.hh"

/////////   twcallback (order) // forza il riordino; clientdata per accedere oggett giusto?
/*
class DisplayPlane // interfaccia implementata dai diversi tipi di plane
{
  tutti i costruttori prendono la twbar, geomapping e il loro tipo di dato
  
  twbar; + twbarname;
  geomapping*;
  int order;
  
    display ();
}
    

// ogni DisplaPlaneDataType implementa quest'interfaccia per il tipo di dato datatype.
// le se sue variabili e callback antweakbar sono private

 */

class DisplayPlane
{

public:

    int sidx, pidx;
    TwBar* bar;
    string bar_name;
    GeoMapping* map;
    int order;
    BoundingBox viewport;

    DisplayPlane (int sidx, int pidx);
    virtual ~DisplayPlane ();

    void set_viewport (int w, int h);
    virtual void display ();
    virtual void push_transform ();
    virtual void pop_transform ();
};


DisplayPlane* selector (Plane* p, GeoMapping* m, int sidx, int pidx);

/*
class DisplayManager
{
  a partire dal datamanager costruisce se stessa e i display plane
  
  vector<DisplayPlane> all // tutti i possibili display plane
  
  vector<DisplayPlane*> vis // display attivi nell'ordine giusto
  
  void reorder_display (); //ricostruisce vis scannando all
  
  void display (); // usa vis per visualizzare
}
*/

class DisplayManager
{
public:

    DataManager* dm;

    vector<DisplayPlane*> all_dp;
    vector<DisplayPlane*> vis_dp;

    DisplayManager (DataManager* dm);
    ~DisplayManager ();

    void set_viewport (int w, int h);
    void display ();
    void reorder_display ();
    void mute_current ();
    int find_current ();

    int sidx, pidx;
    vector<int> p_pidx;
};


#endif // _DISPLAYMANAGER.HH
