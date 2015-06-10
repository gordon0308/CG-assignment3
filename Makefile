
PLATFORM := $(shell uname)

# If you're not using Cygwin or Linux, 
# you will need to add your own setup here
ifneq (, $(findstring CYGWIN, $(PLATFORM)))
    GL_LIBS = -lopengl32 -lglu32 -lglut32
	EXT = .exe
endif

ifneq (, $(findstring Linux, $(PLATFORM)))
    GL_LIBS = -lGL -lGLU -lglut -lGLEW
    GL_LIBS += -L/usr/lib/nvidia-304 # necessary for some NVidia drivers;
	EXT = 
endif

ifneq (, $(findstring Darwin, $(PLATFORM)))
    CPPFLAGS   = -DMacOSX -D__APPLE__ -m32 -arch x86_64    
    CPPFLAGS += -Wno-deprecated
    GL_LIBS = -framework GLUT -framework OpenGL -lGLEW
	EXT = 
endif

CC = g++
EXE = assign3
OBJS = assign3.o Shader.o Sphere.o vbo.o model.o tiny_obj_loader.o

# If you haven't seen this before, notice how the rules
# of the Makefile build .o files from c++ source, and then
# build the executable from the .o files. Files are only
# re-made if they've been modified, or something they depend
# on has been modified.

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS)

assign3.o: assign3.cpp
	$(CC) $(CPPFLAGS) -c assign3.cpp

Shader.o : Shader.cpp Shader.hpp
	$(CC) $(CPPFLAGS) -c Shader.cpp

Sphere.o : Sphere.cpp Sphere.hpp
	$(CC) $(CPPFLAGS) -c Sphere.cpp

vbo.o : vbo.cpp vbo.hpp
	$(CC) $(CPPFLAGS) -c vbo.cpp

model.o : model.cpp model.hpp
	$(CC) $(CPPFLAGS) -c model.cpp

tiny_obj_loader.o : tiny_obj_loader.cc tiny_obj_loader.h
	$(CC) $(CPPFLAGS) -c tiny_obj_loader.cc

clean:
	rm -f *.o $(EXE)$(EXT)
