g++ -pipe -D_THREAD_SAFE -pthread -O2 -DNDEBUG -c meshToSSp.cc -o meshToSSp.oo
g++ -pipe -D_THREAD_SAFE -pthread -O2 -DNDEBUG meshToSSp.oo -o meshToSSp -lpng -lz -lm -lglut -lGLU -lX11 -lGL

