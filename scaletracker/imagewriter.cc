#include "imagewriter.hh"

static pix16_t pix_red = {65535, 0, 0};
static pix16_t pix_green = {0, 65535, 0};
static pix16_t pix_blue = {0, 0, 65535};
static pix16_t pix_yellow = {65535,65535,0};
static pix16_t pix_magenta = {32768,0,32768};
static pix16_t pix_black = {0,0,0};

ImageWriter::ImageWriter (Dem *dem)
{
    raster_out = raster16_new (dem->width, dem->height);
    width = dem->width;
    height = dem->height;
    int max = dem->max;
    int min = dem->min;
    int m = (65536 / (max - min));

    printf ("max: %d, min: %d, multiplier: %d\n", max, min, m);

    for (int i = 0; i < dem->width; i++)
    	for (int j = 0; j < dem->height; j++)
	{
	    int v = ((unsigned short) (*dem)(Coord (i, j)));
	    v -= (int) dem->min;
    	    raster16_set (raster_out, i, j, dem->width, dem->height,
			  pix16_mono (v * m));
	}
}

ImageWriter::~ImageWriter ()
{
    raster16_free (raster_out);
}

void ImageWriter::draw_points (std::vector<CriticalPoint>* points)
{
	for (unsigned i = 0; i < (*points).size(); i++)
	{
	    pix16_t pix_to_draw =
		(*points)[i].t == MAX? pix_red :
		(*points)[i].t == MIN? pix_blue :
		(*points)[i].t == SA2? pix_green :
		(*points)[i].t == SA3? pix_yellow :
		(*points)[i].t == EQU? pix_black :
		raster16_get (raster_out, (*points)[i].c.x, (*points)[i].c.y,
			      width, height);
	    raster16_set (raster_out, (*points)[i].c.x, (*points)[i].c.y,
			  width, height, pix_to_draw);
	}
}

void ImageWriter::draw_lines (Grid<char>* ilines)
{
    for (int i = 0; i < ilines->width; i++)
	for (int j = 0; j < ilines->height; j++)
	    if ((*ilines)(i, j) != 0)
		raster16_set (raster_out, i, j, width, height, pix_magenta);
}

void ImageWriter::write (char* name, char* mark, int count)
{
#define __FNLEN 128
    char image_out[__FNLEN] = {'\0'};
    if (count < 0)
	snprintf (image_out, __FNLEN, "%s%s.%s", name, mark, "tif");
    else
	snprintf (image_out, __FNLEN, "%s%s%d.%s", name, mark, count, "tif");
#undef __FNLEN

    TIFF* tiff_out = tiff_open_write (image_out);
    tiff16_tag_write (tiff_out, width, height);
    raster16_write (tiff_out, raster_out, width, height);
    TIFFClose (tiff_out);
}
