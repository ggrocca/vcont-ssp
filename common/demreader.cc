#include "math.h"
#include "demreader.hh"
#include <float.h>


void DEMReader::set_maxmin ()
{
    max = INT_MIN;
    min = INT_MAX;

    for (unsigned i = 0; i < width; i++)
	for (unsigned j = 0; j < height; j++)
	{
	    int v = get_pixel (i, j);
	    if (v < min)
		min = v;
	    if (v > max)
		max = v;
	}
}


const char* DEMSelector::fmts[] = {"png", "bmp", "tif", "hgt", "\0"};

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

int PNGReader::get_pixel (unsigned int x, unsigned int y)
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
	
    fprintf (OTHER_STREAM, "%s%s%s""width %d, height %d, max %d, min %d.\n",
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

int BMPReader::get_pixel (unsigned int x, unsigned int y)
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
	
    fprintf (OTHER_STREAM, "%s%s%s""width %d, height %d, max %d, min %d.\n",
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

int TIFReader::get_pixel (unsigned int x, unsigned int y)
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
	
    fprintf (OTHER_STREAM, "%s%s%s""width %d, height %d, max %d, min %d.\n",
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
}

int HGTReader::get_pixel(unsigned int x, unsigned int y)
{
    y = height - 1 - y;
    // short pixel = data[(x * height) + y];
    short pixel = data[(y * width) + x];
    
    if (pixel == SHRT_MIN)
    {
	eprint ("Found HGT hole: %d,%d\n", x, y);
	return INT_MIN;
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
	    "width %d, height %d, max %d, min %d.\n",
	    p, f, s,
	    length, byte_length, width, height,
	    max, min);
    return;
}
