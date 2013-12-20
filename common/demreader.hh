#ifndef _DEMREADER_HH
#define _DEMREADER_HH


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <limits.h>

#include <vector>

#include "utils.hh"
#define cimg_display 0
#include "CImg.h"
#include "debug.h"


class DEMReader {
public:
    unsigned width, height;
    int max, min;

    DEMReader () {}
    virtual ~DEMReader () {}
    // get_pixel returns INT_MIN in case of error or missing data
    virtual int get_pixel (unsigned int x, unsigned int y) = 0; 
    virtual void print_info (char* filename = 0) = 0;
    void set_maxmin ();
};

class HGTReader : public DEMReader {
public:
    int byte_length;
    int length;
    short int* data;

    HGTReader (const char *filename);
    virtual ~HGTReader ();
    int get_pixel (unsigned int x, unsigned int y);
    void print_info (char* filename);
    // multiplier was: 13;
};

class PNGReader : public DEMReader {
    cimg_library::CImg<unsigned short> img;
public:
    PNGReader (const char *filename);
    virtual ~PNGReader ();
    int get_pixel (unsigned int x, unsigned int y);
    void print_info (char* filename);
    // multiplier was: 1;
};

class BMPReader : public DEMReader {
    cimg_library::CImg<unsigned short> img;
public:
    BMPReader (const char *filename);
    virtual ~BMPReader ();
    int get_pixel (unsigned int x, unsigned int y);
    void print_info (char* filename);
    // multiplier was: 1;
};

class TIFReader : public DEMReader {
    cimg_library::CImg<unsigned short> img;
public:
    TIFReader (const char *filename);
    virtual ~TIFReader ();
    int get_pixel (unsigned int x, unsigned int y);
    void print_info (char* filename);
    // multiplier was: 1;
};


class DEMSelector {
public:
    typedef enum {PNG=0, BMP, TIF, HGT} Format;
    static const char* fmts[];

    static DEMReader* get (const char* filename)
    {
	char *ext = get_ext ((char*)filename);
	int fmtnum = 0;
	for (const char** p = fmts; **p != '\0'; p++, fmtnum++)
	    if (!strcmp (*p, ext))
		return get (filename, static_cast<Format>(fmtnum));
	eprint ("File `%s': format `%s' not supported.\n", filename, ext);
	return 0;
    }

    static DEMReader* get (const char* filename, Format fmt)
    {
	switch (fmt)
	{
	case PNG:
	    return new PNGReader (filename);
	case BMP:
	    return new BMPReader (filename);
	case TIF:
	    return new TIFReader (filename);
	case HGT:
	    return new HGTReader (filename);
	default:
	    eprint ("%s","Wrong format code\n");
	    return 0;
	}
    }

    static void destroy (DEMReader* demr)
    {
	delete demr;
    }
};


#endif // _DEMREADER_HH
