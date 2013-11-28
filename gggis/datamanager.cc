#include <fstream>

#include "datamanager.hh"
#include "../common/track.hh"
#include "../common/scalespace.hh"


char* DST_strings[] = __DST_STRINGS;
char* DST_extensions[][_DM_EXT_NUM] = __DST_EXTENSIONS;

DataManager::DataManager (char *dir)
{
    vector<string> es;
    list_dir (dir, &es);
    
    for (int i = 0; i < es.size(); i++)
	datasets.push_back (new DataSet (es[i]));
}

DataManager::~DataManager ()
{
    for (int i = 0; i < datasets.size(); i++)
        delete datasets[i];
}


DataSet::DataSet (string dir) : dir (dir), name (get_base (dir))
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
    list_dir (dir.c_str(), &es);

    if (!ends_with (es[0], "__GEOREF.txt"))
        eprintx (2, "No georef file in dataset %s\n", dir.c_str());

    map = GeoMapping (es[0]);

    for (int i = 1; i < es.size(); i++)
	planes.push_back (new Plane (es[i]));

}

DataSet::~DataSet ()
{
    for (int i = 0; i < planes.size(); i++)
        delete planes[i];    
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
    	data = (void*) DEMSelector::get (pathname.c_str());
    	break;

    case IMG:
    	// CImg
    	data = (void*) new cimg_library::CImg<unsigned char>(pathname.c_str());
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
    	data = new Track (pathname.c_str());
    	break;

    case SSP:
    	// ScaleSpace
    	data = (void*) new ScaleSpace (pathname.c_str(), ScaleSpaceOpts());
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
	delete (DEMReader*) data;
    	break;

    case IMG:
    	// CImg
	delete (cimg_library::CImg<unsigned char>*) data;
    	break;

    case CRT:
    	// build a critical points loader if needs arise
    	break;

    case TRK:
    	// Track
	delete (Track*) data;
    	break;

    case SSP:
    	// ScaleSpace
	delete (ScaleSpace*) data;
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
