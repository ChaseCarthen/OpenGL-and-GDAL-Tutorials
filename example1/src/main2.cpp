#include <SDL2/SDL.h>
//#include <SDL2/sdl_opengl.h>
#include <OpenGL/GLU.h>
#include <OpenGL/gl3.h>
#define GL3_PROTOTYPES 1
#include "renderer.h"
#include <iostream>
#include <buffer.h>
#include <triangle.h>
using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
float Vertices[9] = {0.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0,0.0};

bool QUIT = false;

//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes matrices and clear color
bool initGL();

//Input handler
void handleKeys( unsigned char key, int x, int y );

//Per frame update
void update();

//Renders quad to the screen
void render();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

//Render flag
bool gRenderQuad = true;

GLuint VBO;
GLint VaoId;

buffer buf;
triangle Tri;
bool init()
{

	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Use OpenGL 2.1
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create context
			gContext = SDL_GL_CreateContext( gWindow );
			if( gContext == NULL )
			{
				printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Use Vsync
				if( SDL_GL_SetSwapInterval( 1 ) < 0 )
				{
					printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
				}

				//Initialize OpenGL
				if( !initGL() )
				{
					printf( "Unable to initialize OpenGL!\n" );
					success = false;
				}
			}
		}
	}
	//glGenVertexArrays(1, &VaoId);
	//glBindVertexArray(VaoId);
	GLuint vao;

	// This is an absolute requirement by opengl 3 standards
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

    //buf.generateBuffer(GL_ARRAY_BUFFER);
    //buf.bindBuffer();
    //buf.allocateBufferData(sizeof(Vertices),Vertices,GL_STATIC_DRAW);

	//glGenBuffers(1, &VBO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3, Vertices, GL_STATIC_DRAW);
    Tri.setup();

	return success;
}

bool initGL()
{
	GLenum error = GL_NO_ERROR;
	bool success = true;
	//Initialize clear color
	glClearColor( 1.f, 0.f, 0.f, 1.f );
	
	//Check for error
	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
		success = false;
	}
	return success;
	
	

	//Initialize Projection Matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	
	//Check for error
	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
		success = false;
	}

	//Initialize Modelview Matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	//Check for error
	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
		success = false;
	}
	
	//Initialize clear color
	glClearColor( 1.f, 0.f, 0.f, 1.f );
	
	//Check for error
	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
		success = false;
	}
	
	return success;
}

void handleKeys( unsigned char key, int x, int y )
{
	//Toggle quad
	if( key == 'q' )
	{
		gRenderQuad = !gRenderQuad;
	}
	else if( key == 'a')
	{
		QUIT = true;
	}
}

void update()
{
	//No per frame update needed
}

void render()
{


	//Clear color buffer
	//glClearColor( 1.f, 0.f, 0.f, 1.f );
	glClear( GL_COLOR_BUFFER_BIT );
	glClearColor( 1.f, 0.f, 0.f, 1.f );
	Tri.render();
	return;
	//Render quad
	if( gRenderQuad )
	{
		glBegin( GL_QUADS );
			glVertex2f( -0.5f, -0.5f );
			glVertex2f( 0.5f, -0.5f );
			glVertex2f( 0.5f, 0.5f );
			glVertex2f( -0.5f, 0.5f );
		glEnd();
	}
}

void close()
{
	//Destroy window	
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	renderer Renderer = renderer();
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Renderer.init();	
		//Renderer.addShader(GL_VERTEX_SHADER,"../shader/simple.vert");
		//Renderer.addShader(GL_FRAGMENT_SHADER,"../shader/simple.frag");
		//cout << Renderer.compile() << endl;
		//cout << Renderer.link() << endl;

		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;
		
		//Enable text input
		SDL_StartTextInput();

		//While application is running
		while( !quit )
		{
			//Handle events on queue
			while( SDL_PollEvent( &e ) != 0 )
			{
				//User requests quit
				if( e.type == SDL_QUIT )
				{
					quit = true;
				}
				//Handle keypress with current mouse position
				else if( e.type == SDL_TEXTINPUT )
				{
					int x = 0, y = 0;
					SDL_GetMouseState( &x, &y );
					handleKeys( e.text.text[ 0 ], x, y );
				}
				else if (e.type == SDL_KEYDOWN)
				{
					// handle key down events here
					if(e.key.keysym.sym == SDLK_ESCAPE)
					{
						quit = true;
					}
				}
			}
			quit = quit | QUIT;
			//Render quad
			render();
			//cout << "ISSUES" << endl;
			//Renderer.useProgram();
			//
			//glBindBuffer(GL_ARRAY_BUFFER, VBO);
			//buf.bindBuffer();
			//Renderer.enableVertexAttribPointer("poses");
			//sglEnableVertexAttribArray(Renderer.);
			
			//buf.setVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);

			//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			//Renderer.renderRaw(GL_TRIANGLES,3);
			//glDrawArrays(GL_POINTS, 0, 1);

			//cout << sizeof(float) << endl;
			//cout << sizeof(Vertices) << endl;

			auto error = glGetError();
		if( error != GL_NO_ERROR )
		{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );

		}

			//Update screen
			SDL_GL_SwapWindow( gWindow );
		}
		
		//Disable text input
		SDL_StopTextInput();
	}

 	Renderer.cleanup();
    
	//Free resources and close SDL
	close();

	return 0;
}
