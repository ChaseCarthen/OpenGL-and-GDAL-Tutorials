CODE := ../../code
CODE_SRC := $(CODE)/src
SOURCE := ../src/main.cpp $(CODE_SRC)/gishandler.cpp $(CODE_SRC)/buffer.cpp $(CODE_SRC)/renderer.cpp $(CODE_SRC)/shader.cpp \
$(CODE_SRC)/AssetManager.cpp $(CODE_SRC)/camera.cpp $(CODE_SRC)/gbuffer.cpp $(CODE_SRC)/terrain.cpp \
$(CODE_SRC)/framerenderer.cpp 


INCLUDE := -I$(CODE)/include
LIBS := -lgdal -lSDL2
FLAGS := -g -std=c++11 

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	INCLUDE += -I/usr/include/gdal
	LIBS += -lGLEW -lGLU -lGL  
endif
ifeq ($(UNAME_S),Darwin)
	INCLUDE += -I/usr/local/include
	LIBS += -L/usr/local/lib -framework OpenGL -framework Cocoa  
endif



all: $(SOURCE)
	mkdir -p ../bin
	g++ $(FLAGS) -o ../bin/out $(SOURCE) $(LIBS) $(INCLUDE) 



