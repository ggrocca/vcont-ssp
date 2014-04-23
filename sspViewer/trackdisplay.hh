#ifndef __TRACKDISPLAY__
#define __TRACKDISPLAY__

#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include <iostream>
#include <algorithm>
#include <vector>

#include "track.hh"
//#include "demreader.hh"
//#include "grid.hh"

#define PI 3.1415926535


class TrackSortedPoint
{
public:
    TrackSortedPoint( double lifePerc, int originalIndex );
    ~TrackSortedPoint(){};
	double life;
	int baseIndex;
	int lifeIndex;
};

struct LifeSorter {
	bool operator() (TrackSortedPoint* a, TrackSortedPoint* b) {
		return (a->life) < (b->life);
	}
};
struct BaseSorter {
	bool operator() (TrackSortedPoint* a, TrackSortedPoint* b) {
		return (a->baseIndex) < (b->baseIndex);
	}
};


class TrackDisplay
{
public:
    
    TrackDisplay();
    ~TrackDisplay(){};

	float* terrain;
	int w, h;

    bool draw_terrain;
    bool draw_criticals;
    bool draw_boundary_criticals;
    bool draw_as_spheres;

	double baseRadius;
	double weightedRadius;

	double weightThreshold;
	double weightBalance;

	double gaussWFrom;
	double gaussWTo;
	double bauerWFrom;
	double bauerWTo;

	typedef enum {R_DEFAULT, R_GAUSS, R_BAUER} RadiusPrintMode;
	RadiusPrintMode printMode;

	typedef enum {ON_NUMBER, ON_LIFE} ThresholdCutType;
	ThresholdCutType thresholdMode;

	typedef enum {W_MIN, W_MAX, W_AVG} WeightType;
	WeightType choosedWeightType;

	std::vector<TrackSortedPoint*> gaussSorted;
	std::vector<TrackSortedPoint*> bauerSorted;
	LifeSorter myLifeSorter;
	BaseSorter myBaseSorter;

    Track* gaussTrack;
	Track* bauerTrack;
    TrackOrdering* gaussTrOrder;
    TrackOrdering* bauerTrOrder;
    
	void loadTerrain( char* file);
	void loadTracks( char* gaussFile, char* bauerFile);
	void draw();
	void drawCircle(Point p, float radius, int color);
	void drawCross(Point p, float wGauss, float wBauer, int color);

	void getBounds( Point* a, Point* b);

	float getLife( int i, int tMode, Track* t, std::vector<TrackSortedPoint*> v, double min, double max);
	float getCriticalWeight( int i, int tMode, int pMode);

	bool isInWindow(int i);
	bool pointNeedToBeDrawn(Point p);

	void save( char* file, bool viewedOnly);
	void InitSortedVectorByTrack(std::vector<TrackSortedPoint*> *sortedVector, Track* track, char* filename);

};


#endif
