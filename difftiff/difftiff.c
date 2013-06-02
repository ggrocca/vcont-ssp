#include "../common/rastertiff.h"

int main (int argc, char* argv[])
{
    if (argc != 3)
    {
	fprintf (stderr, "Usage: difftiff file1 file2\n");
	exit (1);
    }

    TIFF* t1 = tiff_open_read (argv[1]);
    TIFF* t2 = tiff_open_read (argv[2]);
    
    int w, h;
    int _w, _h;

    tiff_tag_read (t1, &w, &h);
    tiff_tag_read (t2, &_w, &_h);
    
    printf ("image :[%5d][%5d]\n", w, h);

    if (w != _w || h != _h)
    {
	fprintf (stderr, "Error, second image :[%5d][%5d]\n", _w, _h);
	exit (1);
    }

    raster16_t r1 = raster16_read (t1, w, h);
    raster16_t r2 = raster16_read (t2, w, h);

    raster16_diff (r1, r2, w, h);

    raster16_free (r1);
    raster16_free (r2);
}
