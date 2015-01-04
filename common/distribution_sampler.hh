#ifndef _DISTRIBUTION_SAMPLER_HH
#define _DISTRIBUTION_SAMPLER_HH

#include <math.h>

class DistributionSampler {
public:

    double max, min, steps, expfactor;
    double range, minpar, lastpar, expmult;

    DistributionSampler(double max = 20.0, double min = 0.0,
	    double steps = 100, double expfactor = 1.05)
	: max (max), min (min), steps (steps), expfactor (expfactor)
    {
	range = max - min;
	minpar = range / pow (expfactor, steps);
	lastpar = (minpar * pow (expfactor, steps)) - minpar;
	expmult = (range / (double)(steps)) * (steps) / lastpar;
    }

    // linear distribution.
    double lin (double i)
    {
	return ((range / (double) steps) * i) + min;
    }

    // simple exponential distribution.
    double exp (double i)
    {
	return (minpar * pow (expfactor, i)) + min;
    }

    // exponential distribution.
    //   translated to make the first value always equal to min.
    double exp_t (double i)
    {
	return ((minpar * pow (expfactor, i)) - minpar) + min;
    }

    // exponential distribution.
    //   translated to make the first value always equal to min;
    //   scaled to make the last value always equal to max.
    double exp_ts (double i)
    {
	return (((minpar * pow (expfactor, i)) - minpar) * expmult) + min;
    }
};

#endif // _DISTRIBUTION_SAMPLER_HH
