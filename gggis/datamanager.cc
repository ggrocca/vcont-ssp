#include <fstream>

#include "datamanager.hh"
#include "../common/track.hh"
#include "../common/scalespace.hh"


char* DST_strings[] = __DST_STRINGS;
char* DST_extensions[][_DM_EXT_NUM] = __DST_EXTENSIONS;

DataManager::DataManager (char *dir, DataOrganization org)
{
    if (org == MULTIPLE)
    {
	vector<string> es;
	list_dir (dir, &es);
    
	for (unsigned i = 0; i < es.size(); i++)
	    datasets.push_back (new DataSet (es[i], org));
    }

    if (org == SINGLE)
	datasets.push_back (new DataSet (dir, org));
    
}

DataManager::~DataManager ()
{
    for (unsigned i = 0; i < datasets.size(); i++)
        delete datasets[i];
}

BoundingBox DataManager::getbb ()
{
    vector <BoundingBox> all;
    for (unsigned i = 0; i < datasets.size(); i++)
	    all.push_back (datasets[i]->getbb_world());

    return BoundingBox (all);    
}


DataSet::DataSet (string dir, DataOrganization org) : dir (dir), name (get_base (dir))
{
    /*
      nome di directory in input

      la directory contiene tante altre directory, con un nome che e' il
      nome del dataset ogni dataset ha una particolare registrazione di
      coordinate in un file di nome __GEOREF.txt

      __GEOREF.txt contiene un mapping lineare:
      xl,yl:lat,lon
      xu,yu:lat,lon

      che da' la trasformazione lineare da applicare a un rettangolo che
      contiene le possibili coordinate contenute nel dataset.

      Il dataset contiene un numero arbitrario di file che vengono riconosciuti

      file di tipo datasettype
      typedef enum {DEM, IMG, CRT, TRK, SSP, CRV, EXM} DataSetType;
  
      associazione di default dall'estensione, a meno che non esista un
      file nomefile.type che contiene il tipo da considerare.

    */
    
    vector<string> es;
    bool have_georef_in_list_dir;
    string georef_file;

    if (org == PLANE) // we open a single file
    {
	es.push_back (dir);
	have_georef_in_list_dir = false;
	georef_file = dir + DM_GEOREF_FILE;
    }
    else // we open a dir
    {
	list_dir (dir.c_str(), &es);
	if (have_georef_in_list_dir = ends_with (es[0], DM_GEOREF_FILE))
	    georef_file = es[0];
    }    

    // save all possible planes
    for (unsigned i = have_georef_in_list_dir? 1 : 0; i < es.size(); i++)
	planes.push_back (new Plane (es[i]));

    FILE *fp = fopen (georef_file.c_str(), "r");
    if (fp != NULL) // test presence of georef file
    {
	fclose (fp);
	map = GeoMapping (georef_file.c_str());
    }
    else if (org == MULTIPLE) // multiple datasets and no file, exit
	eprintx (2, "No georef file in dataset %s\n", dir.c_str());	    
    else // single dataset, no file, we get the bounding box
	map = GeoMapping (getbb_local(), getbb_local());




    // 	planes.push_back (new Plane (es[i]));
	
    // 	string georef = 

    // }
    // else
    // {

    // 	bool have_georef = 

    // 	map = NULL;

    // 	if (have_georef)
    // 	    map = new GeoMapping (es[0]);

    // }
}

DataSet::~DataSet ()
{
    for (unsigned i = 0; i < planes.size(); i++)
        delete planes[i];    
}

BoundingBox DataSet::getbb_local ()
{
    vector <BoundingBox> all;
    for (unsigned i = 0; i < planes.size(); i++)
	all.push_back (planes[i]->getbb());

    return BoundingBox (all);
}

BoundingBox DataSet::getbb_world ()
{
    return map.world;
}

BoundingBox Plane::getbb ()
{
    switch (type)
    {
    case DEM:
	{
	    // DEMReader
	    Point a (0,0);
	    Point b;
	    b.x = ((DEMReader*) data[0])->width;
	    b.y = ((DEMReader*) data[0])->height;
	    return BoundingBox (a, b);
	}
    case IMG:
	{
	    // CImg
	    Point a (0,0);
	    Point b;
	    b.x = ((cimg_library::CImg<unsigned char>*) data[0])->width ();
	    b.y = ((cimg_library::CImg<unsigned char>*) data[0])->height ();
	    return BoundingBox (a, b);
	}
    case CRT:
    	// build a critical points loader if needs arise
	return BoundingBox::emptiest ();

    case TRK:
    	// Track
	return BoundingBox::emptiest ();

    case SSP:
	{
	    // ScaleSpace
	    Point a (0,0);
	    Point b;
	    b.x = ((ScaleSpace*) data[0])->dem[0]->width;
	    b.y = ((ScaleSpace*) data[0])->dem[0]->height;
	    return BoundingBox (a, b);
	}
    case CRV:
    	// TODO
	return BoundingBox::emptiest ();

    case EXM:
    	// TODO
	return BoundingBox::emptiest ();

    case INL:
    	// TODO
	return BoundingBox::emptiest ();

    case UKW:
    default:
    	eprintx (45, "Unrecognized file type for file %s.\n", filename.c_str());
	return BoundingBox::emptiest ();
    }

}

Plane::Plane (string pathname) : pathname (pathname), filename (get_base (pathname)) 
{
    string type_string;

    if (get_ext (pathname) == _DM_TYPEFILE_EXT)
    	return;

    std::ifstream ifs ((pathname + _DM_TYPEFILE_EXT).c_str());
    if (ifs.is_open ())
	type_string.assign ((std::istreambuf_iterator<char>(ifs)),
			    (std::istreambuf_iterator<char>()));
    else
	type_string = ext2ds_string (get_ext (pathname));

    type = string2ds_type (type_string);

    switch (type)
    {
    case DEM:
    	// DEMReader
    	data.push_back ((void*) DEMSelector::get (pathname.c_str()));
    	break;

    case IMG:
    	// CImg
    	data.push_back ((void*) new cimg_library::CImg<unsigned char>(pathname.c_str()));
    	// width = img.width ();
    	// height = img.height ();
    	// y = height - y - 1;
    	// img(x,y);
    	break;

    case CRT:
    	// build a critical points loader if needs arise
    	break;

    case TRK:
    	// Track
    	// data = (void*) new Track (pathname.c_str());
	Track* track;
	TrackOrdering* order;
	track_reader (pathname.c_str(), &track, &order);
    	data.push_back ((void*) track);
	data.push_back ((void*) order);
	tprints (SCOPE_TRACKING, "HERE: %zu\n", order->events.size());
    	break;

    case SSP:
    	// ScaleSpace
    	data.push_back ((void*) new ScaleSpace (pathname.c_str(), ScaleSpaceOpts()));
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
    	eprintx (45, "Unrecognized file type for file %s.\n", filename.c_str());
    	break;
    }

}


Plane::~Plane ()
{
    switch (type)
    {
    case DEM:
    	// DEMReader
	delete (DEMReader*) data[0];
    	break;

    case IMG:
    	// CImg
	delete (cimg_library::CImg<unsigned char>*) data[0];
    	break;

    case CRT:
    	// build a critical points loader if needs arise
    	break;

    case TRK:
    	// Track
	delete (Track*) data[0];
	delete (TrackOrdering*) data[1];
    	break;

    case SSP:
    	// ScaleSpace
	delete (ScaleSpace*) data[0];
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
}
