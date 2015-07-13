CODE := ../../code
CODE_SRC := $(CODE)/src
SOURCE := ../src/main.cpp $(CODE_SRC)/gishandler.cpp $(CODE_SRC)/buffer.cpp $(CODE_SRC)/renderer.cpp $(CODE_SRC)/shader.cpp \
$(CODE_SRC)/AssetManager.cpp $(CODE_SRC)/camera.cpp $(CODE_SRC)/gbuffer.cpp $(CODE_SRC)/terrain.cpp \
$(CODE_SRC)/framerenderer.cpp 
INCLUDE := -I/usr/local/include -I$(CODE)/include
LIBS := -L/usr/local/lib -lgdal -framework OpenGL -framework Cocoa -lSDL2
FLAGS := -std=c++11 

all: $(SOURCE)
	g++ -o ../bin/out $(SOURCE) $(LIBS) $(INCLUDE) $(FLAGS)
