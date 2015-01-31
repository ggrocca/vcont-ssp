#ifndef _GENERICLIMITS_HH
#define _GENERICLIMITS_HH

#include <climits>
#include <limits>

// GG SUBSTITUTE WITH <type_traits> in c++11
template<class T>
struct GenericType {
    enum { IS_NUMERIC = 0 };
    static T lowest() { return (T()); }
};
template<> struct GenericType <char> {
    enum { IS_NUMERIC = 1 };
    static char lowest() { return (std::numeric_limits<char>::min()); }
};
template<> struct GenericType <unsigned char> {
    enum { IS_NUMERIC = 1 };
    static unsigned char lowest() { return (std::numeric_limits<unsigned char>::min());}
};
template<> struct GenericType <short> {
    enum { IS_NUMERIC = 1 };
    static short lowest() { return (std::numeric_limits<short>::min()); }
};
template<> struct GenericType <unsigned short> {
    enum { IS_NUMERIC = 1 };
    static  unsigned short lowest(){return(std::numeric_limits<unsigned short>::min());}
};
template<> struct GenericType <int> {
    enum { IS_NUMERIC = 1 };
    static int lowest() { return (std::numeric_limits<int>::min()); }
};
template<> struct GenericType <unsigned int> {
    enum { IS_NUMERIC = 1 };
    static unsigned int lowest() { return (std::numeric_limits<unsigned int>::min()); }
};
template<> struct GenericType <float> {
    enum { IS_NUMERIC = 1 };
    static float lowest() { return (-std::numeric_limits<float>::max()); }
};
template<> struct GenericType <double> {
    enum { IS_NUMERIC = 1 };
    static double lowest() { return (-std::numeric_limits<double>::max()); }
};

#endif // _GENERICLIMITS_HH
