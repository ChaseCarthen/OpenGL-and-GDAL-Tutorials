#include <framerenderer.h>

// Acquired from ogl dev
framerenderer::framerenderer()
{
	proj = 1;
}

// A nice generic interface function for rendereing things
void framerenderer::render(glm::mat4& view, glm::mat4& projection)
{
	Renderer.useProgram();
	Buffer.bindBuffer();
	Renderer.enableVertexAttribPointer("Position");
	Renderer.setGLVertexAttribPointer("Position", 2, GL_FLOAT, GL_FALSE, sizeof(float), 0);

	// enable three textures
	Renderer.setUniformInteger("gPositionMap",0);
	Renderer.setUniformInteger("gColorMap",1);
	Renderer.setUniformInteger("gNormalMap",2);
	Renderer.setUniformInteger("gProjectorMap",4);
	float SCREEN_SIZE[2]={(float)SCREEN_WIDTH,(float)SCREEN_HEIGHT};
	Renderer.setUniformFloatArray2("gScreenSize",1,SCREEN_SIZE);
	Renderer.setUniformFloatArray3("EyePos", 1, &cameraPos.x);
	Renderer.setUniformFloat("ambient", .5);
	Renderer.setUniformFloat("diffuse", .5);
	Renderer.setUniformFloat("specular", 1.0);
	Renderer.setUniformFloat("specularPower", 2.0);
	glm::vec3 color(1,1,1);
	glm::vec3 direction(0,-1,0);
	Renderer.setUniformFloatArray3("color", 1, &color.x);
	Renderer.setUniformFloatArray3("dirlight",1,&direction.x);
	Renderer.setUniformInteger("hasproj",proj);
	Renderer.renderRaw(GL_TRIANGLES,12);
}

// Need to include the mvp matrix here as well -- we need to update the world somehow
void framerenderer::update(float dt)
{

}

// A function that muse be called after opengl has been initialized!!!!!
void framerenderer::setup()
{
	Renderer.init();
	Renderer.addShader(GL_VERTEX_SHADER, AssetManager::GetAppPath()  + "../../code/shader/frame.vert");
	Renderer.addShader(GL_FRAGMENT_SHADER, AssetManager::GetAppPath() + "../../code/shader/frame.frag");
	cout << Renderer.compile() << endl;
	cout << Renderer.link() << endl;
	float verts[12] = {-1.0, -1.0,
	                   1.0, -1.0,
	                   1.0, 1.0,
	                   -1.0, -1.0,
	                   -1.0, 1.0,
	                   1.0, 1.0
	                  };
	Buffer.generateBuffer(GL_ARRAY_BUFFER);
	Buffer.bindBuffer();
	Buffer.allocateBufferData(sizeof(verts),&verts[0],GL_STATIC_DRAW);
}
