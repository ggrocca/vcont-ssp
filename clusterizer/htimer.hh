#ifndef _GETTIME_H
#define _GETTIME_H

//#include <sys/types.h>
#include <sys/time.h>
#include <stdint.h>

#define __NSEC_PER_SEC	1000000000L
#define __NSEC_PER_USEC	1000L

typedef uint64_t htimer;

static inline htimer __timeval_to_ns(const struct timeval *tv)
{
    return ((htimer) tv->tv_sec * __NSEC_PER_SEC) +
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

static inline void htimer_start (htimer *t)
{
    __TIC (*t);
}

static inline htimer htimer_get (htimer *t)
{
    return __TOC (*t);
}


#define _HTIMER_LEN 128

static inline char* htimer2string (htimer elapsed, char format, int precision)
{
    double _d = 0.0;
    char _f[_HTIMER_LEN];
    static char _r[_HTIMER_LEN];

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
    snprintf (_f, _HTIMER_LEN, "%%.%dLf", precision);
    snprintf (_r, _HTIMER_LEN, _f, (long double) elapsed / _d);
    return _r;
}

static inline char* htimer2string (htimer elapsed, char format)
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
    return htimer2string (elapsed, format, _p);
}

static inline char* htimer2string (htimer elapsed)
{
    return htimer2string (elapsed, 'm');
}

#endif
