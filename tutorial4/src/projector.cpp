#include <projector.h>

projector::projector()
{
	Texgen = glm::mat4(1.0/2.0,0,0,1.0/2.0,
			    0,1.0/2.0,0,1.0/2.0,
                            0,0,1.0/2.0,1.0/2.0,
                            0,0,0,1.0);
	//cout << "TEXGEN:!!!:!:!"<< Texgen[3][3] << endl; 
    projection = glm::ortho<float>(-320, 320,-50000,50000, 0.1f, 10000.0f);//glm::perspective<float>(
                 //    35,         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
                  //   4.0f / 3.0f, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
                   //  0.1f,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
                   //  100000.0f       // Far clipping plane. Keep as little as possible.
                 //);

    up = glm::vec3(0, 0, -1);
    direction = glm::vec3(0, 1, 0);
    //direction = glm::rotate(direction, 180.0f, glm::cross(direction, up));
    position = glm::vec3(5000, 1000, 5000);
	    view = glm::lookAt( position, //Eye Position
                        position + direction, //Focus point
                        up); //Positive Y is up
        
}


void projector::setup()
{
	Renderer.init();
	Renderer.addShader(GL_VERTEX_SHADER, AssetManager::GetAppPath()  + "../../code/shader/projector.vert");
	Renderer.addShader(GL_FRAGMENT_SHADER, AssetManager::GetAppPath() + "../../code/shader/projector.frag");
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

void projector::update(float dt)
{
	
}

void projector::render(glm::mat4& view2, glm::mat4& projection2)
{
	cout << "TEXGEN:!!!:!:!"<< Texgen[0][0] << endl; 
	Renderer.useProgram();
	Buffer.bindBuffer();
	Renderer.enableVertexAttribPointer("Position");
	Renderer.setGLVertexAttribPointer("Position", 2, GL_FLOAT, GL_FALSE, sizeof(float), 0);

	// enable three textures
	Renderer.setUniformInteger("gPositionMap",0);
	Renderer.setUniformInteger("gColorMap",1);
	//Renderer.setUniformInteger("gNormalMap",2);
	float SCREEN_SIZE[2]={(float)SCREEN_WIDTH,(float)SCREEN_HEIGHT};
	Renderer.setUniformFloatArray2("gScreenSize",1,SCREEN_SIZE);
	Renderer.setUniformMatrix4x4("projection", projection);
	Renderer.setUniformMatrix4x4("view", view);
	Renderer.setUniformMatrix4x4("tex", Texgen);
	Renderer.renderRaw(GL_TRIANGLES,12);
	Renderer.disableVertexAttribPointer("Position");
}