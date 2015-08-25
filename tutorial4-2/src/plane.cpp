#include <plane.h>

plane::plane()
{

}

void plane::buildPlane(int x, int y, int width, int height)
{

	for(int i = 0; i < width-1; i += 2)
	{
		for(int j =0; j < height-1; j += 2)
		{
			verts.push_back(x+i);
			verts.push_back(0);
			verts.push_back(y+j);

			verts.push_back(x+i+1);
			verts.push_back(0);
			verts.push_back(y+j);

			verts.push_back(x+i);
			verts.push_back(0);
			verts.push_back(y+j+1);

			verts.push_back(x+i+1);
			verts.push_back(0);
			verts.push_back(y+j+1);

			inds.push_back(verts.size()-4);
			inds.push_back(verts.size()-2);
			inds.push_back(verts.size()-1);

			inds.push_back(verts.size()-4);
			inds.push_back(verts.size()-1);
			inds.push_back(verts.size()-3);
		}
	}
	Buffer.generateBuffer(GL_ARRAY_BUFFER);
	Buffer.bindBuffer();
	Buffer.allocateBufferData(sizeof(int)*verts.size(),&verts[0],GL_STATIC_DRAW);

	Buffer2.generateBuffer(GL_ELEMENT_ARRAY_BUFFER);
	Buffer2.bindBuffer();
	Buffer2.allocateBufferData(sizeof(int)*inds.size(),&inds[0],GL_STATIC_DRAW);
}

void plane::setup()
{
	Renderer.init();
	Renderer.addShader(GL_VERTEX_SHADER, AssetManager::GetAppPath()  + "../../code/shader/simple.vert");
	Renderer.addShader(GL_FRAGMENT_SHADER, AssetManager::GetAppPath() + "../../code/shader/simple.frag");
	cout << Renderer.compile() << endl;
	cout << Renderer.link() << endl;
}

void plane::update(float dt)
{

}

void plane::render(glm::mat4& view, glm::mat4& projection)
{
	Renderer.useProgram();
	Buffer.bindBuffer();
	Buffer2.bindBuffer();
	Renderer.enableVertexAttribPointer("poses");
	Renderer.setGLVertexAttribPointer("poses", 3, GL_FLOAT, GL_FALSE, sizeof(float), 0);

	Renderer.render(inds.size());

	Renderer.disableVertexAttribPointer("poses");
}