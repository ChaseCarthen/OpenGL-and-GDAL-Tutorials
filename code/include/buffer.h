#if defined(__APPLE__) || defined(MACOSX)
#include <OpenGL/gl3.h>
#else //linux as default
#include <GL/glew.h>

#endif


#ifndef _BUFFER_H_
#define _BUFFER_H_
class buffer
{

public:
buffer();
void allocateBufferData(unsigned int size, void* data, GLenum usage);
void generateBuffer(GLenum bufferType);
void bindBuffer();
void setVertexAttribPointer(unsigned int index, int size, GLenum type, bool normalized, int stride, void* pointer);
void cleanup();

private:

GLuint Buffer;

// buffer type
GLenum BufferType;
};
#endif