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
void __bar_visibility (int sidx, int pidx, bool visible)
{
    stringstream name;
    name << "bar_plane_" << sidx <<"_"<< pidx;
    string bar_name = name.str();
    string is_vis = visible? " visible=true" : " visible=false";
	
    TwDefine ((bar_name + is_vis).c_str());

    TwBar* b = TwGetBarByName(bar_name.c_str());
    TwRefreshBar (b);
}


void TW_CALL tw_set_sidx (const void *value, void *cd)
{
    int new_sidx = *(const int *) value;
    int new_pidx = ((DisplayManager*) cd)->p_pidx[new_sidx];

    __bar_visibility (((DisplayManager*) cd)->sidx, ((DisplayManager*) cd)->pidx, false);
    __bar_visibility (new_sidx, new_pidx, true);

    ((DisplayManager*) cd)->sidx = new_sidx;
    ((DisplayManager*) cd)->pidx = new_pidx;

    stringstream ss; ss<<"Controller/select_plane "<<" min=0 max="
		       <<((int) (((DisplayManager*) cd)->dm->datasets[new_sidx]->planes.size ())) - 1;
    TwDefine(ss.str().c_str());

    glutPostRedisplay();
}


void TW_CALL tw_get_sidx (void *value, void *cd)
{
    *(int *) value = ((DisplayManager*) cd)->sidx;
}

void TW_CALL tw_set_pidx (const void *value, void *cd)
{
    int new_pidx = *(const int *) value;

    __bar_visibility (((DisplayManager*) cd)->sidx, ((DisplayManager*) cd)->pidx, false);
    __bar_visibility (((DisplayManager*) cd)->sidx, new_pidx, true);

    ((DisplayManager*) cd)->pidx = new_pidx;
    ((DisplayManager*) cd)->p_pidx[((DisplayManager*) cd)->sidx] = new_pidx;

    glutPostRedisplay();
}


void TW_CALL tw_get_pidx (void *value, void *cd)
{
    *(int *) value = ((DisplayManager*) cd)->pidx;
}


void TW_CALL tw_reorder (void *cd)
{ 
    ((DisplayManager*) cd)->reorder_display ();
}



DisplayManager::DisplayManager (DataManager* dm) : dm (dm)
{
    for (int i = 0; i < dm->datasets.size(); i++)
    {
	for (int j = 0; j < dm->datasets[i]->planes.size(); j++)
	{
	    all_dp.push_back (selector (dm->datasets[i]->planes[j],
					&(dm->datasets[i]->map), i, j) );
	    // __bar_visibility (i, j, false);

	}
	p_pidx.push_back(0);
    }
    reorder_display ();

    pidx = sidx = 0;

    TwBar* cbar;

    cbar = TwNewBar("Controller");
    TwDefine("Controller size='250 350'");
    TwDefine("Controller valueswidth=80");
    TwDefine("Controller color='192 255 192' text=dark ");
    TwDefine("Controller position='10 10'");
    
    stringstream ss; ss << "min=0 max="<< ((int) dm->datasets.size ()) - 1 <<" step=1 keydecr='<' keyincr='>'";
    string s = ss.str();
    TwAddVarCB(cbar, "select_dataset", TW_TYPE_INT32, tw_set_sidx, tw_get_sidx, this, s.c_str());
    ss.str(""); ss.clear(); ss << "min=0 max="<< ((int) dm->datasets[0]->planes.size ()) - 1 <<" step=1 keydecr='[' keyincr=']'";
    s = ss.str();
    TwAddVarCB(cbar, "select_plane", TW_TYPE_INT32, tw_set_pidx, tw_get_pidx, this, s.c_str());

    TwAddButton(cbar, "vis_reorder", tw_reorder, this, "");

    __bar_visibility (sidx, pidx, true);
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



DisplayPlane* selector (Plane* p, GeoMapping* m, int sidx, int pidx)
{
    switch (p->type)
    {
    case DEM:
	// DEMReader
	return new DisplayDem (p, m, sidx, pidx);
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

    return new DisplayPlane (sidx, pidx);
}

