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
    
    DisplayPlane (int sidx, int pidx) : sidx (sidx), pidx (pidx)
    {
	// base interface
	
	std::stringstream ss; ss << "bar_plane_" << sidx <<"_"<< pidx;
	
	bar_name = ss.str();
	
	bar = TwNewBar(bar_name.c_str());

	string s = bar_name + " "
	    "size='250 350' valueswidth=80 "
	    "color='192 192 255' text=dark "
	    "position='270 10' "
	    "visible=false";
	TwDefine(s.c_str());


	order = -1;

	TwAddVarRW(bar, "vis_order", TW_TYPE_INT32, &order,
		   "min=-1 max=256 step=1");
    }

    
    virtual ~DisplayPlane () {}
    virtual void display () {}
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

    void display ();
    void reorder_display ();
    void mute_current ();
    int find_current ();

    int sidx, pidx;
    vector<int> p_pidx;
};


#endif // _DISPLAYMANAGER.HH
