#ifndef _GETTIME_H
#define _GETTIME_H

//#include <sys/types.h>
#include <sys/time.h>
#include <stdint.h>

#define __NSEC_PER_SEC	1000000000L
#define __NSEC_PER_USEC	1000L

typedef uint64_t ht;

#ifdef __cplusplus
#define H2S_fp ht2string
#define H2S_f ht2string
#define H2S ht2string
#else
#define H2S_fp ht2string_fp
#define H2S_f ht2string_f
#define H2S ht2string
#endif

#ifdef __cplusplus
#define H2B2S_f ht2bandwidth2string
#define H2B2S ht2bandwidth2string
#else
#define H2B2S_f ht2bandwidth2string_f
#define H2B2S ht2bandwidth2string
#endif

#ifdef __cplusplus
#define H2B_f ht2bandwidth
#define H2B ht2bandwidth
#else
#define H2B_f ht2bandwidth_f
#define H2B ht2bandwidth
#endif


static inline ht __timeval_to_ns(const struct timeval *tv)
{
        return ((ht) tv->tv_sec * __NSEC_PER_SEC) +
		tv->tv_usec * __NSEC_PER_USEC;
}

#define __TIC(t) ({					\
			struct timeval __tv;		\
			gettimeofday(&__tv, NULL);	\
                        (t) = __timeval_to_ns(&__tv);	\
		})

#define __TOC(t) ({						\
			struct timeval __tv;			\
			gettimeofday(&__tv, NULL);		\
                        __timeval_to_ns(&__tv) - (t);		\
		})

static inline void ht_start (ht *t)
{
  __TIC (*t);
}

static inline ht ht_get (ht *t)
{
  return __TOC (*t);
}

static inline long double ht_format (ht elapsed, char format)
{
    long double _d = 0.0;

    switch (format)
    {
    case 's':
        _d = 1000000000.0f;
        break;
    case 'm':
        _d = 1000000.0f;
        break;
    case 'u':
        _d = 1000.0f;
        break;
    case 'n':
        _d = 1.0f;
        break;
    }

    return ((long double) elapsed) / _d;
}

#define _HT_LEN 128

static inline char* H2S_fp (ht elapsed, char format, int precision)
{
    double _d = 0.0;
    char _f[_HT_LEN];
    static char _r[_HT_LEN];

    switch (format)
    {
    case 's':
        _d = 1000000000.0f;
        break;
    case 'm':
        _d = 1000000.0f;
        break;
    case 'u':
        _d = 1000.0f;
        break;
    case 'n':
        _d = 1.0f;
        break;
    }
    snprintf (_f, _HT_LEN, "%%.%dLf", precision);
    snprintf (_r, _HT_LEN, _f, (long double) elapsed / _d);
    return _r;
}

static inline char* H2S_f (ht elapsed, char format)
{
    int _p;

    switch (format)
    {
    case 's':
        _p = 6;
        break;
    case 'm':
        _p = 3;
        break;
    case 'u':
        _p = 1;
        break;
    case 'n':
        _p = 0;
        break;
    }
    return H2S_fp (elapsed, format, _p);
}

static inline char* H2S (ht elapsed)
{
    return H2S_f (elapsed, 'm');
}

static inline long double H2B_f (size_t b, ht t, char f)
{
    long double _d;

    switch (f)
    {
    case 'G':
	_d = 1024.0 * 1024.0 * 1024.0;
	break;
    case 'M':
	_d = 1024.0 * 1024.0;
	break;
    case 'K':
	_d = 1024.0;
	break;
    }

    return (((long double) b) / _d) / ((long double) ht_format (t,'s'));
}

static inline long double H2B (size_t b, ht t)
{
    return H2B_f (b, t, 'G');
}

static inline char* H2B2S_f (size_t b, ht t, char f)
{
    static char _r[_HT_LEN];
    snprintf (_r, _HT_LEN, "%.3Lf", H2B_f (b, t, f));    
    return _r;
}

static inline char* H2B2S (size_t b, ht t)
{
    return H2B2S_f (b, t, 'G');
}

#endif
