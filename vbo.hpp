#ifndef vbo_HPP
#define vbo_HPP

#include <vector>

class vbo
{
public:
	void createVBO(int a_iSize = 0);
	void releaseVBO();

	void* mapBufferToMemory(int iUsageHint);
	void* mapSubBufferToMemory(int iUsageHint, unsigned int uiOffset, unsigned int uiLength);
	void unmapBuffer();

	void bindVBO(int a_iBufferType = GL_ARRAY_BUFFER);
	void uploadDataToGPU(int iUsageHint);
	
	void addData(void* ptrData, unsigned int uiDataSize);

	float getDataPointer();
	unsigned int getBuffer();

	vbo();

private:
	unsigned int uiBuffer;
	int iSize;
	int iBufferType;
	std::vector<float> data;

	bool bDataUploaded;
};

#endif