g++ -pipe -D_THREAD_SAFE -pthread -O2 -DNDEBUG -c pngToMesh.cc -o pngToMesh.oo
g++ -pipe -D_THREAD_SAFE -pthread -O2 -DNDEBUG pngToMesh.oo -o pngToMesh -lpng -lz -lm -lglut -lGLU -lX11 -lGL

