#include "headerLibrary.h"

#ifdef __APPLE__
#include <GLUT/glew.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>


#include "vbo.hpp"

vbo::vbo()
{
	bDataUploaded = false;
}

void vbo::createVBO(int a_iSize)
{
	glGenBuffers(1, &uiBuffer);
	data.reserve(a_iSize);
	iSize = a_iSize;
}

void vbo::releaseVBO()
{
	glDeleteBuffers(1, &uiBuffer);
	bDataUploaded = false;
	data.clear();
}

void* vbo::mapBufferToMemory(int iUsageHint)
{
	if(!bDataUploaded)return NULL;
	void* ptrRes = glMapBuffer(iBufferType, iUsageHint);
	return ptrRes;
}

void* vbo::mapSubBufferToMemory(int iUsageHint, unsigned int uiOffset, unsigned int uiLength)
{
	if(!bDataUploaded)return NULL;
	void* ptrRes = glMapBufferRange(iBufferType, uiOffset, uiLength, iUsageHint);
	return ptrRes;
}

void vbo::unmapBuffer()
{
	glUnmapBuffer(iBufferType);
}

void vbo::bindVBO(int a_iBufferType)
{
	iBufferType = a_iBufferType;
	glBindBuffer(iBufferType, uiBuffer);
}

void vbo::uploadDataToGPU(int iDrawingHint)
{
	glBufferData(iBufferType, data.size(), &data[0], iDrawingHint);
	bDataUploaded = true;
	data.clear();
}

void vbo::addData(void* ptrData, unsigned int uiDataSize)
{
	data.insert(data.end(), (float*)ptrData, (float*)ptrData+uiDataSize);
}

float vbo::getDataPointer()
{
	if(bDataUploaded)return NULL;
	return data[0];
}

unsigned int vbo::getBuffer()
{
	return uiBuffer;
}
