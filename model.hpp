#ifndef model_HPP
#define model_HPP

#include "tiny_obj_loader.h"
//#include "tiny_obj_loader.cc"

#include "vbo.hpp"

#define VALS_PER_VERT 3
#define VALS_PER_TEX 2
#define VALS_PER_NORMAL 3
#define CUBE_NUM_TRIS 12      // number of triangles in a cube (2 per face)
#define CUBE_NUM_VERTICES 8  
#define LIGHT_SPEED 0.2f
#define TEX_SIZE        64
#define CUBE_NUM_VERTICES 8 

class model
{
public:
	int loadModel(std::string sFileName,GLuint ProgramID);
	void renderModel();
	void releaseModel();
	GLuint* initCubeMap();
	int loadMaterial(GLuint);
	int createSkyVAO(GLuint programID);
	unsigned int texGen(const char *filename);

	int createVAO(tinyobj::material_t m,tinyobj::shape_t s);
	GLuint createTextures(tinyobj::material_t m,std::string basepath);
	
	model();
private:
	bool bLoaded;

	GLuint createTextures(tinyobj::material_t m,std::string basepath, int texIdx);
	std::vector<unsigned int> objHandler;
	std::vector<GLuint> objTexture;
	std::vector<int> IndicesNumber;

	vbo vboModelData;
	unsigned int uiVAO;
	//CTexture tAmbientTexture;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	int idxCnt;

	GLuint TexID[20];
	GLuint CurrentTex ;
	GLubyte myimage[TEX_SIZE][TEX_SIZE][3];

	float minimumX,minimumY,minimumZ,maximumX,maximumY,maximumZ;
	std::string path;
	std::string basepath;

	GLuint myProgram;
};

#endif