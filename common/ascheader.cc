#include "ascheader.hh"

#include <stdio.h>
#include <stdlib.h>

ASCHeader::ASCHeader () :
    width (0), height (0), xllcorner (0), yllcorner (0),
    cellsize (0), nodata_value (0) {}

ASCHeader::ASCHeader (ASCReader& ascr) :
    width (ascr.width), height (ascr.height),
    xllcorner (ascr.xllcorner), yllcorner (ascr.yllcorner),
    cellsize (ascr.cellsize), nodata_value (ascr.nodata_value) {}

ASCHeader::ASCHeader (ASCHeader& asch) :
    width (asch.width), height (asch.height),
    xllcorner (asch.xllcorner), yllcorner (asch.yllcorner),
    cellsize (asch.cellsize), nodata_value (asch.nodata_value) {}

ASCHeader::ASCHeader (ASCHeader& asch, Coord crop_llc, Coord crop_hrc)
{
    width = crop_hrc.x - crop_llc.x;
    height = crop_hrc.y - crop_llc.y;
    Point llcorner = img2asc (Point (crop_llc.x, crop_llc.y));
    xllcorner = llcorner.x;
    yllcorner = llcorner.y;
    cellsize = asch.cellsize;
    nodata_value = asch.nodata_value;
}

ASCHeader::ASCHeader (char* name)
{
    FILE* fp;
    fp = fopen (name, "r");
    if (fp == NULL)
	eprintx (2, "Could not open file `%s'. %s\n", name, strerror (errno));
    
    fscanf (fp, "ncols %d\n", &width);
    fscanf (fp, "nrows %d\n", &height);
    fscanf (fp, "xllcorner %lf\n", &xllcorner);
    fscanf (fp, "yllcorner %lf\n", &yllcorner);
    fscanf (fp, "cellsize %lf\n", &cellsize);
    fscanf (fp, "NODATA_value %lf\n", &nodata_value);

    fclose (fp);
}

ASCHeader::ASCHeader (FILE* fp)
{
    fscanf (fp, "ncols %d\n", &width);
    fscanf (fp, "nrows %d\n", &height);
    fscanf (fp, "xllcorner %lf\n", &xllcorner);
    fscanf (fp, "yllcorner %lf\n", &yllcorner);
    fscanf (fp, "cellsize %lf\n", &cellsize);
    fscanf (fp, "NODATA_value %lf\n", &nodata_value);
}

void ASCHeader::asc2img (Point a, Point* i)
{
    i->x = (a.x - xllcorner) / cellsize;
    i->y = (a.y - yllcorner) / cellsize;
}

void ASCHeader::img2asc (Point i, Point* a)
{
    a->x = (i.x * cellsize) + xllcorner;
    a->y = (i.y * cellsize) + yllcorner;
}

Point ASCHeader::asc2img (Point a)
{
    Point i;
    asc2img (a, &i);
    return i;
}

Point ASCHeader::img2asc (Point i)
{
    Point a;
    img2asc (i, &a);
    return a;
}
