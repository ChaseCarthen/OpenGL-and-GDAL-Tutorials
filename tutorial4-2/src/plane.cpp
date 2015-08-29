#include <plane.h>

plane::plane()
{

}

void plane::buildPlane(int x, int y, int width, int height)
{

	for(int i = 0; i < width-1; i += 1)
	{
		for(int j =0; j < height-1; j += 1)
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

			inds.push_back(verts.size()/3-4);
			inds.push_back(verts.size()/3-1);
			inds.push_back(verts.size()/3-2);

			inds.push_back(verts.size()/3-4);
			inds.push_back(verts.size()/3-3);
			inds.push_back(verts.size()/3-1);
		}
	}
	Buffer.generateBuffer(GL_ARRAY_BUFFER);
	Buffer.bindBuffer();
	Buffer.allocateBufferData(sizeof(float)*verts.size(),&verts[0],GL_STATIC_DRAW);

	Buffer2.generateBuffer(GL_ELEMENT_ARRAY_BUFFER);
	Buffer2.bindBuffer();
	Buffer2.allocateBufferData(sizeof(int)*inds.size(),&inds[0],GL_STATIC_DRAW);
	setup();
}

void plane::setup()
{
	Renderer.init();
	Renderer.addShader(GL_VERTEX_SHADER, AssetManager::GetAppPath()  + "../../code/shader/primative.vert");
	Renderer.addShader(GL_FRAGMENT_SHADER, AssetManager::GetAppPath() + "../../code/shader/primative.frag");
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
	glm::mat4 mvp = projection * view;
	Renderer.setUniformMatrix4x4("mvp", mvp);
	Renderer.enableVertexAttribPointer("poses");
	Renderer.setGLVertexAttribPointer("poses", 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);

	Renderer.render(inds.size());

	Renderer.disableVertexAttribPointer("poses");
}