
//#define GLM_FORCE_RADIANS
#include <iostream>

#include <AssetManager.h>

#include <SDL2/SDL.h>



#if defined(__APPLE__) || defined(MACOSX)
#include <OpenGL/gl3.h>
#include <OpenGL/GLU.h>
#else //linux as default
#include <GL/glew.h>
//#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glext.h>
//#define GL_GLEXT_PROTOTYPES 1

#endif

//#define GL3_PROTOTYPES 1
#include <renderer.h>
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
#include <shape.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
using namespace std;
using namespace chrono;


float Vertices[9] = {0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0};

string ErrorString(GLenum error)
{
	if (error == GL_INVALID_ENUM)
	{
		return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
	}

	else if (error == GL_INVALID_VALUE)
	{
		return "GL_INVALID_VALUE: A numeric argument is out of range.";
	}

	else if (error == GL_INVALID_OPERATION)
	{
		return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
	}

	else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
	{
		return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
	}

	else if (error == GL_OUT_OF_MEMORY)
	{
		return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
	}
	else
	{
		return "None";
	}
};

//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes matrices and clear color
bool initGL();

//Input handler
void handleKeys( unsigned char key, int x, int y )
{};

void HandleEvents(SDL_Event e, float dt = 0);

//Per frame update
void update(float dt = 0);

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

camera Camera;

framerenderer fr;

//string fname = "../data/output_srtm.tif"; //1m_DTM.tif";
terrain Terrain;

// A memento to the shapfiles..
shape shap;
shape shap2;
shape shap3;

GLint VaoId;

high_resolution_clock::time_point current;

bool quit;

int main(int argc, char** argv)
{
	renderer Renderer = renderer();

	string appPath = argv[0];
	cout << argv[0] << endl;
	appPath.erase(appPath.end() - 3, appPath.end());

	// Lets set the application path for this guy
	AssetManager::SetAppPath(appPath);
	cout << "HERE @" << endl;

	current = high_resolution_clock::now();
	high_resolution_clock::time_point past = high_resolution_clock::now();

	//Start up SDL and create window
	if ( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		cout << "INITIALIZED" << endl;
		Terrain.SetFile(AssetManager::GetAppPath() + "../../data/drycreek.tif");
		Terrain.setup();
		shap.load(AssetManager::GetAppPath() + "../../data/streamDCEW2/stream2.shp");
		shap.createMesh(Terrain.GetProjection(),Terrain.GetOrigin(),glm::vec2(1,1),Terrain);
		shap2.load(AssetManager::GetAppPath() + "../../data/boundDCEW/boundDCEW.shp");
		shap2.createMesh(Terrain.GetProjection(),Terrain.GetOrigin(),glm::vec2(1,1),Terrain);
		shap3.load(AssetManager::GetAppPath() + "../../data/sitesDCEW2012/DCEWsites2013.shp");
		shap3.createMesh(Terrain.GetProjection(),Terrain.GetOrigin(),glm::vec2(1,1),Terrain);
		//Main loop flag
		quit = false;

		//Event handler
		SDL_Event e;

		//Enable text input
		SDL_StartTextInput();

		//While application is running
		while ( !quit )
		{
			//auto t = glGetError();
			//cout << ErrorString(t) << endl;
			current = high_resolution_clock::now();
			duration<double> time_span = duration_cast<duration<double>>(current - past);
			//Handle events on queue

			if (time_span.count() <= 1.0 / 30.0)
			{
				continue;
			}
			//past = current
			//cout << time_span.count();
			while ( SDL_PollEvent( &e ) != 0 )
			{
				HandleEvents(e, time_span.count());
			}



			// Update first
			update();

			// Now render
			render();

			auto error = glGetError();
			if ( error != GL_NO_ERROR )
			{
				cout << "Error initializing OpenGL! " << gluErrorString( error )  << endl;

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
		//Use OpenGL 3.3 -- Make sure you have a graphics card that supports 3.3
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
				auto t = glGetError();
				cout << ErrorString(t) << endl;


				//Use Vsync
				if ( SDL_GL_SetSwapInterval( 1 ) < 0 )
				{
					printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
				}
				t = glGetError();
				cout << ErrorString(t) << endl;

				#if !defined(__APPLE__) && !defined(MACOSX)
				cout << glewGetString(GLEW_VERSION) << endl;
				glewExperimental = GL_TRUE;

				auto status = glewInit();
				//Check for error
				if (status != GLEW_OK)
				{
					//std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
					success = false;
				}
				#endif
				
				t = glGetError();
				cout << ErrorString(t) << endl;
				//Initialize OpenGL
				if ( !initGL() )
				{
					printf( "OUCH Unable to initialize OpenGL!\n" );
					success = false;
				}

				//cout << glGetString(GL_VERSION) << endl;
				cout << "GUFFER SUCCESS: " << GBuffer::Init(SCREEN_WIDTH, SCREEN_HEIGHT) << endl;
				fr.setup();
				fr.setScreenDims(SCREEN_WIDTH, SCREEN_HEIGHT);
			}
		}
	}

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	return success;
}

bool initGL()
{
	GLenum error = GL_NO_ERROR;
	bool success = true;
	//Initialize clear color
	glClearColor( 0.f, 0.f, 1.f, 1.f );


	error = glGetError();
	if ( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
		success = false;
	}

	return success;
}

void update(float dt)
{
	//No per frame update needed
	Camera.update();
}

void render()
{
	fr.SetCameraPos(Camera.getPos());
	glm::mat4 view = Camera.getView();
	glm::mat4 projection = Camera.getProjection();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearColor( 0.f, 0.f, 0.5f, 0.f );
	fr.render(view, projection);

	GBuffer::BindForWriting();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	glClearColor( 0.f, 0.f, 0.5f, 0.f );
	Terrain.render(view, projection);
	shap.render(view,projection);
	shap2.render(view,projection);
	shap3.render(view,projection);
	GBuffer::DefaultBuffer();
	return;
}

void close()
{
	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

void HandleEvents(SDL_Event e, float dt)
{
	//User requests quit
	if ( e.type == SDL_QUIT )
	{
		quit = true;
	}
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
			Camera.rotateX(1 * dt);
		}

		// rotate camera right
		if (e.key.keysym.sym == SDLK_e)
		{
			Camera.rotateX(-1 * dt);
		}

		//Camera.applyRotation();

		// Move left
		if (e.key.keysym.sym == SDLK_a)
		{
			Camera.strafe(10 * dt);
		}
		// move back
		if (e.key.keysym.sym == SDLK_s)
		{
			Camera.translate(-10 * dt);
		}

		// move right
		if (e.key.keysym.sym == SDLK_d)
		{
			Camera.strafe(-10 * dt);
		}

		// move forward
		if (e.key.keysym.sym == SDLK_w)
		{
			Camera.translate(10 * dt);
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
			Camera.rotateY(-1 * dt);
		}
		if (e.key.keysym.sym == SDLK_x)
		{
			Camera.rotateY(1 * dt);
		}
		if (e.key.keysym.sym == SDLK_r)
		{
			Camera.flight(1 * dt);
		}
		if (e.key.keysym.sym == SDLK_f)
		{
			Camera.flight(-1 * dt);
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
