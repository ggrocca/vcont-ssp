#include <stdio.h>
#include <stdlib.h>
#include "rastertiff.h"
#include <assert.h>

////////////////////////////////////
/// raster rgb 8 bit per channel ///
////////////////////////////////////

raster8_t raster8_read (TIFF* image, uint32 w, uint32 h)
{
    if (image) 
    {
	size_t npixels;
	raster8_t raster;

	npixels = w * h;
	raster = (raster8_t) _TIFFmalloc(npixels * sizeof (pix8_t));

	if (raster == NULL)
	{
	    fprintf (stderr, "raster8_read: error in raster allocation\n");
	    return NULL;
	}

	if (!TIFFReadRGBAImage(image, w, h, raster, 0))
	{
	    fprintf (stderr, "raster8_read: error in TIFFReadRGBAImage\n");
	    return NULL;
	}

	return raster;
    }

    fprintf (stderr, "raster8_read: invalid TIFF image\n");
    return NULL;    
}

void raster8_print (raster8_t raster, uint32 w, uint32 h)
{
    uint32 i;
    unsigned char r,g,b;

    if (raster == NULL)
    {
	fprintf (stderr, "raster8_print: raster is NULL\n");
	return;
    }

    printf ("-----------------------------------------\n");
    for (i = 0; i < w * h; i++)
    {
	void* p = &raster[i];
	r = *((unsigned char *)p);
	g = *((unsigned char *)p+1);
	b = *((unsigned char *)p+2);
#ifdef VERBOSE
	printf ("<%d: %d-%d-%d>%c", i, r, g, b, ((i+1) % (w-1))? ' ' : '\n');
#else
	printf ("%3d%c", r, ((i+1) % w)? ' ' : '\n');
#endif
    }
    printf ("-----------------------------------------\n");
}

void raster8_free (raster8_t raster)
{
    _TIFFfree(raster);
}


////////////////////
/// handle tiff  ///
////////////////////

TIFF* tiff_open_read (char* file)
{
    TIFF* tiffp;

    if ((tiffp = TIFFOpen (file, "r")) == NULL)
    {
	fprintf (stderr, "tiff_open_read(): Could not open %s\n", file);
	exit (42);
    }
    
    return tiffp;
}

TIFF* tiff_open_write (char* file)
{
    TIFF* tiffp;

    if ((tiffp = TIFFOpen (file, "w")) == NULL)
    {
	fprintf (stderr, "tiff_open_read(): Could not open %s\n", file);
	exit (42);
    }

    return tiffp;
}

void tiff_tag_read (TIFF* in, int* width, int* height)
{
    int _length;
    int _width;

    TIFFGetField (in, TIFFTAG_IMAGELENGTH, &_length);
    TIFFGetField (in, TIFFTAG_IMAGEWIDTH, &_width);

    *width = _length;
    *height = _width;
}

void tiff16_tag_write (TIFF* out, int width, int height){
    uint32 bps = 16;
    uint32 spp = 3;

    TIFFSetField (out, TIFFTAG_IMAGEWIDTH, width);  // set the width of the image
    TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);    // set the height of the image
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, spp);   // set number of channels per pixel
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, bps);    // set the size of the channels
    //TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);    // set the origin of the image.
    //   Some other essential fields to set that you do not have to understand for now.
    TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    TIFFSetField(out, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(out, 0));
}

#define	CopyField(tag, v) \
    if (TIFFGetField(in, tag, &v)) TIFFSetField(out, tag, v)
#define	CopyField2(tag, v1, v2) \
    if (TIFFGetField(in, tag, &v1, &v2)) TIFFSetField(out, tag, v1, v2)
#define	CopyField3(tag, v1, v2, v3) \
    if (TIFFGetField(in, tag, &v1, &v2, &v3)) TIFFSetField(out, tag, v1, v2, v3)
#define	CopyField4(tag, v1, v2, v3, v4) \
    if (TIFFGetField(in, tag, &v1, &v2, &v3, &v4)) TIFFSetField(out, tag, v1, v2, v3, v4)

static void
cpTag(TIFF* in, TIFF* out, uint16 tag, uint16 count, TIFFDataType type)
{
	switch (type) {
	case TIFF_SHORT:
		if (count == 1) {
			uint16 shortv;
			CopyField(tag, shortv);
		} else if (count == 2) {
			uint16 shortv1, shortv2;
			CopyField2(tag, shortv1, shortv2);
		} else if (count == 4) {
			uint16 *tr, *tg, *tb, *ta;
			CopyField4(tag, tr, tg, tb, ta);
		} else if (count == (uint16) -1) {
			uint16 shortv1;
			uint16* shortav;
			CopyField2(tag, shortv1, shortav);
		}
		break;
	case TIFF_LONG:
		{ uint32 longv;
		  CopyField(tag, longv);
		}
		break;
	case TIFF_RATIONAL:
		if (count == 1) {
			float floatv;
			CopyField(tag, floatv);
		} else if (count == (uint16) -1) {
			float* floatav;
			CopyField(tag, floatav);
		}
		break;
	case TIFF_ASCII:
		{ char* stringv;
		  CopyField(tag, stringv);
		}
		break;
	case TIFF_DOUBLE:
		if (count == 1) {
			double doublev;
			CopyField(tag, doublev);
		} else if (count == (uint16) -1) {
			double* doubleav;
			CopyField(tag, doubleav);
		}
		break;
          default:
                TIFFError(TIFFFileName(in),
                          "Data type %d is not supported, tag %d skipped.",
                          tag, type);
	}
}

static struct cpTag {
    uint16	tag;
    uint16	count;
    TIFFDataType type;
} tags[] = {
    /* { TIFFTAG_IMAGEWIDTH,             1, TIFF_LONG }, */
    /* { TIFFTAG_IMAGELENGTH,            1, TIFF_LONG }, */
    { TIFFTAG_IMAGEWIDTH,             1, TIFF_SHORT },
    { TIFFTAG_IMAGELENGTH,            1, TIFF_SHORT },
    { TIFFTAG_SUBFILETYPE,            1, TIFF_LONG },
    { TIFFTAG_BITSPERSAMPLE,          1, TIFF_SHORT },
    { TIFFTAG_SAMPLESPERPIXEL,        1, TIFF_SHORT },
    { TIFFTAG_COMPRESSION,            1, TIFF_SHORT },
    { TIFFTAG_PHOTOMETRIC,            1, TIFF_SHORT },
    { TIFFTAG_MAKE,                   1, TIFF_ASCII },
    { TIFFTAG_MODEL,                  1, TIFF_ASCII },
    { TIFFTAG_ROWSPERSTRIP,           1, TIFF_LONG },
    /* { TIFFTAG_STRIPOFFSETS,           1, TIFF_LONG }, */
    /* { TIFFTAG_STRIPBYTECOUNTS,        1, TIFF_LONG }, */
    { TIFFTAG_XRESOLUTION,            1, TIFF_RATIONAL },
    { TIFFTAG_YRESOLUTION,            1, TIFF_RATIONAL },
    { TIFFTAG_PLANARCONFIG,           1, TIFF_SHORT },
    { TIFFTAG_RESOLUTIONUNIT,         1, TIFF_SHORT },
    { TIFFTAG_DATETIME,               1, TIFF_ASCII },

    /* { TIFFTAG_THRESHHOLDING,          1, TIFF_SHORT }, */
    /* { TIFFTAG_DOCUMENTNAME,           1, TIFF_ASCII }, */
    /* { TIFFTAG_IMAGEDESCRIPTION,       1, TIFF_ASCII }, */
    /* { TIFFTAG_MAKE,                   1, TIFF_ASCII }, */
    /* { TIFFTAG_MODEL,                  1, TIFF_ASCII }, */
    /* { TIFFTAG_MINSAMPLEVALUE,         1, TIFF_SHORT }, */
    /* { TIFFTAG_MAXSAMPLEVALUE,         1, TIFF_SHORT }, */
    /* { TIFFTAG_XRESOLUTION,            1, TIFF_RATIONAL }, */
    /* { TIFFTAG_YRESOLUTION,            1, TIFF_RATIONAL }, */
    /* { TIFFTAG_PAGENAME,               1, TIFF_ASCII }, */
    /* { TIFFTAG_XPOSITION,              1, TIFF_RATIONAL }, */
    /* { TIFFTAG_YPOSITION,              1, TIFF_RATIONAL }, */
    /* { TIFFTAG_RESOLUTIONUNIT,         1, TIFF_SHORT }, */
    /* { TIFFTAG_SOFTWARE,               1, TIFF_ASCII }, */
    /* { TIFFTAG_DATETIME,               1, TIFF_ASCII }, */
    /* { TIFFTAG_ARTIST,                 1, TIFF_ASCII }, */
    /* { TIFFTAG_HOSTCOMPUTER,           1, TIFF_ASCII }, */
    /* { TIFFTAG_WHITEPOINT,             (uint16) -1, TIFF_RATIONAL }, */
    /* { TIFFTAG_PRIMARYCHROMATICITIES,  (uint16) -1,TIFF_RATIONAL }, */
    /* { TIFFTAG_HALFTONEHINTS,          2, TIFF_SHORT }, */
    /* { TIFFTAG_INKSET,                 1, TIFF_SHORT }, */
    /* { TIFFTAG_DOTRANGE,               2, TIFF_SHORT }, */
    /* { TIFFTAG_TARGETPRINTER,          1, TIFF_ASCII }, */
    /* { TIFFTAG_SAMPLEFORMAT,           1, TIFF_SHORT }, */
    /* { TIFFTAG_YCBCRCOEFFICIENTS,      (uint16) -1,TIFF_RATIONAL }, */
    /* { TIFFTAG_YCBCRSUBSAMPLING,       2, TIFF_SHORT }, */
    /* { TIFFTAG_YCBCRPOSITIONING,       1, TIFF_SHORT }, */
    /* { TIFFTAG_REFERENCEBLACKWHITE,    (uint16) -1,TIFF_RATIONAL }, */
    /* { TIFFTAG_EXTRASAMPLES,           (uint16) -1, TIFF_SHORT }, */
    /* { TIFFTAG_SMINSAMPLEVALUE,        1, TIFF_DOUBLE }, */
    /* { TIFFTAG_SMAXSAMPLEVALUE,        1, TIFF_DOUBLE }, */
    /* { TIFFTAG_STONITS,                1, TIFF_DOUBLE }, */
};
#define	NTAGS	(sizeof (tags) / sizeof (tags[0]))

#define	CopyTag(tag, count, type)	cpTag(in, out, tag, count, type)

void tag_cp(TIFF* in, TIFF* out)
{
    struct cpTag* p;

    for (p = tags; p < &tags[NTAGS]; p++)
	CopyTag(p->tag, p->count, p->type);

    { // copy ICC profile
	uint32 len32;
	void** data;
	if (TIFFGetField(in, TIFFTAG_ICCPROFILE, &len32, &data))
	    TIFFSetField(out, TIFFTAG_ICCPROFILE, len32, data);
    }
}


/////////////////////////////////////
/// raster rgb 16 bit per channel ///
/////////////////////////////////////
pix16_t pix16_mono (uint16 v)
{
    pix16_t p;
    
    p.r = p.g = p.b = v;
    
    return p;
}

pix16_t raster16_get (raster16_t r, uint32 x, uint32 y, uint32 w, uint32 h)
{
    if (x < w && y < h)
    {
	y = h - 1 - y;
	/* // return r[(x * w) + y]; */
	return r[(y * w) + x];
	/* return r[(x * h) + y]; */
    }
    
    fprintf (stderr, "raster16_get: out of raster pixel access\n");
    return pix16_mono (0);
}

void raster16_set (raster16_t r, uint32 x, uint32 y, uint32 w, uint32 h, pix16_t v)
{
    if (x < w && y < h)
    {
	y = h - 1 - y;
	/* //r[(x * w) + y] = v; */
	r[(y * w) + x] = v;
	/* r[(x * h) + y] = v; */
    }
    else
	fprintf (stderr, "raster16_set: out of raster pixel access. <%d, %d> <%d, %d>\n", x, y, w, h);	
}

raster16_t raster16_new (uint32 w, uint32 h)
{
    size_t npixels;
    raster16_t raster;

    npixels = w * h;
    raster = (raster16_t) _TIFFmalloc(npixels * sizeof (pix16_t));
	
    if (raster == NULL)
    {
	fprintf (stderr, "raster16_read: error in raster allocation\n");
	return NULL;
    }

    return raster;
}

raster16_t raster16_read (TIFF* image, uint32 w, uint32 h)
{
    int npixels;
    raster16_t raster;
    int raster_pos;

    tdata_t buf;
    size_t bufsize;
    tstrip_t strip;
    uint32 i;

    uint16 r,g,b;

    if (!image) 
    {
	fprintf (stderr, "raster16_read: invalid TIFF image\n");
	return NULL;
    }

    npixels = w * h;
    raster_pos = 0;
    raster = (raster16_t) _TIFFmalloc(npixels * sizeof (pix16_t));
	
    if (raster == NULL)
    {
	fprintf (stderr, "raster16_read: error in raster allocation\n");
	return NULL;
    }

    buf = _TIFFmalloc(bufsize = TIFFStripSize(image));
    for (strip = 0; strip < TIFFNumberOfStrips(image); strip++)
    {
	TIFFReadEncodedStrip(image, strip, buf, (tsize_t) -1);

	// GG warn: should consider for fill order, planar and all other things
	// GG warn: assuming out of nowhere a sequence of 3 16 bit pixels
	for (i = 0; i+5 < bufsize; i = i + 6)
	{
	    r = *(uint16*)(buf + i);
	    g = *(uint16*)(buf + i + 2);
	    b = *(uint16*)(buf + i + 4);
	    if (raster_pos < npixels)
	    {
		raster[raster_pos].r = r;
		raster[raster_pos].g = g;
		raster[raster_pos].b = b;
		raster_pos++;
	    }

	    /* This could occur if number of strips is not divisible
	       by image height and is not really and error nor a
	       problem. */

	    /* else */
	    /* { */
	    /* 	fprintf (stderr, "raster16_read: warning, there's more strips than raster space" */
	    /* 		 " raster_pos: %d, npixels: %d\n", raster_pos, npixels); */
	    /* 	assert (0); */
	    /* } */
	}
    }

    _TIFFfree(buf);
    return raster;
}

void raster16_write (TIFF* image, raster16_t raster, uint32 w, uint32 h)
{
    int npixels;
    int raster_pos;

    tdata_t buf;
    size_t bufsize;
    tstrip_t strip;
    uint32 i;

    uint16 r,g,b;

    if (!image) 
    {
	fprintf (stderr, "raster16_write: invalid TIFF image\n");
	return;
    }

    if (raster == NULL)
    {
	fprintf (stderr, "raster16_write: NULL raster.\n");
	return;
    }

    npixels = w * h;
    raster_pos = 0;

    buf = _TIFFmalloc(bufsize = TIFFStripSize(image));
    /* printf ("bufsize: %d, numberOfStrips: %d, stripSize: %d\n", (int) bufsize, */
    /* 	    (int) TIFFNumberOfStrips(image), (int) TIFFStripSize(image)); */
    for (strip = 0; strip < TIFFNumberOfStrips(image); strip++)
    {
	for (i = 0; i+5 < bufsize; i = i + 6)
	{
	    r = g = b = 0;
	    if (raster_pos < npixels)
	    {
		r = raster[raster_pos].r;
		g = raster[raster_pos].g;
		b = raster[raster_pos].b;
		raster_pos++;
	    }

	    /* This could occur if number of strips is not divisible
	       by image height and is not really and error nor a
	       problem. */

	    /* else */
	    /* { */
	    /* 	fprintf (stderr, "raster16_write: warning, there's more strips than raster space." */
	    /* 		 " raster_pos: %d, npixels: %d\n", raster_pos, npixels); */
	    /* 	assert (0); */
	    /* } */

	    *(uint16*)(buf + i) = r;
	    *(uint16*)(buf + i + 2) = g;
	    *(uint16*)(buf + i + 4) = b;
	}
	
	TIFFWriteEncodedStrip(image, strip, buf, TIFFStripSize(image));
    }
    _TIFFfree(buf);

    if (raster_pos != npixels)
	fprintf (stderr, "raster16_write: warning! npixels == %d, raster_pos == %d.\n", npixels, raster_pos);

    return;
}

raster16_t raster16_copy (raster16_t r_in, uint32 w, uint32 h)
{
    size_t npixels;
    raster16_t r_out;
    
    if (r_in == NULL)
    {
	fprintf (stderr, "raster16_copy: r_in is NULL\n");
	return NULL;
    }

    npixels = w * h;
    r_out = (raster16_t) _TIFFmalloc (npixels * sizeof (pix16_t));

    if (r_out == NULL)
    {
	fprintf (stderr, "raster16_copy: error in raster allocation\n");
	return NULL;
    }

    _TIFFmemcpy (r_out, r_in, npixels * sizeof (pix16_t));

    return r_out;
}

int raster16_diff (raster16_t ra, raster16_t rb, uint32 w, uint32 h, int verbose_level)
{
    if (ra == NULL)
    {
	fprintf (stderr, "raster16_print: ra is NULL\n");
	return -1;
    }
    if (rb == NULL)
    {
	fprintf (stderr, "raster16_print: rb is NULL\n");
	return -1;
    }

    int count = 0;

    for (unsigned i = 0; i < w; i++)
	for (unsigned j = 0; j < h; j++)
	{
	    pix16_t p[2];
	    char mono[2];
	    p[0] = raster16_get (ra, i, j, w, h);
	    p[1] = raster16_get (rb, i, j, w, h);

	    if (p[0].r != p[1].r || p[0].g != p[1].g || p[0].b != p[1].b)
	    {
		count++;

		if (verbose_level >= 2)
		{
		    printf ("[%5d][%5d] ", i, j);
		
		    for (int k = 0; k < 2; k++)
		    {
			printf (k == 0? "A: " : "B: ");

			if ((mono[k] = (p[k].r == p[k].g && p[k].r == p[k].b)))
			    printf ("(-----,%5d,-----)", p[k].r);
			else
			    printf ("(%5d,%5d,%5d)", p[k].r, p[k].g, p[k].b);
		    
			if (k == 0)
			    printf ("  :::  ");
		    }
		
		    if (mono[0] && mono[1])
			printf ("  :::  A-B: <%5d>", p[0].r - p[1].r);
		    printf ("\n");
		}
	    }
	}

    return count;
}


void raster16_print (raster16_t ra, uint32 w, uint32 h)
{
    if (ra == NULL)
    {
	fprintf (stderr, "raster16_print: raster is NULL\n");
	return;
    }

    for (unsigned i = 0; i < w; i++)
	for (unsigned j = 0; j < h; j++)
	{
	    pix16_t p = raster16_get (ra, i, j, w, h);

	    printf ("[%5d][%5d] ", i, j);

	    if (p.r == p.g && p.r == p.b)
		printf ("(-----,%5d,-----)\n", p.r);
	    else
		printf ("(%5d,%5d,%5d)\n", p.r, p.g, p.b);

	}

/* uint32 i; */

/*     printf ("-----------------------------------------\n"); */
/*     for (i = 0; i < w * h; i++) */
/*     { */
/* #ifdef VERBOSE */
/* 	printf ("<%d: %d-%d-%d>%c", i, raster[i].r, raster[i].g, raster[i].b, */
/* 		((i+1) % (w-1))? ' ' : '\n'); */
/* #else */
/* 	printf ("%5d%c", raster[i].r, ((i+1) % w)? ' ' : '\n'); */
/* #endif */
/*     }     */
/*     printf ("-----------------------------------------\n"); */
}

void raster16_print8 (raster16_t raster, uint32 w, uint32 h)
{
    uint32 i;
    unsigned char r,g,b;

    if (raster == NULL)
    {
	fprintf (stderr, "raster16_print: raster is NULL\n");
	return;
    }

    printf ("-----------------------------------------\n");
    for (i = 0; i < w * h; i++)
    {
	r = *(((unsigned char *) &raster[i].r) + 1);
	g = *(((unsigned char *) &raster[i].g) + 1);
	b = *(((unsigned char *) &raster[i].b) + 1);
#ifdef VERBOSE
	printf ("<%d: %d-%d-%d>%c", i, r, g, b,
		((i+1) % (w-1))? ' ' : '\n');
#else
	printf ("%5d%c", r, ((i+1) % w)? ' ' : '\n');
#endif
    }    
    printf ("-----------------------------------------\n");
}

void raster16_free (raster16_t raster)
{
    _TIFFfree(raster);
}
