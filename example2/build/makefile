all: ../src/main.cpp
	 g++ ../src/gishandler.cpp ../src/main2.cpp ../src/renderer.cpp ../src/shader.cpp ../src/buffer.cpp ../src/triangle.cpp ../src/camera.cpp ../src/terrain.cpp ../src/gbuffer.cpp ../src/framerenderer.cpp -o ../bin/cube2  -L/usr/local/lib -lSDL2 -g -I /usr/local/include -I../include -framework OpenGL -framework Cocoa -framework GLUT -lgdal -std=c++11
clean:
	rm ../bin/* 
