#include <algorithm>
// class DisplayManager
// {
//     DataManager* dm;

//     vector<DisplayPlane*> all_dp;
//     vector<DisplayPlane*> vis_dp;

//     DisplayManager (DataManager* dm);
//     ~DisplayManager ();

//     void display ();
//     void reorder_display();
// }

#include "displaymanager.hh"
#include "displaydem.hh"




// code to make bar visible
void __bar_visibility (int idx, bool visible)
{
    stringstream name;
    name << "bar_plane_" << idx;
    string bar_name = name.str();
    string is_vis = visible? " visible=true" : " visible=false";
	
    TwDefine ((bar_name + is_vis).c_str());
}


void TW_CALL tw_setbarnum (const void *value, void *cd)
{
    int new_bar_num = *(const int *) value;

    __bar_visibility (((DisplayManager*) cd)->bar_num, false);
    __bar_visibility (new_bar_num, true);

    ((DisplayManager*) cd)->bar_num = new_bar_num;

    glutPostRedisplay();
}


void TW_CALL tw_getbarnum (void *value, void *cd)
{
    *(int *) value = ((DisplayManager*) cd)->bar_num;
}


void TW_CALL tw_reorder (void *cd)
{ 
    ((DisplayManager*) cd)->reorder_display ();
}



DisplayManager::DisplayManager (DataManager* dm) : dm (dm)
{
    int k = 0;
    for (int i = 0; i < dm->datasets.size(); i++)
	for (int j = 0; j < dm->datasets[i]->planes.size(); j++)
	    all_dp.push_back (selector (dm->datasets[i]->planes[i],
					&(dm->datasets[i]->map), k++) );

    reorder_display ();

    bar_num = 0;
    for (int i = 0; i < all_dp.size(); i++)
	__bar_visibility (i, !i);

    TwBar* cbar;

    cbar = TwNewBar("Controller");
    TwDefine("Controller size='250 350'");
    TwDefine("Controller valueswidth=80");
    TwDefine("Controller color='192 255 192' text=dark ");

    stringstream ss; ss << "min=0 max="<< ((int)all_dp.size())-1 <<" step=1 keydecr='<' keyincr='>'";
    string s = ss.str();
    TwAddVarCB(cbar, "select plane", TW_TYPE_INT32, tw_setbarnum, tw_getbarnum, this, s.c_str());

    TwAddButton(cbar, "vis_reorder", tw_reorder, this, "");
}

DisplayManager::~DisplayManager () {}

void DisplayManager::display ()
{
    // static int yay = 1;
    // static int dim = 0;
    // if (yay)
    // 	tprint ("Here yay %d\n", yay = !yay);
    // if (dim != vis_dp.size())
    // 	tprint ("Here I DIM %d\n", vis_dp.size());
    // dim = vis_dp.size();


    for (int i = 0; i < vis_dp.size(); i++)
	vis_dp[i]->display ();
}


bool display_comparison (DisplayPlane* lp, DisplayPlane* rp)
{
    return lp->order < rp->order;
}

void DisplayManager::reorder_display ()
{
    vis_dp.clear ();

    for (int i = 0; i < all_dp.size(); i++)
	if (all_dp[i]->order >= 0)
	    vis_dp.push_back (all_dp[i]);

    std::sort (vis_dp.begin(), vis_dp.end(), display_comparison);
}



DisplayPlane* selector (Plane* p, GeoMapping* m, int idx)
{
    switch (p->type)
    {
    case DEM:
	// DEMReader
	return new DisplayDem (p, m, idx);
	break;

    case IMG:
	// CImg
	//(cimg_library::CImg<unsigned char>*);
	break;

    case CRT:
	// build a critical points loader if needs arise
	break;

    case TRK:
	// Track
	//(Track*);
	break;

    case SSP:
	// ScaleSpace
	//(ScaleSpace*);
	break;

    case CRV:
	// TODO
	break;

    case EXM:
	// TODO
	break;

    case INL:
	// TODO
	break;

    case UKW:
    default:
	break;
    }

    return new DisplayPlane (idx);
}

