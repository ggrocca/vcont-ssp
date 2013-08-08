#ifndef SCALESPACE_H
#define SCALESPACE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <vector>

#include "dem.hh"
#include "scaletypes.hh"

class ScaleSpaceOpts {

public:

    static const unsigned int ILINES    = 1;
    static const unsigned int PRESMOOTH = 2;
    static const unsigned int PERTURB   = 4;
    static const unsigned int JUMP      = 8;
    static const unsigned int CROP      = 16;
    static const unsigned int CLIP      = 32;
    static const unsigned int CONTROL   = 64;

    double perturb_amp;
    int perturb_seed;
    int jump_num;
    double clip_value;
    Coord crop_a;
    Coord crop_b;
    int filter_algo;

    ScaleSpaceOpts () : opcodes (0) {}
    ScaleSpaceOpts (unsigned int opcodes) : opcodes (opcodes) {}

    void set (unsigned int opcode) { opcodes |= opcode; }
    bool check (unsigned int opcode) { return (bool)(opcodes & opcode); }

private:

    unsigned int opcodes;
};

///// DEM OPERATIONS //////

// Dem ();                                            // empty dem, no allocation
// Dem (int w, int h);                                // empty dem, allocate w * h
// Dem (DEMReader* dr);                               // create dem from demreader
// Dem (Dem& dem);                                    // copy dem
// Dem (Dem& dem, Coord a, Coord b);                  // crop & copy dem
// Dem (Dem& dem, double amp, unsigned int seed);     // copy & perturb dem
// Dem (Dem& dem, TGaussianBlur<double>& BlurFilter); // copy dem

// double& operator() (Coord c, AccessType a = ABYSS);      // read & write with coord
// double& operator() (int x, int y, AccessType a = ABYSS); // read & write with x,y

// void clip_background (double v);           // values < v are clipped to -DBL_MAX
// void clip_background (Dem& dem, double v); // clip values that are < v in dem

class ScaleSpace {

public:

    int levels;

    ScaleSpace (DEMReader* base, int levels, ScaleSpaceOpts opts);
    ScaleSpace (char* filename, ScaleSpaceOpts opts);
    ~ScaleSpace ();

    double operator() (int level, Coord c);
    double operator() (int level, int x, int y);

    static double lerp (double t, double a, double b);
    void point_print_interpolated (int level, double t, Coord c);
    void point_print_interpolated (int scope, int level, double t, Coord c);
    void write_critical (char* filename);
    void write_scalespace (char* filename);

    std::vector<Dem*> dem;
    std::vector< std::vector<CriticalPoint> > critical;
    std::vector< Grid<char> > ilines;

    

    // helper functions for levels, time and scale

    // L: 0 1 2   3 4  5  6  7   8   9   10   11
    // S: 1 2 4   8 16 32 64 128 256 512 1024 2048
    // W: 9 9 13 17 25 33 49 67  97  135 193  271
    // S = pow (2, L)
    // W = odd (floor (sqrt (S) * 6))
    static inline int level2window (int level)
    {
	int w = floor (6.0 * sqrt (pow (2.0, (double)level)));
	return w % 2 == 0? w + 1 : w;
    }

    // T = log2 (S)
    static inline double scale2time (double scale)
    {
	return log2 (scale);
    }

    // S = pow (2, T)
    static inline double time2scale (double time)
    {
	return pow (2.0, time);
    }
};



#endif
