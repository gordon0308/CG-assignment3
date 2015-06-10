#include "headerLibrary.h"

#include "model.hpp"
#include <iostream>

#define WALL "wallTex.jpg"
#define FLOOR "floorTex.jpg"
#define UP "upTex.jpg"
#define CUBE_NUM_TRIS 12  
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

model::model()
{
	bLoaded = false;
	idxCnt=0;
	CurrentTex=0;
}

int model::loadModel(string path,GLuint ProgramID)
{
	myProgram=ProgramID;
	CurrentTex=0;
	basepath=path.substr(0,path.rfind('/')+1);
	string err=tinyobj::LoadObj(shapes,materials,path.c_str(),basepath.c_str());
	if (!err.empty()) {
		cerr << err << endl;
		exit(0);
	}

	// Create VBO

	vboModelData.createVBO();
	vboModelData.bindVBO();

	vboModelData.uploadDataToGPU(GL_STATIC_DRAW);

	// Create one VAO
	minimumX=minimumY=minimumZ=-numeric_limits<float>::max();
	maximumX=maximumY=maximumZ=numeric_limits<float>::max();

	int counter=0;
	map<vector<float> ,int> offset;
	vector<float> v,vt,vn,c,ca,cd,cs;
	vector<int> i;

	for(int idx = 0; idx < shapes.size(); idx++) {
		//objTexture.push_back(loadMaterial(materials[idx],basepath,ProgramID));
		v.insert(v.end(),shapes[idx].mesh.positions.begin(),shapes[idx].mesh.positions.end());
		if(shapes[idx].mesh.normals.size()!=0){
			vn.insert(vn.end(),shapes[idx].mesh.normals.begin(),shapes[idx].mesh.normals.end());
		}else{
			vector<float> temp(shapes[idx].mesh.positions.size(),0);
			vn.insert(vn.end(),temp.begin(),temp.end());
		}
		for (int x = 0; x < shapes[idx].mesh.texcoords.size() / 2; x++) {
			int ti=2*x;
			vt.push_back(shapes[idx].mesh.texcoords[ti]);
			vt.push_back(shapes[idx].mesh.texcoords[ti+1]);
		}
		vector<vector<float> > temps;
		for (int x = 0; x < shapes[idx].mesh.positions.size() / 3; x++) {
			int vi=3*x;
			c.push_back(materials[idx].ambient[0]);c.push_back(materials[idx].ambient[1]);c.push_back(materials[idx].ambient[2]);
			ca.push_back(materials[idx].ambient[0]);ca.push_back(materials[idx].ambient[1]);ca.push_back(materials[idx].ambient[2]);
			cd.push_back(materials[idx].diffuse[0]);cd.push_back(materials[idx].diffuse[1]);cd.push_back(materials[idx].diffuse[2]);
			cs.push_back(materials[idx].specular[0]);cs.push_back(materials[idx].specular[1]);cs.push_back(materials[idx].specular[2]);

			if(shapes[idx].mesh.positions[vi+0]>minimumX) minimumX=shapes[idx].mesh.positions[vi];
			if(shapes[idx].mesh.positions[vi+0]<maximumX) maximumX=shapes[idx].mesh.positions[vi];
			if(shapes[idx].mesh.positions[vi+1]>minimumY) minimumY=shapes[idx].mesh.positions[vi+1];
			if(shapes[idx].mesh.positions[vi+1]<maximumY) maximumY=shapes[idx].mesh.positions[vi+1];
			if(shapes[idx].mesh.positions[vi+2]>minimumZ) minimumZ=shapes[idx].mesh.positions[vi+2];
			if(shapes[idx].mesh.positions[vi+2]<maximumZ) maximumZ=shapes[idx].mesh.positions[vi+2];

			vector<float> temp;
			for(int e = vi; e < vi+3; e++){
				temp.push_back(shapes[idx].mesh.positions[e]);
			}
			temps.push_back(temp);
			if(offset.find(temp)==offset.end()){
				offset[temp]=counter;
			}
			counter++;
		}
		for (int x = 0; x < shapes[idx].mesh.indices.size() / 3; x++) {
			int vi=3*x;
			i.push_back(offset[temps[shapes[idx].mesh.indices[vi]]]);
			i.push_back(offset[temps[shapes[idx].mesh.indices[vi+1]]]);
			i.push_back(offset[temps[shapes[idx].mesh.indices[vi+2]]]);
		}

	}
	idxCnt=i.size();

	float Vertices[v.size()];
	copy(v.begin(),v.end(),Vertices);
	float Colour[c.size()];
	copy(c.begin(),c.end(),Colour);
	unsigned int indices[i.size()];
	copy(i.begin(),i.end(),indices);
	float tex_coord[ vt.size() ];
	copy(vt.begin(),vt.end(),tex_coord);
	float normals[vn.size()];
	copy(vn.begin(),vn.end(),normals);
	float ambient[ ca.size() ];
	copy(ca.begin(),ca.end(),ambient);
	float diffuse[ cd.size() ];
	copy(cd.begin(),cd.end(),diffuse);
	float specular[ cs.size() ];
	copy(cs.begin(),cs.end(),specular);

	int size=sizeof(indices)/sizeof(indices[0]);

	idxCnt/=3;

	glGenVertexArrays(1, &uiVAO);
	glBindVertexArray(uiVAO);

	int vertLoc = glGetAttribLocation(ProgramID, "a_vertex");
	int colLoc = glGetAttribLocation(ProgramID, "a_colour");
	int texLoc = glGetAttribLocation(ProgramID, "a_tex_coord");
	int normLoc = glGetAttribLocation(ProgramID, "a_normal");


	int mtlambientHandle = glGetAttribLocation(ProgramID, "mtl_ambient");
	int mtldiffuseHandle = glGetAttribLocation(ProgramID, "mtl_diffuse");
	int mtlspecularHandle = glGetAttribLocation(ProgramID, "mtl_specular");

  int lightOnHandle = glGetUniformLocation(ProgramID, "light_on");
  glUniform1i(lightOnHandle,  true); 

	// Buffer to store model
	int BufferSize=7;

	unsigned int buffer[BufferSize];
	glGenBuffers(BufferSize, buffer);


        // Set vertex position
        glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(Vertices), Vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(vertLoc);
        glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);
        
        //ambient Colour attributes
        glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(ambient), ambient, GL_STATIC_DRAW);
        glEnableVertexAttribArray(mtlambientHandle);
        glVertexAttribPointer(mtlambientHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //diffuse Colour attributes
        glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(diffuse), diffuse, GL_STATIC_DRAW);
        glEnableVertexAttribArray(mtldiffuseHandle);
        glVertexAttribPointer(mtldiffuseHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //specular Colour attributes
        glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(specular), specular, GL_STATIC_DRAW);
        glEnableVertexAttribArray(mtlspecularHandle);
        glVertexAttribPointer(mtlspecularHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);
        
    
        // Texture attributes
        glBindBuffer(GL_ARRAY_BUFFER, buffer[4]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
        glEnableVertexAttribArray(texLoc);
        glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
        
        
        // Normal attributes
        glBindBuffer(GL_ARRAY_BUFFER, buffer[5]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(normals),normals,GL_STATIC_DRAW);
        glEnableVertexAttribArray(normLoc);
        glVertexAttribPointer(normLoc, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);
        
        // Set element attributes. Notice the change to using GL_ELEMENT_ARRAY_BUFFER
        // We don't attach this to a shader label, instead it controls how rendering is performed
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[6]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                     sizeof(indices), indices, GL_STATIC_DRAW);
    
	bLoaded = true;

	loadMaterial(myProgram);

    // Un-bind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	return uiVAO;
}

void model::renderModel()
{
	if(!bLoaded)return;

	glBindVertexArray(uiVAO);

  glEnable(GL_TEXTURE_2D);

  for(int idx=0;idx<materials.size();idx++){
    int texHandle=glGetUniformLocation(myProgram,"texMap");

    if(texHandle==-1)
      exit(1);
    glActiveTexture(GL_TEXTURE0+myProgram);
    
    glGenTextures(idx+1, &TexID[idx]);

	  glBindTexture(GL_TEXTURE_2D, TexID[idx]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glUniform1i(texHandle, 0);

    glDrawElements(GL_TRIANGLES, idxCnt*3, GL_UNSIGNED_INT, 0); 
  }
	
}

int model::loadMaterial(GLuint i)
{
    // A shader program has many texture units, slots in which a texture can be bound, available to
  // it and this function defines which unit we are working with currently
  // We will only use unit 0 until later in the course. This is the default.
  glActiveTexture(GL_TEXTURE0+i);

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    
    glGenTextures( materials.size(), TexID );
    
    // create checkerboard image
    for(int i=0; i < TEX_SIZE; i++){
        for(int j=0; j < TEX_SIZE; j++)
        {
            GLubyte c;
            c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
            myimage[i][j][0]  = c;
            myimage[i][j][1]  = c;
            myimage[i][j][2]  = c;
        }                
    }

    // load checkerboard image as texture 0
    glBindTexture( GL_TEXTURE_2D, TexID[0] );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 TEX_SIZE, TEX_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, myimage);
    
    glGenerateMipmap(GL_TEXTURE_2D); // generates mipmaps for the texture attached to target of the active texture unit
    
    // load an image from file as texture 1
    int x, y, n;
    for(int idx=0;idx<materials.size();idx++){
    if(materials[idx].diffuse_texname!=""){
      std::string imgpath=basepath+materials[idx].diffuse_texname;
          int x, y, n;
          unsigned char *data = stbi_load(
                                imgpath.c_str(), /*char* filepath */
                                &x, /*The address to store the width of the image*/
                                &y, /*The address to store the height of the image*/
                                &n  /*Number of channels in the image*/,
                                0   /*Force number of channels if > 0*/
             ); 
      std::cout<<imgpath<<std::endl;
      glBindTexture( GL_TEXTURE_2D, TexID[idx] );
        if (n == 3) {
              glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
          }else {
              fprintf(stderr, "Image pixels are not RGB. You will need to change the glTexImage2D command.");
        }
          stbi_image_free(data);
          glGenerateMipmap(GL_TEXTURE_2D); 
    }else{
      glBindTexture( GL_TEXTURE_2D, TexID[idx] );
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,TEX_SIZE, TEX_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, myimage);
      glGenerateMipmap(GL_TEXTURE_2D); // generates mipmaps for the texture attached to target of the active texture unit

    }
  }


    return 0;
  }


void model::releaseModel()
{
	if(!bLoaded)return;
	glDeleteVertexArrays(1, &uiVAO);
	vboModelData.releaseVBO();
	bLoaded = false;
}

int model::createSkyVAO(GLuint programID)
{
    // Cube has 8 vertices at its corners
    float cubeVertices[ CUBE_NUM_VERTICES*VALS_PER_VERT ] = {
         -1.0f, -1.0f, 1.0f ,
          1.0f, -1.0f, 1.0f ,
          1.0f,  1.0f, 1.0f ,
         -1.0f,  1.0f, 1.0f ,
         -1.0f, -1.0f, -1.0f ,
          1.0f, -1.0f, -1.0f ,
          1.0f,  1.0f, -1.0f ,
         -1.0f,  1.0f, -1.0f  
    };

    // Colours for each vertex; red, green, blue and alpha
    float cubeNormals[ CUBE_NUM_VERTICES*VALS_PER_VERT ] = {
        -1.0f, -1.0f, 1.0f ,
         1.0f, -1.0f, 1.0f ,
         1.0f,  1.0f, 1.0f ,
        -1.0f,  1.0f, 1.0f ,
        -1.0f, -1.0f, -1.0f ,
         1.0f, -1.0f, -1.0f ,
         1.0f,  1.0f, -1.0f ,
        -1.0f,  1.0f, -1.0f
    };
    
    // Texture coords for each vertex. 2 per vertex.
    float tex_coord[CUBE_NUM_VERTICES * VALS_PER_TEX];/* = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };
*/
    // Each square face is made up of two triangles
    unsigned int indices[CUBE_NUM_TRIS * 3] = {
        0,1,2, 2,3,0,
        1,5,6, 6,2,1,
        5,4,7, 7,6,5,
        4,0,3, 3,7,4,
        3,2,6, 6,7,3,
        4,5,1, 1,0,4
    };
    

    unsigned int uiVAO;
    glGenVertexArrays(1, &uiVAO);
    glBindVertexArray(uiVAO);

    int vertLoc = glGetAttribLocation(programID, "a_vertex");
    int normLoc = glGetAttribLocation(programID, "a_normal" );
    int texLoc = glGetAttribLocation(programID, "a_tex_coord" );

    // Buffers to store position, colour and index data
    unsigned int buffer[4];
    glGenBuffers(4, buffer);

    // Set vertex position
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, 
                 sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vertLoc);
    glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

    // Normal attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, 
                 sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(normLoc);
    glVertexAttribPointer(normLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

    // Texture attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
    glEnableVertexAttribArray(texLoc);
    glVertexAttribPointer(texLoc, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);
    

    // Set element attributes. Notice the change to using GL_ELEMENT_ARRAY_BUFFER
    // We don't attach this to a shader label, instead it controls how rendering is performed
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 sizeof(indices), indices, GL_STATIC_DRAW);   
    
    // Un-bind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    return uiVAO;
}

bool load_cube_map_side (GLuint texture, GLenum side_target, const char* file_name) {
    glBindTexture (GL_TEXTURE_CUBE_MAP, texture);

    int x, y, n;
    int force_channels = 4;
    unsigned char*  image_data = stbi_load (file_name, &x, &y, &n, force_channels);
    if (!image_data) {
        fprintf (stderr, "ERROR: could not load %s\n", file_name);
        return false;
    }
    // non-power-of-2 dimensions check
    if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
        fprintf (stderr, "WARNING: image %s is not power-of-2 dimensions\n", file_name);
    } 
  
    // copy image data into 'target' side of cube map
    glTexImage2D (side_target,0,GL_RGBA,x,y,0,GL_RGBA,GL_UNSIGNED_BYTE,image_data);
    free (image_data);
    return true;
}


GLuint* model::initCubeMap(){
  GLuint *tex_cube=new GLuint[6];
  // generate a cube-map texture to hold all the sides
  glActiveTexture (GL_TEXTURE0);
  glGenTextures (1, tex_cube);
  
  // load each image and copy into a side of the cube-map texture
  assert (load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, WALL));
  assert (load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, WALL));
  assert (load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, UP));
  assert (load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, FLOOR));
  assert (load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, WALL));
  assert (load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_X, WALL));

  // format cube map texture
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  return tex_cube;  
}

unsigned int model::texGen(const char *filename) {
    
	
    // load an image from file as texture 1
    int x, y, n;
    unsigned char *data = stbi_load(
                                    filename, /*char* filepath */
                                    &x, /*The address to store the width of the image*/
                                    &y, /*The address to store the height of the image*/
                                    &n  /*Number of channels in the image*/,
                                    0   /*Force number of channels if > 0*/
                                    );
    
    
	// Texture 'names' are a handle for a single texture
	unsigned int textureHandle;
	glGenTextures(1, &textureHandle);	// You generate as many as you want to work with
    
	// Sets ogl state to work on this currently empty texture
	// AND associates active texture unit to use this texture we are building
	glBindTexture(GL_TEXTURE_2D, textureHandle);
    
    // TexImage loads the image data to the currently bound texture
	// See ogl spec. for full details of this methods parameters.
	if (n == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else {
        fprintf(stderr, "Image pixels are not RGB. You will need to change the glTexImage2D command.");
    }
    stbi_image_free(data);
    
    
	// Textures can take a number of parameters that define exactly what will return for texture
	// lookups inside the shader program, thus changing what is drawn.
	// There are heaps of different parameters for TexParameter*. See the spec.
    
	// Linear interpolation filter gives a smoothed out appearance the square pixels
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// Clamp to edge can help to remove visible seams when lookups are at the edge of the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
	return textureHandle;
}
