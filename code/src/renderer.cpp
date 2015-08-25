#include <renderer.h>

renderer::renderer()
{
	 shaders = std::vector<shader> ();
}

void renderer::init()
{
	program = glCreateProgram();
}
bool renderer::addShader(GLenum t,string fname)
{
	bool status = false;
    GLuint Shader = glCreateShader(t);
	shader s(Shader);
	status = s.open(fname);
	cout << "SHADER VATE: "<< Shader << endl;
	if(status)
	{
		shaders.push_back(s);
	}
	return status;
}

bool renderer::link()
{
	bool status = false;
	for(int i=0; i < shaders.size();  i++)
	{

        cout << "ATTACHING: "<< shaders[i].getSource() << endl;
		status = shaders[i].link(program);
	}
    std::cout << "LINKING" << std::endl;
    glLinkProgram(program);
    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if(!link_status)
    {
        char logs[1000];
        int length=1000;
        glGetProgramInfoLog(program, 1000, &length, &logs[0]);
        std::cerr << logs << std::endl;
       return false;
    }
	return status;
}

bool renderer::compile()
{
	bool status = false;
	for(int i=0; i < shaders.size();  i++)
	{
		cout << "COMPILING: "<< shaders[i].getSource() << endl;
		status = shaders[i].compile();
		if(!status)
		{
			break;
		}
	}
	return status;
}

void renderer::cleanup()
{
	for(auto i : shaders)
	{
		i.cleanup();
	}
	glDeleteProgram(program);
	shaders.clear();
}

bool renderer::getAttribute(string str, GLint& val)
{
	val = glGetAttribLocation(program,str.c_str());
	return val != -1;
}

bool renderer::getUniform(string str, GLint& val)
{
	val = glGetUniformLocation(program,str.c_str());
	return val != -1;
}

void renderer::useProgram()
{
	glUseProgram(program);
}

void renderer::render(int count)
{
 glDrawElements(
     GL_TRIANGLES,      // mode
     count,    // count
     GL_UNSIGNED_INT,   // type
     (void*)0           // element array buffer offset
 );
} 

void renderer::render2(GLenum geom, int count)
{
    glDrawElements(
        geom,      // mode
        count,    // count
        GL_UNSIGNED_INT,   // type
        (void*)0           // element array buffer offset
    );
}

void renderer::renderRaw(GLenum geom,int count)
{
    glDrawArrays(geom,0,count);
}


void renderer::setTexture(string name,unsigned int texTarget,GLuint number,GLuint& texture)
{
    GLint v;
    glActiveTexture(number - GL_TEXTURE0+GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D,texture);
    getUniform(name,v);

    glUniform1i(v, number - GL_TEXTURE0);


};
void renderer::setUniformFloatArray2(string name, int count, float* floats)
{
    GLint v;
    getUniform(name,v);
    glUniform2fv(v,count,floats);
}

void renderer::setUniformFloatArray3(string name, int count, float* floats)
{
    GLint v;
    getUniform(name,v);
    glUniform3fv(v,count,floats);
}