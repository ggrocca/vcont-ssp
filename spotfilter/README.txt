== Command line interfaces

* SCALETRACKER

scaletracker gets a DEM as input, performs the scalespace analysis and
algorithms and generates several files containing the results:

./spotfilter.app/Contents/MacOS/scaletracker -n 14 -i hawaii1km-gdr.asc -o hawaii -s -a  -p 0.01 0 -j 1

Relevant options:

"-o hawaii" name of the output files (hawaii.ssp, hawaii.trk, etc)

"-n 14" is the number of levels in the gaussian pyramid, 14 levels in
this case (0 to 13). If a lot of points have final life value 13 (life
cannot be higher than the last level) it makes sense to make it
higher. It needs to be higher the bigger the input dataset is.

"-j 1" is optional and skips the first, most detailed level in the gaussian
pyramid. Useful if too many useless points are generated.

Leave other options as they are!

* SPOTFILTER

spotfilter visualizes the data generated by scaletracker and adds
filtering capabilities in order to choose which spotheights to keep.

./spotfilter.app/Contents/MacOS/spotfilter -t hawaii.trk -s hawaii.ssp

"-t name.trk" for trak file, "-s name.ssp" for scalespace file, both
mandatory.



== Spotfilter GUI

* GUI toolkit.

It is done with Anttweakbar: http://anttweakbar.sourceforge.net/doc/

It's a library that enables fast tweaking of parameters, as seen in
this video: http://www.youtube.com/watch?v=OEpsG2PGi-s

Values of interactive parameters are displayed with a blue font. Read
only information has a grey font. Interactive parameters can be changed by
hovering with mouse over the value and:
- clicking on the value and write;
- clicking the plus/minus buttons;
- clicking the black dot and turning the rotoslider (http://anttweakbar.sourceforge.net/doc/tools:anttweakbar:rotoslider)


* Terrain Rendering.

You can decide wheter to render the terrain and
the mapping of terrain values to black and and white points.

Move the terrain: clicking on the image and dragging the mouse around.

Zooming: keeping pressed 'a' on the keyboard and clicking the mouse it
zooms in on the location of the mouse click. Pressing 'z' it zooms out.




== Spotfilter method

* Candidate spot heights.

possible points are maxima, minima or saddles on the terrain. These
points are ordered by two importance rankings:

- the main one is how long their life is across scales. This measures
  how important a point is from a morphological analysis perspective.

- the secondary one is a differential value relative to neighbours'
  values on the surface. This measures how much the local terrain
  abruptly changes around the point.


* Spot heights rendering.

You can choose whether to render points at all (RENDER SPOTS), how
much big to render them (SPOT SIZE) and if you want to render them all
the same size or scaled by how long their life is (SCALE BY LIFE -
this usually requires a low spot size value).

You can choose to render different sets of points:

- The final selection (which is points always added plus points selected
  by density)

- Points always added (chosen by the always add slider).

- Points selected by density from the pool of candidates.

- Points present in the pool of candidates. This is useful when
  selecting the pool of candidates using the pool add slider (see
  below).



* Filtering GUI.

for each point type you can select:

- how many points to always add (ALWAYS ADD slider), ordered by life;

- how many points to add to a pool of potential candidates (POOL ADD),
  again along life ordering and starting where the "always selected"
  set ends;

- a cull value based on the differential value (DIFFERENTIAL CUT) that
  basically says "do not add to the pool points that has differential
  value less than the cull value". This is especially useful if there
  are too many points in flat areas.

Points that are in the pools of potential candidates are added to the
final selection of points using a density criteria (DENSITY VAL
slider, the last one). This is a value in pixels that adds a point
from the pools to the final selection only if the distance of the
point from every other already selected is less than
density_val. Pools are ordered by life.


* Read-only information.

The GUI tells also the following additional information:

- TOTAL. How many points of that type have been extracted.

- ALWAYS LIFE, POOL LIFE. The life value of the last point that is
  being added to the always and pool sets.

- POOL SURVIVED. How many points of the pool are present in the final
  selection after applying the density criteria.

- ALWAYS DISCARDED. How many points are outside of the always and pool
  sets.

- DIFFERENTIAL DISCARDED. How many points have been culled by the pool
  based on the differential cut.



