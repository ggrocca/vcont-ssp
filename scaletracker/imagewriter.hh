#ifndef _IMAGEWRITER_HH
#define _IMAGEWRITER_HH

#include "../common/dem.hh"
#include "../common/rastertiff.h"

class ImageWriter {
private:
    raster16_t raster_out;
    unsigned width, height;

public:
    ImageWriter (Dem *image, int mult = 0);
    ~ImageWriter ();
    
    void draw_points (std::vector<CriticalPoint>* points);
    void draw_lines (Grid<char>* ilines);

    void write (char* name, char* mark, int count);
};

#endif // _IMAGEWRITER_HH
