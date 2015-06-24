#include <triangle.h>

triangle::triangle() : verticies{0.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0,0.0}
{

}

void triangle::update(float dt)
{
	// For now we don't do anything with this triangle
}

void triangle::render()
{
	// Handle shader stuffs
	Renderer.useProgram();

	Buffer.bindBuffer();
	Renderer.enableVertexAttribPointer("poses");
			
	Buffer.setVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);

	Renderer.renderRaw(GL_TRIANGLES,3);
}

void triangle::setup()
{
	// Lets use the simple shaders that I have included as part of this project for rendering..
	Renderer.init();	
	Renderer.addShader(GL_VERTEX_SHADER,"../shader/simple.vert");
	Renderer.addShader(GL_FRAGMENT_SHADER,"../shader/simple.frag");
	cout << Renderer.compile() << endl;
	cout << Renderer.link() << endl;

	// Let populate the buffer
	Buffer.generateBuffer(GL_ARRAY_BUFFER);
    Buffer.bindBuffer();
    Buffer.allocateBufferData(sizeof(verticies),verticies,GL_STATIC_DRAW);
}