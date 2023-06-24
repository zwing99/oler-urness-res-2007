FRAMEWORK = -framework GLUT -framework OpenGL -framework CoreServices

CC = g++

CFLAGS = -g

INCLUDE = -I. -I/usr/include/ -I/usr/include/X11/ -I/usr/local/include/OpenGL

LDLIBS = -lGLU -lGL -lX11 -lXi -lXmu -lm -lobjc -lglew

LDFLAGS = -L. -L/usr/lib -L/usr/lib/X11 -L/usr/X11R6/lib 

OBJS = testLattice.cpp
OBJS2 = testLatticeMonteCarlo.cpp
OBJS3 = testLatticeMonteCarlo2.cpp

HELPER = lattice.cpp dipole.cpp vec.cpp Point.cpp StreamLine.cpp StreamSurface.cpp testLattice.cpp
HELPEROBJS =  $(HELPER:.cpp=.o)

iteration: $(HELPEROBJS)
	$(CC) -o iteration $(HELPEROBJS) $(FRAMEWORK) $(LDFLAGS) $(LDLIBS) 

montecarlo:
	$(CC) -o monteCarlo $(HELPER) $(OBJS2) $(FRAMEWORK) $(LDFLAGS) $(LDLIBS) 

montecarlo2:
	$(CC) -o monteCarlo2 $(HELPER) $(OBJS3) $(FRAMEWORK) $(LDFLAGS) $(LDLIBS) 

.cpp.o:
	$(CC) -c $<

clean:
	rm -f *.o monteCarlo monteCarlo2 iteration testLattice

