#include "math.h"
#include "demreader.hh"
#include <float.h>


void DEMReader::set_maxmin ()
{
    max = -DBL_MAX;
    min = DBL_MAX;

    for (unsigned i = 0; i < width; i++)
	for (unsigned j = 0; j < height; j++)
	{
	    double v = get_pixel (i, j);
	    if (v < min)
		min = v;
	    if (v > max)
		max = v;
	}
}


const char* DEMSelector::fmts[] = {"png", "bmp", "tif", "hgt", "asc", "\0"};

PNGReader::PNGReader(const char *filename)
{
    img.assign (filename);
    width = img.width ();
    height = img.height ();
    set_maxmin();
    // print_info (filename);
}

PNGReader::~PNGReader()
{

}

double PNGReader::get_pixel (unsigned int x, unsigned int y)
{
    y = height - y - 1;
    return img(x,y);
}

void PNGReader::print_info (char *filename)
{
    fprintf (OTHER_STREAM, "PNGReader: ");
    // print file only if not null
    char* empty = "";
    char* prev = "file ";
    char* succ = ". ";
    char* p = filename? prev : empty;
    char* s = filename? succ : empty;
    char* f = filename? filename : empty;
	
    fprintf (OTHER_STREAM, "%s%s%s""width %d, height %d, max %lf, min %lf.\n",
	    p, f, s, width, height, max, min);
    return;
}

BMPReader::BMPReader (const char *filename)
{
    img.assign (filename);
    width = img.width ();
    height = img.height ();
    set_maxmin();
    // print_info (filename);
}

BMPReader::~BMPReader ()
{

}

double BMPReader::get_pixel (unsigned int x, unsigned int y)
{
    y = height - y - 1;
    return img(x,y);
}

void BMPReader::print_info (char *filename = 0)
{
    fprintf (OTHER_STREAM, "BMPReader: ");
    // print file only if not null
    char* empty = "";
    char* prev = "file ";
    char* succ = ". ";
    char* p = filename? prev : empty;
    char* s = filename? succ : empty;
    char* f = filename? filename : empty;
	
    fprintf (OTHER_STREAM, "%s%s%s""width %d, height %d, max %lf, min %lf.\n",
	    p, f, s, width, height, max, min);
    return;
}

TIFReader::TIFReader (const char *filename)
{
    img.assign (filename);
    width = img.width ();
    height = img.height ();
    set_maxmin();
    // print_info (filename);
}

TIFReader::~TIFReader ()
{

}

double TIFReader::get_pixel (unsigned int x, unsigned int y)
{
    y = height - y - 1;
    return img(x,y);
}

void TIFReader::print_info (char *filename = 0)
{
    fprintf (OTHER_STREAM, "TIFReader: ");

    // print file only if not null
    char* empty = "";
    char* prev = "file ";
    char* succ = ". ";
    char* p = filename? prev : empty;
    char* s = filename? succ : empty;
    char* f = filename? filename : empty;
	
    fprintf (OTHER_STREAM, "%s%s%s""width %d, height %d, max %lf, min %lf.\n",
	    p, f, s, width, height, max, min);
    return;
}

HGTReader::~HGTReader ()
{
    free (data);
}

HGTReader::HGTReader(const char* filename)
{
    FILE* fp;
    int byte;
    int len;

    fp = fopen (filename, "r");
    if (fp == NULL)
	eprintx (2, "Could not open file `%s'. %s\n", filename, strerror (errno));
    

    for (len = 0; (byte = getc (fp)) != EOF; len++)
	;

    byte_length = len;
    length = len / 2;

    rewind (fp);

    data = (short int*) malloc (length * sizeof (short int));
    width = height = (int) sqrt (length);

    for (len = 0; len < length; len++)
    {
	char b1 = getc (fp);
	char b2 = getc (fp);
	char* p = (char*) &data[len];
	*p = b2;
	*(p+1) = b1;
    }

    set_maxmin();
    // print_info (filename);
    fclose (fp);
}

double HGTReader::get_pixel(unsigned int x, unsigned int y)
{
    y = height - 1 - y;
    // short pixel = data[(x * height) + y];
    short pixel = data[(y * width) + x];
    
    if (pixel == SHRT_MIN)
    {
	eprint ("Found HGT hole: %d,%d\n", x, y);
	return -DBL_MAX;
    }
    return pixel;
}


void HGTReader::print_info (char *filename = 0)
{
    // print file only if not null
    char* empty = "";
    char* prev = "file ";
    char* succ = ". ";
    char* p = filename? prev : empty;
    char* s = filename? succ : empty;
    char* f = filename? filename : empty;
	
    fprintf (OTHER_STREAM, "HGTReader: %s%s%s length %d, byte_length %d, "
	    "width %d, height %d, max %lf, min %lf.\n",
	    p, f, s,
	    length, byte_length, width, height,
	    max, min);
    return;
}


ASCReader::~ASCReader ()
{
    free (data);
}

ASCReader::ASCReader(const char* filename)
{
    FILE* fp;
    fp = fopen (filename, "r");
    if (fp == NULL)
	eprintx (2, "Could not open file `%s'. %s\n", filename, strerror (errno));
    
    fscanf (fp, "ncols %d\n", &width);
    fscanf (fp, "nrows %d\n", &height);
    fscanf (fp, "xllcorner %lf\n", &xllcorner);
    fscanf (fp, "yllcorner %lf\n", &yllcorner);
    fscanf (fp, "cellsize %lf\n", &cellsize);
    fscanf (fp, "NODATA_value %lf\n", &nodata_value);

    data = (double*) malloc (width * height * sizeof (double));

    int len = 0;
    for (unsigned w = 0; w < width; w++)
	for (unsigned h = 0; h < height; h++)
	{
	    double value;
	    fscanf (fp, h == height - 1? "%lf\n" : "%lf ", &value);
	    data[len++] = value;
	}

    set_maxmin();
    // print_info (filename);
    fclose (fp);
}

double ASCReader::get_pixel(unsigned int x, unsigned int y)
{
    if (x >= width || y >= height)
	eprintx (-1,"out of bounds access: (%d,%d). w: %d, h: %d.\n",
		 x, y, width, height);
    
    y = height - 1 - y;
    // short pixel = data[(x * height) + y];

    short pixel = data[(y * width) + x];
    
    if (pixel == nodata_value)
    {
	eprint ("Found ASC hole: %d,%d\n", x, y);
	return -DBL_MAX;
    }
    return pixel;
}


void ASCReader::print_info (char *filename = 0)
{
    // print file only if not null
    char* empty = "";
    char* prev = "file ";
    char* succ = ". ";
    char* p = filename? prev : empty;
    char* s = filename? succ : empty;
    char* f = filename? filename : empty;
	
    fprintf (OTHER_STREAM, "ASCReader: %s%s%s "
	     "xllcorner: %lf, yllcorner: %lf, cellsize: %lf, nodata_value: %lf. "
	     "width %d, height %d, max %lf, min %lf.\n",
	     p, f, s,
	     xllcorner, yllcorner, cellsize, nodata_value,
	     width, height,
	     max, min);
    return;
}
