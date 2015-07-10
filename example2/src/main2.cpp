#include <SDL2/SDL.h>
//#include <SDL2/sdl_opengl.h>
#include <OpenGL/GLU.h>
#include <OpenGL/gl3.h>
#define GL3_PROTOTYPES 1
#include "renderer.h"
#include <iostream>
#include <buffer.h>
#include <triangle.h>
#include <gishandler.h>
#include <vector>
#include <camera.h>
#include <chrono>
#include <terrain.h>
#include <gbuffer.h>
#include <framerenderer.h>

using namespace std;
using namespace chrono;
//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
string fname = "../data/output_srtm.tif"; //1m_DTM.tif";
terrain Terrain(fname);
float Vertices[9] = {0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0};

bool QUIT = false;

//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes matrices and clear color
bool initGL();

//Input handler
void handleKeys( unsigned char key, int x, int y )
{};

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

high_resolution_clock::time_point current;

buffer buf;
buffer elements;
buffer terrainpoints;

framerenderer fr;

camera Camera;

bool init()
{

	//Initialization flag
	bool success = true;

	//Initialize SDL
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Use OpenGL 2.1
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
		if ( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create context
			gContext = SDL_GL_CreateContext( gWindow );
			if ( gContext == NULL )
			{
				printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Use Vsync
				if ( SDL_GL_SetSwapInterval( 1 ) < 0 )
				{
					printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
				}

				//Initialize OpenGL
				if ( !initGL() )
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

	cout << glGetString(GL_VERSION) << endl;
	return success;
}

bool initGL()
{
	GLenum error = GL_NO_ERROR;
	bool success = true;
	//Initialize clear color
	glClearColor( 0.f, 0.f, 1.f, 1.f );

	//Check for error
	error = glGetError();
	if ( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
		success = false;
	}
	cout << "GUFFER SUCCESS: " << GBuffer::Init(SCREEN_WIDTH,SCREEN_HEIGHT) << endl;
	fr.setup();
	fr.setScreenDims(SCREEN_WIDTH,SCREEN_HEIGHT);
	return success;



	//Initialize Projection Matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	//Check for error
	error = glGetError();
	if ( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
		success = false;
	}

	//Initialize Modelview Matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	//Check for error
	error = glGetError();
	if ( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
		success = false;
	}

	//Initialize clear color
	glClearColor( 0.f, 0.f, 1.f, 1.f );

	//Check for error
	error = glGetError();
	if ( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
		success = false;
	}

	return success;
}

void update()
{
	//No per frame update needed
	cout << "HELLO" << endl;
}

void render()
{
	fr.SetCameraPos(Camera.getPos());
	glm::mat4 view = Camera.getView();
	glm::mat4 projection = Camera.getProjection();
	//GBuffer::BindForReading();
	//Clear color buffer
	//glClearColor( 1.f, 0.f, 0.f, 1.f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor( 0.f, 0.f, 0.5f, 0.f );
	

	/*GLsizei HalfWidth = (GLsizei)(SCREEN_WIDTH / 2.0f);
	GLsizei HalfHeight = (GLsizei)(SCREEN_HEIGHT / 2.0f);

	GBuffer::SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
	glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
	                  0, 0, HalfWidth, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	GBuffer::SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
	glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
	                  0, HalfHeight, HalfWidth, SCREEN_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	GBuffer::SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
	glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
	                  HalfWidth, HalfHeight, SCREEN_WIDTH, SCREEN_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	GBuffer::SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_TEXCOORD);
	glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
	                  HalfWidth, 0, SCREEN_WIDTH, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);*/
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor( 0.f, 0.f, 0.5f, 0.f );
	fr.render(view, projection);
	GBuffer::BindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glClearColor( 0.f, 0.f, 0.5f, 0.f );

	Terrain.render(view, projection);
	GBuffer::DefaultBuffer();
	return;

	//
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

	current = high_resolution_clock::now();
	high_resolution_clock::time_point past = high_resolution_clock::now();



	// Time to do a test open of the file yay!


	//
	//Start up SDL and create window
	if ( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		Terrain.setup();
		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

		//Enable text input
		SDL_StartTextInput();

		//While application is running
		while ( !quit )
		{
			current = high_resolution_clock::now();
			duration<double> time_span = duration_cast<duration<double>>(current - past);
			/*if(time_span.count() < 1/60.0f)
			{
				continue;
			}*/

			//Handle events on queue
			while ( SDL_PollEvent( &e ) != 0 )
			{
				//User requests quit
				if ( e.type == SDL_QUIT )
				{
					quit = true;
				}
				//Handle keypress with current mouse position
				//else if( e.type == SDL_TEXTINPUT )
				//{
				//	int x = 0, y = 0;
				//	SDL_GetMouseState( &x, &y );
				//	handleKeys( e.text.text[ 0 ], x, y );
				//}
				else if (e.type == SDL_KEYDOWN)
				{
					// handle key down events here
					if (e.key.keysym.sym == SDLK_ESCAPE)
					{
						quit = true;
					}

					// rotate camera left
					if (e.key.keysym.sym == SDLK_q)
					{
						Camera.rotateX(1 * time_span.count());
					}

					// rotate camera right
					if (e.key.keysym.sym == SDLK_e)
					{
						Camera.rotateX(-1 * time_span.count());
					}

					//Camera.applyRotation();

					// Move left
					if (e.key.keysym.sym == SDLK_a)
					{
						Camera.strafe(10 * time_span.count());
					}
					// move back
					if (e.key.keysym.sym == SDLK_s)
					{
						Camera.translate(-10 * time_span.count());
					}

					// move right
					if (e.key.keysym.sym == SDLK_d)
					{
						Camera.strafe(-10 * time_span.count());
					}

					// move forward
					if (e.key.keysym.sym == SDLK_w)
					{
						Camera.translate(10 * time_span.count());
					}

					if (e.key.keysym.sym == SDLK_y)
					{
						glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
					}
					if (e.key.keysym.sym == SDLK_u)
					{
						glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
					}
					if (e.key.keysym.sym == SDLK_z)
					{
						Camera.rotateY(-1 * time_span.count());
					}
					if (e.key.keysym.sym == SDLK_x)
					{
						Camera.rotateY(1 * time_span.count());
					}
					if (e.key.keysym.sym == SDLK_r)
					{
						Camera.flight(1 * time_span.count());
					}
					if (e.key.keysym.sym == SDLK_f)
					{
						Camera.flight(-1 * time_span.count());
					}
				}
				else if (e.type == SDL_KEYUP)
				{
					if (e.key.keysym.sym == SDLK_w)
					{
						cout << "W RELEASED" << endl;
						Camera.resetVerticalSpeed();
					}
					else if (e.key.keysym.sym == SDLK_s)
					{
						cout << "S RELEASED" << endl;
						Camera.resetVerticalSpeed();
					}
					if (e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_d)
					{
						Camera.resetHorizontalSpeed();
					}
					if (e.key.keysym.sym == SDLK_q || e.key.keysym.sym == SDLK_e)
					{
						// Reset Horizontal rotation
						Camera.resetHorizontalRotation();
					}
					if (e.key.keysym.sym == SDLK_z || e.key.keysym.sym == SDLK_x)
					{
						Camera.resetVerticalRotation();
					}
					if (e.key.keysym.sym == SDLK_r || e.key.keysym.sym == SDLK_f)
					{
						Camera.resetFlightSpeed();
					}
				}
			}
			Camera.update();
			quit = quit | QUIT;

			//Render quad
			render();
			//cout << "ISSUES" << endl;

			//cout << indicies.size() << endl;
			//glDrawArrays(GL_POINTS, 0, 1);

			//cout << sizeof(float) << endl;
			//cout << sizeof(Vertices) << endl;

			auto error = glGetError();
			if ( error != GL_NO_ERROR )
			{
				printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );

			}

			//Update screen
			SDL_GL_SwapWindow( gWindow );
			past = current;
		}

		//Disable text input
		SDL_StopTextInput();
	}

	Renderer.cleanup();

	//Free resources and close SDL
	close();

	return 0;
}
