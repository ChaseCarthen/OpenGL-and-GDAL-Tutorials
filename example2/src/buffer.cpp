#include <buffer.h>

buffer::buffer()
{

}

void buffer::setVertexAttribPointer(unsigned int index, int size, GLenum type, bool normalized, int stride, void* pointer)
{
	bindBuffer();
	glVertexAttribPointer(index,size,type,normalized,stride,pointer);
}

void buffer::generateBuffer(GLenum bufferType)
{
	BufferType = bufferType;
	glGenBuffers(1,&Buffer);
}

void buffer::allocateBufferData(unsigned int size, void* data, GLenum usage)
{
	bindBuffer();
	glBufferData(BufferType,size,data,usage);
}

void buffer::bindBuffer()
{
	glBindBuffer(BufferType,Buffer);
}

void buffer::cleanup()
{
	glDeleteBuffers(1,&Buffer);
}

