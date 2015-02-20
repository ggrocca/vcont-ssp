#include "scalarfield.hh"

ScalarField::ScalarField (Mesh& mesh) : Field<double> (mesh), max (-DBL_MAX), min (DBL_MAX) {}

ScalarField::ScalarField (Mesh& mesh, const char*curvfile) : Field<double> (mesh), max (-DBL_MAX), min (DBL_MAX) // ascii format
{
    int size;

    FILE *fp = fopen (curvfile, "r");

    fscanf (fp, "%d", &size);
    resize (size);
    
    for (int i = 0; i < size; i++)
    {
	double kv1, kv2;
	Point3D kd1, kd2;

	fscanf (fp, "%lf %lf "
		"%lf %lf %lf "
		"%lf %lf %lf ", &kv1, &kv2,
		&kd1.x, &kd1.y, &kd1.z,
		&kd2.x, &kd2.y, &kd2.z);

	// right know it's gaussian curvature - there should be an option to select
	(*this)[i] = kv1 * kv2;
    }
}

ScalarField::ScalarField (ScalarField& sc) : Field<double> (sc.mesh), max (sc.max), min (sc.min)
{
    for (unsigned i = 0; i < sc.size(); i++)
	(*this)(i) = sc (i);
}

ScalarField::ScalarField (Mesh& mesh, FILE* fp) : Field<double> (mesh) // binary format
{
    int size;
    
    fread (&size, sizeof (int), 1, fp);

    tprints (SCOPE_IO, "size:%d\n", size);

    resize (size);
    fread (&(data[0]), sizeof (double), data.size(), fp);

    for (int i = 0; i < size; i++)
    {
	double v = (*this)(i);
	if (v < min)
	    min = v;
	if (v > max)
	    max = v;
	
    }
    
    tprints (SCOPE_IO, "min: %le, max:%le\n", min, max);
}

unsigned ScalarField::size ()
{
    return data.size();
}


void ScalarField::write (FILE* fp)
{
    int sz = size();
    fwrite (&sz, sizeof (int), 1, fp);
    fwrite (&(data[0]), sizeof (double), data.size(), fp);
}

double& ScalarField::operator() (int v)
{
    return Field<double>::operator()(v);
}

double& ScalarField::operator[] (int v)
{
    return Field<double>::operator[](v);
}

bool ScalarField::is_equal (int a, int b)
{
    double av = (*this)(a);
    double bv = (*this)(b);

    if (av > bv)
    	return false;
    if (av < bv)
    	return false;

    return true;
}

bool ScalarField::has_plateaus ()
{
    for (unsigned i = 0; i < size(); i++)
    {
	std::vector<int> vn;
	mesh.neighborhood (i, vn);
	
	for (unsigned j = 0; j < vn.size (); j++)
	{
	    if (is_equal (i, vn[j]))
	    {
		printf ( "--> PLATEAUS: [%d]==[%d]. Values: %le,%le --- -DBL_MAX=%le\n",
			 i, vn[j], (*this)(i), (*this)(vn[j]), -DBL_MAX);
		return true;
	    }
	}
    }

    return false;

}

RelationType ScalarField::point_relation (int a, int b)
{
    double av = (*this)(a);
    double bv = (*this)(b);

    if (av > bv)
    	return GT;
    if (av < bv)
    	return LT;

    eprintx (EXIT_RELATION,
	     "Plateu detected! a: %d[%lf], b: %d[%lf]\n",
	     a, av, b, bv);

    return EQ;    
}

CriticalType ScalarField::point_type (int v)
{
    RelationType current,previous,first;
    int changes = 0;

    std::vector<int> vn;
    mesh.neighborhood (v, vn);

    // fprintf (stderr, "!!!!!! %zu -- v: %d V(%zu) VV(%zu)\n",
    // 	     vn.size(), v, mesh.V.size(), mesh.VV.size());
    
    first = previous = point_relation (v, vn[vn.size()-1]);
    
    for (unsigned i = 0; i < vn.size(); i++)
    {
	current = point_relation (v, vn[i]);
	point_type_step (current, &previous, &changes);
    }

    CriticalType r = point_type_analyze (first, changes, v, vn.size());

    return r;
}



//////////////////////////
//////////////////////////

void scan_critical_vertices (ScalarField* f, std::vector<CriticalVertex>& cps)
{
    cps.clear();

    int nbmax = 0;
    for (unsigned i = 0; i < f->size(); i++)
	if (f->mesh.neighborhood_size (i) > nbmax)
	    nbmax = f->mesh.neighborhood_size (i);
   printf ("max neighborhood_size: %d\n", nbmax);
    
    for (unsigned i = 0; i < f->size(); i++)
	{
	    CriticalType ct;

	    if ((ct = f->point_type (i)) != REG)
	    {
		CriticalVertex cv;
		cv.v = i;
		cv.t = ct;
		cps.push_back (cv);
	    }
	}
}

SurfaceScaleSpace::SurfaceScaleSpace (std::vector<ScalarField*> scfs)
{
    levels = scfs.size ();
    
    fields = std::vector<ScalarField*>(levels);
    for (int i = 0; i < levels; i++)
	fields[i] = scfs[i];

    criticals = std::vector< std::vector<CriticalVertex> > (levels);
    for (int i = 0; i < levels; i++)
    {
	criticals[i] = std::vector<CriticalVertex>();
	scan_critical_vertices (fields[i], criticals[i]);
	tprintp ("###$$$", "Critical vertices, scanned level %d\n", i);
    }
    
}

// SurfaceScaleSpace::SurfaceScaleSpace (ScalarField& scfs)
// {
//     // gg this should make gaussian smoothing
// }


SurfaceScaleSpace::SurfaceScaleSpace (Mesh& mesh, const char* filename)
{
    FILE* fp = fopen (filename, "r");
    if (fp == NULL)
	eprintx (2, "Could not open file `%s'. %s\n", filename, strerror (errno));

    fread (&levels, sizeof (int), 1, fp);

    fields = std::vector<ScalarField*>(levels);
    for (int i = 0; i < levels; i++)
	fields[i] = new ScalarField (mesh, fp);

    fclose (fp);

    criticals = std::vector< std::vector<CriticalVertex> > (levels);
    for (int i = 0; i < levels; i++)
    {
	criticals[i] = std::vector<CriticalVertex>();
	scan_critical_vertices (fields[i], criticals[i]);
	tprintp ("###$$$", "Critical vertices, scanned level %d\n", i);
    }
}

SurfaceScaleSpace::~SurfaceScaleSpace ()
{
    for (int i = 0; i < levels; i++)
	delete fields[i];
}


void SurfaceScaleSpace::write_critical (char* filename)
{
    FILE* fp = fopen (filename, "w");

    if (fp == NULL)
	eprint ("Could not write file %s\n", filename);

    fprintf (fp, "stats | #num_levels %d\n", levels);

    for (int i = 0; i < levels; i++)
    {
	int num_max, num_min, num_sad;
	num_max = num_min = num_sad = 0;
	fprintf (fp,
		 "stats |     level %d\n"
		 "stats |     #num_points %d\n",
		 i, (int)criticals[i].size());

	for (unsigned j = 0; j < criticals[i].size(); j++)
	{
	    if (criticals[i][j].t == MAX)
		num_max++;
	    if (criticals[i][j].t == MIN)
		num_min++;
	    if (criticals[i][j].t == SA2)
		num_sad++;
	    if (criticals[i][j].t == SA3)
		num_sad += 2;
	}

	fprintf (fp,
		 "stats |  \tnum_max = %d\n"
		 "stats |  \tnum_min = %d\n"
		 "stats |  \tnum_sad = %d\n"
		 "stats |  \tnum_max + num_min - num_sad = %d\n",
		 num_max, num_min, num_sad,
		 num_max + num_min - num_sad);

	for (unsigned j = 0; j < criticals[i].size(); j++)
	    fprintf (fp, "\t\t\tpoint %d [%5d] {%c}\n", 
		     j, criticals[i][j].v,
		     critical2char (criticals[i][j].t));

	fprintf (fp, "\n");
    }
}

void SurfaceScaleSpace::write_scalespace (char* filename)
{
    FILE* fp = fopen (filename, "w");
    if (fp == NULL)
	eprintx (2, "Could not open file `%s'. %s\n", filename, strerror (errno));

    fwrite (&levels, sizeof (int), 1, fp);

    for (int i = 0; i < levels; i++)
	fields[i]->write (fp);

    fclose (fp);

}
