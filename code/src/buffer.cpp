#include <buffer.h>

// a opengl buffer class
buffer::buffer()
{

}

// Set a vertex attrib pointer
void buffer::setVertexAttribPointer(unsigned int index, int size, GLenum type, bool normalized, int stride, void* pointer)
{
	bindBuffer();
	glVertexAttribPointer(index,size,type,normalized,stride,pointer);
}

// generate buffer
void buffer::generateBuffer(GLenum bufferType)
{
	BufferType = bufferType;
	glGenBuffers(1,&Buffer);
}

//allocate buffer data
void buffer::allocateBufferData(unsigned int size, void* data, GLenum usage)
{
	bindBuffer();
	glBufferData(BufferType,size,data,usage);
}

// bind buffef
void buffer::bindBuffer()
{
	glBindBuffer(BufferType,Buffer);
}

// cleanup
void buffer::cleanup()
{
	glDeleteBuffers(1,&Buffer);
}

