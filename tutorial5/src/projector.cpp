#include <projector.h>

projector::projector()
{
	Texgen = glm::mat4(1.0 / 2.0, 0, 0, 1.0 / 2.0,
	                   0, 1.0 / 2.0, 0, 1.0 / 2.0,
	                   0, 0, 1.0 / 2.0, 1.0 / 2.0,
	                   0, 0, 0, 1.0);

	projection = glm::ortho<float>(-1000, 1000, 1000, -1000, 0.9f, 10000.0f);


	up = glm::vec3(0, 0, -1);
	direction = glm::vec3(0, -1, 0);

	position = glm::vec3(110, 0, 110);
	view = glm::lookAt( position, //Eye Position
	                    position + direction, //Focus point
	                    up); //Positive Y is up
	alpha = .5;

}


void projector::setup()
{
	Renderer.init();
	Renderer.addShader(GL_VERTEX_SHADER, AssetManager::GetAppPath()  + "../../code/shader/projector.vert");
	if (projtype == IMAGE)
	{
		Renderer.addShader(GL_FRAGMENT_SHADER, AssetManager::GetAppPath() + "../../code/shader/projectorv.frag");
	}
	else
	{
		Renderer.addShader(GL_FRAGMENT_SHADER, AssetManager::GetAppPath() + "../../code/shader/projectorv2.frag");
	}

	cout << Renderer.compile() << endl;
	cout << Renderer.link() << endl;
	float verts[12] = { -1.0, -1.0,
	                    1.0, -1.0,
	                    1.0, 1.0,
	                    -1.0, -1.0,
	                    -1.0, 1.0,
	                    1.0, 1.0
	                  };
	Buffer.generateBuffer(GL_ARRAY_BUFFER);
	Buffer.bindBuffer();
	Buffer.allocateBufferData(sizeof(verts), &verts[0], GL_STATIC_DRAW);
	cout << "FILENAME: " << filename << endl;
	double x, y;
	string projection;
	if (projtype == IMAGE)
	{
		generateImageTexture(filename, tex, projection, x, y, width, height, xres, yres);
	}
	else
	{
		generateTexture(filename, tex, bandnum,projection, x, y, width, height, xres, yres);
	}

	if(maskfile != "" )
	{
		double unused1,unused2,unused5,unused6;
		int unused3,unused4;
		generateTexture(maskfile,masktex,1,projection,unused1,unused2,unused3,unused4,unused5,unused6);
	}

	origin.x = x;
	origin.y = y;

	// Lets construct the projection
	char* test = &projection[0];
	sr.importFromWkt(&test);
	SetDimensions(width * xres, height * yres);
}

void projector::update(float dt)
{

}


void projector::render(glm::mat4& view2, glm::mat4& projection2)
{
	view = glm::lookAt( position, //Eye Position
	                    position + direction, //Focus point
	                    up);

	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_2D, tex);

	if(masktex)
	{
		glActiveTexture(GL_TEXTURE0 + 6);
		glBindTexture(GL_TEXTURE_2D,masktex);
	}

	Renderer.useProgram();
	Renderer.setUniformFloat("alpha",alpha);
	Buffer.bindBuffer();
	if (projtype == IMAGE)
	{
		Renderer.enableVertexAttribPointer("Position");
		Renderer.setGLVertexAttribPointer("Position", 2, GL_FLOAT, GL_FALSE, sizeof(float), 0);

		// enable three textures
		Renderer.setUniformInteger("gPositionMap", 0);
		Renderer.setUniformInteger("gTextureMap", 1);
		Renderer.setUniformInteger("proj_tex", 5);
		Renderer.setUniformInteger("mask_tex",6);

		float SCREEN_SIZE[2] = {(float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
		Renderer.setUniformFloatArray2("gScreenSize", 1, SCREEN_SIZE);
		Renderer.setUniformMatrix4x4("projection", projection);
		Renderer.setUniformMatrix4x4("view", view);
		Renderer.setUniformMatrix4x4("tex", Texgen);

		Renderer.renderRaw(GL_TRIANGLES, 12);
		Renderer.disableVertexAttribPointer("Position");
	}
	else
	{
		Renderer.enableVertexAttribPointer("Position");
		Renderer.setGLVertexAttribPointer("Position", 2, GL_FLOAT, GL_FALSE, sizeof(float), 0);

		// enable three textures
		Renderer.setUniformInteger("gPositionMap", 0);
		Renderer.setUniformInteger("gTextureMap", 1);
		Renderer.setUniformInteger("proj_tex", 5);
		Renderer.setUniformInteger("mask_tex",6);

		float SCREEN_SIZE[2] = {(float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
		Renderer.setUniformFloatArray2("gScreenSize", 1, SCREEN_SIZE);
		Renderer.setUniformMatrix4x4("projection", projection);
		Renderer.setUniformMatrix4x4("view", view);
		Renderer.setUniformMatrix4x4("tex", Texgen);

		Renderer.renderRaw(GL_TRIANGLES, 12);
		Renderer.disableVertexAttribPointer("Position");
	}
}
