#ifndef RASTERTIFF_H
#define RASTERTIFF_H

#include <tiffio.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    ////////////////////
    /// handle tiff  ///
    ////////////////////

    TIFF* tiff_open_read (char* file);
    TIFF* tiff_open_write (char* file);
    void tiff_tag_read (TIFF* out, int* width, int* height);
    void tiff16_tag_write (TIFF* out, int width, int height);
    void tiff_tag_cp (TIFF* in, TIFF* out);

    ////////////////////////////////////
    /// raster rgb 8 bit per channel ///
    ////////////////////////////////////

    typedef uint32 pix8_t;
    typedef pix8_t* raster8_t;

    raster8_t raster8_read (TIFF* image, uint32 w, uint32 h);
    void raster8_print (raster8_t raster, uint32 w, uint32 h);
    void raster8_free (raster8_t raster);


    /////////////////////////////////////
    /// raster rgb 16 bit per channel ///
    /////////////////////////////////////

    typedef struct {
	uint16 r;
	uint16 g;
	uint16 b;
    } pix16_t;
    typedef pix16_t* raster16_t;

    pix16_t pix16_mono (uint16 v);
    pix16_t raster16_get (raster16_t r, uint32 x, uint32 y, uint32 w, uint32 h);
    void raster16_set (raster16_t r, uint32 x, uint32 y, uint32 w, uint32 h, pix16_t v);
    raster16_t raster16_new (uint32 w, uint32 h);
    raster16_t raster16_read (TIFF* image, uint32 w, uint32 h);
    raster16_t raster16_copy (raster16_t r_in, uint32 w, uint32 h);
    void raster16_write (TIFF* image, raster16_t raster, uint32 w, uint32 h);
    void raster16_diff (raster16_t ra, raster16_t rb, uint32 w, uint32 h);
    void raster16_print (raster16_t raster, uint32 w, uint32 h);
    void raster16_print8 (raster16_t raster, uint32 w, uint32 h);
    void raster16_free (raster16_t raster);

#ifdef __cplusplus
}
#endif

#endif /* RASTERTIFF_H */
