#include <terrain.h>

terrain::terrain(string filename)
{
	fname = filename;
	model = glm::mat4(1);
}

void terrain::setup()
{
	getRawValuesFromFile(fname, vecs, min, max, xres, yres, projection);
	createMesh(vecs, xres, yres, max, indicies, vertexes);

	Renderer.init();
	Renderer.addShader(GL_VERTEX_SHADER, "../shader/terrain.vert");
	Renderer.addShader(GL_FRAGMENT_SHADER, "../shader/terrain.frag");
	cout << Renderer.compile() << endl;
	cout << Renderer.link() << endl;

	terrainpoints.generateBuffer(GL_ARRAY_BUFFER);
	terrainpoints.bindBuffer();
	// bind data buf.allocateBufferData(sizeof(Vertices),Vertices,GL_STATIC_DRAW);
	terrainpoints.allocateBufferData(sizeof(Vertex)*vertexes.size(), &vertexes[0], GL_STATIC_DRAW);

	elements.generateBuffer(GL_ELEMENT_ARRAY_BUFFER);
	elements.bindBuffer();
	elements.allocateBufferData(sizeof(int)*indicies.size(), &indicies[0], GL_STATIC_DRAW);
}

void terrain::update(float dt)
{

}

void terrain::render(glm::mat4& view, glm::mat4& projection)
{
	Renderer.useProgram();

	elements.bindBuffer();
	terrainpoints.bindBuffer();
	Renderer.enableVertexAttribPointer("poses");
	Renderer.enableVertexAttribPointer("TexCoord");
	Renderer.enableVertexAttribPointer("Normal");
	Renderer.setGLVertexAttribPointer("poses", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	Renderer.setGLVertexAttribPointer("Normal", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	Renderer.setGLVertexAttribPointer("TexCoord", 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	glm::mat4 mvp = projection * view;
	Renderer.setUniformMatrix4x4("mvp", mvp);
	Renderer.setUniformMatrix4x4("model",model);
	Renderer.setUniformFloat("Max", max);
	Renderer.setUniformFloat("Min", min);

	Renderer.render(indicies.size());
}

void terrain::cleanup()
{
	Renderer.cleanup();
}