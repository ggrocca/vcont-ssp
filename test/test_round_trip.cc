#include <stdio.h>
#include <stdlib.h>

#include "../common/grid.hh"
// void _pixel_round_trip_6 (int x, int y, int* px, int *py)
// {
//     bool do_redo;
//     static bool ic = true;
//     static int pi = -1;
//     static int pj = -1;
//     static int pc = 0;
//     static int ps = 1;

//  redo:
    
//     do_redo = false;

//     *px = x+pi;
//     *py = y+pj;

//     if (pi == -pj)
// 	do_redo = true;

//     pc++;
	
//     pi = ic? pi : pi + ps;
//     pj = ic? pj + ps : pj;
    
//     if (! ((pc) % 2))
// 	ic = !ic;
    
//     if (pc != 0 && ! ((pc) % 4))
// 	ps = -ps;

//     if (do_redo)
// 	goto redo;
// }


int main (void)
{

    Coord c (0,0);
    Coord p;

    printf ("round trip 6\n");
    for (int i = 0; i < 6; i++)
    {
	c.round_trip_6 (&p);
	printf ("%d --> [% d][% d]\n", i, p.x, p.y);
    }

    printf ("==========\nround trip 8\n");
    for (int i = 0; i < 8; i++)
    {
	c.round_trip_8 (&p);
	printf ("%d --> [% d][% d]\n", i, p.x, p.y);
    }
}
