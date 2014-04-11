#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// helper functions for levels, time and scale

// L: 0 1 2  3  4  5  6  7   8   9   10   11
// S: 1 2 4  8  16 32 64 128 256 512 1024 2048
// W: 9 9 13 17 25 33 49 67  97  135 193  271
// S = pow (2, L)
// W = odd (floor (sqrt (S) * 6))
static inline int level2window (int level)
{
    int w = floor (6.0 * sqrt (pow (2.0, (double)level)));
    return w % 2 == 0? w + 1 : w;
}

static inline double window2scale (int window)
{
    if (window < 7)
	return 1.0;
    
    return pow (((double) window) / 6.0, 2.0);
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

int main (int argc, char *argv[])
{
    for (int level = 0; level < (argc < 2? 10 : atoi (argv[1])); level++)
    {
	int window = level2window (level);
	double scale = time2scale (level);
	double time_from_scale = scale2time (scale);
	double scale_from_window = window2scale (window);
	double time_from_scale_from_window = scale2time (scale_from_window);

	printf ("l: %d,  w: %d,  s: %lf,  t: %lf,  sw: %lf,  tsw: %lf\n",
		level, window, scale, time_from_scale, scale_from_window, time_from_scale_from_window);
    }

    for (int kernel = 3; kernel < (argc < 2? 136 : atoi (argv[2])); kernel += 2)
    {
	double scale_from_window = window2scale (kernel);
	double time_from_scale_from_window = scale2time (scale_from_window);

	printf ("w: %d,  sw: %lf, tsw: %lf\n", kernel, scale_from_window, time_from_scale_from_window);
    }
}
