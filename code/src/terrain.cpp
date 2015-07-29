#include <terrain.h>

terrain::terrain()
{
	model = glm::mat4(1);
}

terrain::terrain(string filename)
{
	fname = filename;
	model = glm::mat4(1);
}

void terrain::setup()
{
	double x,y;
	getRawValuesFromFile(fname, vecs, min, max, xres, yres, projection,x,y,width,height);
	origin.x = x;
	origin.y = y;
	end.x = x + xres * (width);
	end.y = y - yres * (height);
	char* test = &projection[0];
	sr.importFromWkt(&test);
	createMesh(vecs, xres, yres, max, indicies, vertexes);

	Renderer.init();
	Renderer.addShader(GL_VERTEX_SHADER, AssetManager::GetAppPath() + "../../code/shader/terrain.vert");
	Renderer.addShader(GL_FRAGMENT_SHADER, AssetManager::GetAppPath() + "../../code/shader/terrain.frag");
	cout << Renderer.compile() << endl;
	cout << Renderer.link() << endl;

	terrainpoints.generateBuffer(GL_ARRAY_BUFFER);
	terrainpoints.bindBuffer();
	// bind data buf.allocateBufferData(sizeof(Vertices),Vertices,GL_STATIC_DRAW);
	terrainpoints.allocateBufferData(sizeof(Vertex)*vertexes.size(), &vertexes[0], GL_STATIC_DRAW);

	elements.generateBuffer(GL_ELEMENT_ARRAY_BUFFER);
	elements.bindBuffer();
	elements.allocateBufferData(sizeof(int)*indicies.size(), &indicies[0], GL_STATIC_DRAW);
	worldOrigin.x = 0;
	worldOrigin.y = 0;
	worldDims.x = end.x - origin.x;
	worldDims.y = origin.y - end.y;
}

void terrain::update(float dt)
{

}

void terrain::render(glm::mat4& view, glm::mat4& projection)
{
	//model = glm::mat4(1);
	//model = glm::translate(model,glm::vec3(1000,1000,1000));
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
	Renderer.setUniformMatrix4x4("model", model);
	Renderer.setUniformFloat("Max", max);
	Renderer.setUniformFloat("Min", min);

	Renderer.render(indicies.size());
}

void terrain::cleanup()
{
	Renderer.cleanup();
}

void terrain::SetFile(string filename)
{
	fname = filename;
}

// The point must be in utm and in the same zone.
float terrain::SampleTerrain(glm::vec2 point)
{

	// Calculate the normalized points
	auto normalized = (point-origin)/(end-origin);

	if(normalized.x < 1 && normalized.x >= 0 && normalized.y < 1 && normalized.y >= 0)
	{
		int locx = (width-1) * normalized.x;
		int locy = (height-1) * normalized.y;
		//cout << locx << " " << locy << endl;
		return vecs[locx][locy];
	}
	return 0;
}

float terrain::SampleTerrain2(glm::vec2 point)
{

	// Calculate the normalized points
	auto normalized = (point)/(worldDims);

	if(normalized.x < 1 && normalized.x >= 0 && normalized.y < 1 && normalized.y >= 0)
	{
		int locx = (width-1) * normalized.x;
		int locy = (height-1) * normalized.y;

		return vecs[locx][locy];
	}
	return 0;
}