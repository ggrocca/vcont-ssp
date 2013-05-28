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

#include "CImg.h"
#include "debug.h"


class DEMReader {
public:
    unsigned width, height;
    int max, min;

    DEMReader () {}
    virtual ~DEMReader () {}
    virtual int get_pixel (unsigned int x, unsigned int y) = 0;
    virtual void print_info (char* filename = 0) = 0;
    void set_maxmin ();
};

class HGTReader : public DEMReader {
public:
    int byte_length;
    int length;
    short int* data;

    HGTReader (char *filename);
    virtual ~HGTReader ();
    int get_pixel (unsigned int x, unsigned int y);
    void print_info (char* filename);
    // multiplier was: 13;
};

class PNGReader : public DEMReader {
    cimg_library::CImg<unsigned short> img;
public:
    PNGReader (char *filename);
    virtual ~PNGReader ();
    int get_pixel (unsigned int x, unsigned int y);
    void print_info (char* filename);
    // multiplier was: 1;
};

class BMPReader : public DEMReader {
    cimg_library::CImg<unsigned short> img;
public:
    BMPReader (char *filename);
    virtual ~BMPReader ();
    int get_pixel (unsigned int x, unsigned int y);
    void print_info (char* filename);
    // multiplier was: 1;
};

class TIFReader : public DEMReader {
    cimg_library::CImg<unsigned short> img;
public:
    TIFReader (char *filename);
    virtual ~TIFReader ();
    int get_pixel (unsigned int x, unsigned int y);
    void print_info (char* filename);
    // multiplier was: 1;
};


class DEMSelector {
public:
    typedef enum {PNG=0, BMP, TIF, HGT} Format;
    static const char* fmts[];

    static DEMReader* get (char* filename)
    {
	char *ext = get_ext (filename);
	int fmtnum = 0;
	for (const char** p = fmts; **p != '\0'; p++, fmtnum++)
	    if (!strcmp (*p, ext))
		return get (filename, static_cast<Format>(fmtnum));
	eprint ("Format `%s' not supported.\n", ext);
	return 0;
    }

    static DEMReader* get (char* filename, Format fmt)
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

private:
    
    static char* get_ext (char* fn)
    {
	char* p;
	char* r = 0;
	for (p = fn; *p != '\0'; p++)
	    if (*p == '.')
		r = p + 1;
	return r;
    }

};


#endif // _DEMREADER_HH
