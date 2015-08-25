#include <shader.h>
#include <vector>
#include <string>
#include <iostream>
//#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#if defined(__APPLE__) || defined(MACOSX)
#include <OpenGL/gl3.h>
#else //linux as default
#include <GL/glew.h>
#endif


using namespace std;

#ifndef _RENDERER_H_
#define _RENDERER_H_
// A Renderer class that will store and hold shader programs
// It will compile and link all shader passed to it
class renderer
{
public:
    renderer();
    void init();
    bool addShader(GLenum t, string fname);
    bool link();
    bool compile();
    void cleanup();
    //Shader Access Functions go here

    // Uniforms
    bool getUniform(string str, GLint& val);
    void setUniformMatrix4x4(string name,glm::mat4& mat)
    {
        GLint v;
        getUniform(name,v);
        //std::cout << "FOUND " << v <<endl;

        glUniformMatrix4fv(v, 1, GL_FALSE, glm::value_ptr(mat));
    };
    void setUniformFloatArray2(string name, int count, float *floats);
    void setUniformFloatArray3(string name, int count, float *floats);
    void setUniformInteger(string name, int val)
    {
       GLint v;
       getUniform(name,v);
       glUniform1i(v,val);
    };
    void setUniformFloat(string name, float val)
    {
       GLint v;
       getUniform(name,v);
       glUniform1f(v,val);
    };
    // Attributes
    bool getAttribute(string str, GLint& val);
    void enableVertexAttribPointer(string name)
    {
    GLint val;
    getAttribute(name,val);
    glEnableVertexAttribArray(val);
    };
    void disableVertexAttribPointer(string name)
    {
    GLint val;
    getAttribute(name,val);
    glDisableVertexAttribArray(val);
    };
    void setGLVertexAttribPointer(string name,int elements, GLenum type, bool t,int size,void* offset)
    {
    GLint val;
    getAttribute(name,val);
    glVertexAttribPointer( val,//location of attribute
                           elements,//number of elements
                           type,//type
                           t,//normalized?
                           size,//stride
                           offset);
    };

    // Others for other versions of glsl.

    // Assuming 1D Texture...
    void setTexture(string name,unsigned int texTarget,GLuint number,GLuint& texture);

    // Set this render as the current program
    void useProgram();

    //Render Function
    void render(int count);
    void render2(GLenum geom, int count);
    void renderRaw(GLenum geom, int count);
private:
    std::vector<shader> shaders;
    GLuint program;

};


#endif
