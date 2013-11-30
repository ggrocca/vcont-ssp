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


#define _DSM_NAMELEN 32
char sname[_DSM_NAMELEN] = {'\0'};
char pname[_DSM_NAMELEN] = {'\0'};

void __setnames (DisplayManager* dsm)
{
    sname[0] = '\0';
    pname[0] = '\0';
    strlcat (sname, dsm->dm->datasets[dsm->sidx]->name.c_str (), _DSM_NAMELEN);
    strlcat (pname, dsm->dm->datasets[dsm->sidx]->planes[dsm->pidx]->filename.c_str (), _DSM_NAMELEN);
}
#undef _DSM_NAMELEN

// static char DisplayManager::sname[32];
// static char DisplayManager::pname[32];

void __bar_update (int sidx, int pidx)
{
    stringstream name;
    name << "bar_plane_" << sidx <<"_"<< pidx;
    string bar_name = name.str();

    TwBar* b = TwGetBarByName(bar_name.c_str());
    TwRefreshBar (b);
}

// code to make bar visible
void __bar_visibility (int sidx, int pidx, bool visible)
{
    stringstream name;
    name << "bar_plane_" << sidx <<"_"<< pidx;
    string bar_name = name.str();
    string is_vis = visible? " visible=true" : " visible=false";
	
    TwDefine ((bar_name + is_vis).c_str());

    __bar_update (sidx, pidx);
}

void TW_CALL tw_set_sidx (const void *value, void *cd)
{
    DisplayManager* dsm = (DisplayManager*) cd;
    int new_sidx = *(const int *) value;
    int new_pidx = dsm->p_pidx[new_sidx];

    __bar_visibility (dsm->sidx, dsm->pidx, false);
    __bar_visibility (new_sidx, new_pidx, true);

    dsm->sidx = new_sidx;
    dsm->pidx = new_pidx;

    stringstream ss; ss<<"Controller/plane "<<" min=0 max="
		       <<((int) (dsm->dm->datasets[new_sidx]->planes.size ())) - 1;
    TwDefine(ss.str().c_str());
    __setnames (dsm);

    glutPostRedisplay();
}


void TW_CALL tw_get_sidx (void *value, void *cd)
{
    DisplayManager* dsm = (DisplayManager*) cd;
    *(int *) value = dsm->sidx;
}

void TW_CALL tw_set_pidx (const void *value, void *cd)
{
    DisplayManager* dsm = (DisplayManager*) cd;
    int new_pidx = *(const int *) value;

    __bar_visibility (dsm->sidx, dsm->pidx, false);
    __bar_visibility (dsm->sidx, new_pidx, true);

    dsm->pidx = new_pidx;
    dsm->p_pidx[dsm->sidx] = new_pidx;
    __setnames (dsm);

    glutPostRedisplay();
}


void TW_CALL tw_get_pidx (void *value, void *cd)
{
    DisplayManager* dsm = (DisplayManager*) cd;
    *(int *) value = dsm->pidx;
}


void TW_CALL tw_prio (void *cd)
{ 
    DisplayManager* dsm = (DisplayManager*) cd;
    dsm->reorder_display ();
}

void TW_CALL tw_mute (void *cd)
{ 
    DisplayManager* dsm = (DisplayManager*) cd;
    dsm->mute_current ();
}



DisplayManager::DisplayManager (DataManager* dm) : dm (dm)
{
    for (unsigned i = 0; i < dm->datasets.size(); i++)
    {
	for (unsigned j = 0; j < dm->datasets[i]->planes.size(); j++)
	{
	    all_dp.push_back (selector (dm->datasets[i]->planes[j],
					&(dm->datasets[i]->map), i, j) );
	    // __bar_visibility (i, j, false);

	}
	p_pidx.push_back(0);
    }
    reorder_display ();

    pidx = sidx = 0;
    __setnames (this);

    TwBar* cbar;

    cbar = TwNewBar("Controller");
    TwDefine("Controller size='250 350'");
    TwDefine("Controller valueswidth=170");
    TwDefine("Controller color='192 255 192' text=dark ");
    TwDefine("Controller position='10 10'");
    
    stringstream ss; ss << "min=0 max="<< ((int) dm->datasets.size ()) - 1 <<" step=1 keydecr='<' keyincr='>'";
    string s = ss.str();
    
    TwAddSeparator(cbar, NULL, NULL);
    TwAddButton(cbar, "selection", NULL, NULL, "");
    TwAddVarCB (cbar, "dtset", TW_TYPE_INT32, tw_set_sidx, tw_get_sidx, this, s.c_str());
    TwAddVarRO (cbar, "sname", TW_TYPE_CSSTRING(sizeof(sname)), sname, "");
    ss.str(""); ss.clear(); ss << "min=0 max="<< ((int) dm->datasets[0]->planes.size ()) - 1 <<" step=1 keydecr='[' keyincr=']'";
    s = ss.str();
    TwAddVarCB (cbar, "plane", TW_TYPE_INT32, tw_set_pidx, tw_get_pidx, this, s.c_str());
    TwAddVarRO (cbar, "pname", TW_TYPE_CSSTRING(sizeof(pname)), pname, "");

    TwAddSeparator(cbar, NULL, NULL);
    TwAddButton(cbar, "visibility", NULL, NULL, "");
    TwAddButton (cbar, "prio", tw_prio, this, "");
    TwAddButton (cbar, "mute", tw_mute, this, "");

    TwAddSeparator(cbar, NULL, NULL);


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


    for (unsigned i = 0; i < vis_dp.size(); i++)
	vis_dp[i]->display ();
}


bool display_comparison (DisplayPlane* lp, DisplayPlane* rp)
{
    return lp->order < rp->order;
}

void DisplayManager::reorder_display ()
{
    vis_dp.clear ();

    for (unsigned i = 0; i < all_dp.size(); i++)
	if (all_dp[i]->order >= 0)
	    vis_dp.push_back (all_dp[i]);

    std::sort (vis_dp.begin(), vis_dp.end(), display_comparison);
}


int DisplayManager::find_current ()
{
    for (unsigned i = 0; i < all_dp.size(); i++)
	if (sidx == all_dp[i]->sidx && pidx == all_dp[i]->pidx)
	    return i;

    return -1;
}


void DisplayManager::mute_current ()
{
    int i = find_current ();
    if (i < 0)
	return;

    all_dp[i]->order = -1;
    __bar_update (sidx, pidx);
    reorder_display ();
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

