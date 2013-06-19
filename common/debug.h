#ifndef _DEBUG_H
#define _DEBUG_H



//=//////////////////////////////////////////=//
////// MACRO SIGNATURES AND DOCUMENTATION //////
//=//////////////////////////////////////////=//


// Error prints. Write to stderr and add file,line, and function name
// info.

// exit_value: exit with specific value.

// #define eprint(fmt, ...)
// #define eprintx(exit_value, fmt, ...)


// Debug trace prints. Printed only if DEBUGTRACE is defined, either
// in-program or on the compiler command line. Always cheked by the
// compiler, which is a Good Idea(TM) that avoids stale variables. We
// leave to the optimizer their removal if debugging is turned off.

// #define DEBUGTRACE

// scope: print happens only if specific scope is set to 1
// preamble: print something before the file:line:func: section.

// #define tprint(fmt, ...)
// #define tprintp(preamble, fmt, ...)
// #define tprints(scope, fmt, ...)
// #define tprintsp(scope, preamble, fmt, ...)



//=///////////////////////////=//
////// DEBUG CONFIGURATION //////
//=///////////////////////////=//


// if 1, perform an assert when a non critical error is reached.
#define DO_ASSERT 0

// Add here custom exit failure values.
#define EXIT_GRID 3
#define EXIT_RELATION 5


// Add here custom scopes for trace debug prints.
// Set them to 0/1 to disable/enable specific trace prints.

#define SCOPE_NEVER 0
#define SCOPE_ALWAYS 1

#define SCOPE_CRITICAL 0
#define SCOPE_TRACKING 0
#define SCOPE_PLATEUS 0
#define SCOPE_POINTTYPE 0
#define SCOPE_FLIP 0


//=////////////////////////////=//
////// MACRO IMPLEMENTATION //////
//=////////////////////////////=//

#include <assert.h>

// set fancy function names with class and signature if we're using gcc
#ifdef __GNUC__
#define _FUNC __PRETTY_FUNCTION__ 
#else
#define _FUNC __func__
#endif

#define eprint(fmt, ...)						\
    do {								\
	fprintf(stderr, "Error. %s:%d: %s(): " fmt, __FILE__,		\
		__LINE__, _FUNC, __VA_ARGS__);				\
	if (DO_ASSERT)							\
	    assert (0);							\
    } while (0)

#define eprintx(exit_value, fmt, ...)					\
    do {								\
	fprintf(stderr, "Fatal. %s:%d: %s(): " fmt, __FILE__,	\
		__LINE__, _FUNC, __VA_ARGS__);				\
	assert (0);							\
	exit (exit_value);						\
    } while (0)



#ifdef DEBUGTRACE
#define _TRACE_TEST 1
#else
#define _TRACE_TEST 0
#endif


#define tprint(fmt, ...)						\
    do {								\
	if (_TRACE_TEST)						\
	    fprintf(stdout, "%s:%d: %s(): " fmt, __FILE__,		\
		    __LINE__, _FUNC, __VA_ARGS__);			\
    } while (0)

#define tprintp(preamble, fmt, ...)					\
    do {								\
	if (_TRACE_TEST)						\
	    fprintf(stdout, preamble " %s:%d: %s(): " fmt, __FILE__,	\
		    __LINE__, _FUNC, __VA_ARGS__);			\
    } while (0)

#define tprints(scope, fmt, ...)					\
    do {								\
	if (_TRACE_TEST && (scope))					\
	    fprintf(stdout, "%s:%d: %s(): " fmt, __FILE__,		\
		    __LINE__, _FUNC, __VA_ARGS__);			\
    } while (0)

#define tprintsp(scope, preamble, fmt, ...)				\
    do {								\
	if (_TRACE_TEST && (scope))					\
	    fprintf(stdout, preamble " %s:%d: %s(): " fmt, __FILE__,	\
		    __LINE__, _FUNC, __VA_ARGS__);			\
    } while (0)

#define oprints(scope, fmt, ...)					\
    do {								\
	if (_TRACE_TEST && (scope))					\
	    fprintf(stdout, fmt, __VA_ARGS__);				\
    } while (0)

#endif // _DEBUG_H
