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

/*********************************************************************
 * Names of custom scopes for trace debug prints.  Values 0/1        *
 * Disable/enable specific trace prints.  Do NOT modify them here.   *
 * Use the custom_debug.h file, explained below.                     * 
 *********************************************************************/

#define SCOPE_NEVER 0
#define SCOPE_ALWAYS 1

#define SCOPE_CRITICAL 0
#define SCOPE_TRACKING 0
#define SCOPE_PLATEUS 0
#define SCOPE_POINTTYPE 0
#define SCOPE_FLIP 0
#define SCOPE_FILTER 0
#define SCOPE_BOUNDINGBOX 0
#define SCOPE_CAMERA 0
#define SCOPE_SEEK 0
#define SCOPE_IO 0
#define SCOPE_CSVREADER 0

// if 1, perform an assert when a non critical error is reached.
#define DO_ASSERT 0

// choose if traces go on stdout or stderr
#define TRACE_STREAM stdout

// choose if other prints go on stdout or stderr
#define OTHER_STREAM stdout

#ifdef DEBUGTRACE
#define _TRACE_TEST 1
#else
#define _TRACE_TEST 0
#endif

#ifdef __CUSTOM_DEBUG_EXISTS
#include "custom_debug.h"
#endif

/************************************************************************
 *                                                                      *
 *  If the file custom_debug.h is present, it will be automatically     *
 *  included by the makefile.                                           *
 *                                                                      *
 *  Do NOT check the file into the repository.                          *
 *                                                                      *
 *  Use it to modify the defines above with custom values, remembering  *
 *  to undef them prior to redefine them.                               *
 *                                                                      *
 ************************************************************************/

/* custom_debug.h example syntax

#undef SCOPE_PLATEUS
#define SCOPE_PLATEUS 1

*/



//=//////////////////////////=//
////// CUSTOM EXIT VALUES //////
//=//////////////////////////=//

// Add here custom exit failure values.
#define EXIT_GRID 3
#define EXIT_RELATION 5




//=////////////////////////////=//
////// MACRO IMPLEMENTATION //////
//=////////////////////////////=//

#include <assert.h>
#include <errno.h>

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
	fprintf(stderr, "Fatal. %s:%d: %s(): " fmt, __FILE__,		\
		__LINE__, _FUNC, __VA_ARGS__);				\
	if (DO_ASSERT)							\
	    assert (0);							\
	exit (exit_value);						\
    } while (0)

#define tprint(fmt, ...)						\
    do {								\
	if (_TRACE_TEST)						\
	    fprintf(TRACE_STREAM, "%s:%d: %s(): " fmt, __FILE__,	\
		    __LINE__, _FUNC, __VA_ARGS__);			\
    } while (0)

#define tprintp(pre, fmt, ...)						\
    do {								\
	if (_TRACE_TEST)						\
	    fprintf(TRACE_STREAM, pre " %s:%d: %s(): " fmt, __FILE__,	\
		    __LINE__, _FUNC, __VA_ARGS__);			\
    } while (0)

#define tprints(scope, fmt, ...)					\
    do {								\
	if (_TRACE_TEST && (scope))					\
	    fprintf(TRACE_STREAM, "%s:%d: %s(): " fmt, __FILE__,	\
		    __LINE__, _FUNC, __VA_ARGS__);			\
    } while (0)

#define tprintsp(scope, pre, fmt, ...)					\
    do {								\
	if (_TRACE_TEST && (scope))					\
	    fprintf(TRACE_STREAM, pre " %s:%d: %s(): " fmt, __FILE__,	\
		    __LINE__, _FUNC, __VA_ARGS__);			\
    } while (0)

#define oprints(scope, fmt, ...)					\
    do {								\
	if (_TRACE_TEST && (scope))					\
	    fprintf(TRACE_STREAM, fmt, __VA_ARGS__);			\
    } while (0)

#endif // _DEBUG_H
