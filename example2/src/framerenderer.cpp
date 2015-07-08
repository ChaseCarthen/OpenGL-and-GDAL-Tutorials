#include <framerenderer.h>

framerenderer::framerenderer()
{

}
// A nice generic interface function for rendereing things
void framerenderer::render(glm::mat4& view, glm::mat4& projection)
{
	Renderer.useProgram();
	Buffer.bindBuffer();
	Renderer.enableVertexAttribPointer("Position");
	Renderer.setGLVertexAttribPointer("Position", 2, GL_FLOAT, GL_FALSE, sizeof(float), 0);
	// enable three textures
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
	Renderer.addShader(GL_VERTEX_SHADER, "../shader/frame.vert");
	Renderer.addShader(GL_FRAGMENT_SHADER, "../shader/frame.frag");
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
