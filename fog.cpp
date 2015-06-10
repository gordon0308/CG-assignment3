
#include <iostream>
#ifdef __APPLE__
#include <GLUT/glew.h>
//#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
//#include <GL/gl.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Sphere.hpp"
#include "Shader.hpp"
// #include "tiny_obj_loader.h"
// #include "tiny_obj_loader.cc"
#include "model.hpp"
#include "InputState.h"

typedef enum CameraType {
    CAM_DEFAULT,
    CAM_ORTHO,
    CAM_PERSP
} CameraType;
static CameraType Camera = CAM_PERSP;

 #include "headerLibrary.h"


#define VALS_PER_VERT 3
#define VALS_PER_TEX 2
#define VALS_PER_COLOUR 4
#define CUBE_NUM_TRIS 12      // number of triangles in a cube (2 per face)
#define CUBE_NUM_VERTICES 8     // number of vertices in a cube`
 #define VALS_PER_VERT 3
#define VALS_PER_NORMAL 3

// A cube
unsigned int cubeVaoHandle;

unsigned int sphereVaoHandle;

// Window width and height
unsigned int winW = 640;
unsigned int winH = 480;

float positionSpheres = - 10.0f;
float rotationSpheres = 0.0f;

unsigned int modelVAOHandle;
unsigned int skyVAO;
// A cube
unsigned int vaoHandle;

model craft;
model car;
model sky;
Sphere *sphere;

// An accumulation of the input for our camera rotation
float xRot = 0.0f;
float yRot = 0.0f;

// Our shader program
GLuint skyID;
GLuint lightProgramID;
GLuint carID;
GLuint craftID;
GLuint tableprogramID;
unsigned int tableVaoHandle;
unsigned int cubeMapTextureHandle;


int winX = 640;
int winY = 480;
float lightX = 0.0f;
float lightY = 5.0f;
float lightZ = -2.0f;

typedef enum ModeType {
    MODE_FILL,
    MODE_LINE
} ModeType;
static ModeType Mode = MODE_FILL;

GLuint *tex_cube=new GLuint[6];
glm::mat4 projection;

InputState Input;

int createSphereVAO()
{
    if (!sphere) {
        fprintf(stderr, "Error: initalise sphere before creating VAO!\n");
    }
    
    glGenVertexArrays(1, &sphereVaoHandle);
    glBindVertexArray(sphereVaoHandle);

    int vertLoc = glGetAttribLocation(lightProgramID, "a_vertex");
    int normLoc = glGetAttribLocation(lightProgramID, "a_normal");

    unsigned int buffer[3];
    glGenBuffers(3, buffer);

    // Set vertex position attribute
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * sphere->vertCount,
                 sphere->vertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(vertLoc);
    glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);
        
    // Normal attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * sphere->normCount,
                 sphere->normals,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(normLoc);
    glVertexAttribPointer(normLoc, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);

    // Vertex indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(unsigned int) * sphere->indCount,
                 sphere->indices,
                 GL_STATIC_DRAW);

    return 0; // success
}


int createTableVAO()
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
    float cubeColours[ CUBE_NUM_VERTICES*VALS_PER_COLOUR ] = {
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    };

    // Each square face is made up of two triangles
    unsigned int indices[CUBE_NUM_TRIS * 3] = {
        0,1,2, 2,3,0,
        1,5,6, 6,2,1,
        5,4,7, 7,6,5,
        4,0,3, 3,7,4,
        3,2,6, 6,7,3,
        4,5,1, 1,0,4
    };
    

    unsigned int vaoHandle;
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    int vertLoc = glGetAttribLocation(tableprogramID, "a_vertex");
    int colourLoc = glGetAttribLocation(tableprogramID, "a_colour");

    //******** Drawing the cube *******

    // Buffers to store position, colour and index data
    unsigned int buffer[3];
    glGenBuffers(3, buffer);

    // Set vertex position
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, 
                 sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vertLoc);
    glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

    // Colour attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, 
                 sizeof(cubeColours), cubeColours, GL_STATIC_DRAW);
    glEnableVertexAttribArray(colourLoc);
    glVertexAttribPointer(colourLoc, VALS_PER_COLOUR, GL_FLOAT, GL_FALSE, 0, 0);

    // Set element attributes. Notice the change to using GL_ELEMENT_ARRAY_BUFFER
    // We don't attach this to a shader label, instead it controls how rendering is performed
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 sizeof(indices), indices, GL_STATIC_DRAW);   
    
    // Un-bind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    return vaoHandle;
}


static void SetCamera()
{
    
    
    if ( Camera == CAM_ORTHO ) 
    {
        projection = glm::ortho( -winX * 0.005, winX * 0.005,
                                 -winY * 0.005, winY * 0.005,
                                 0.0, 10.0 );
    }
    else if ( Camera == CAM_PERSP )
    {
        // FOV angle chosen so front face of cube (z=-4)
        // is approx the same as for orthographic view
        projection = glm::perspective( 14.80f, (float) winX / winY, 3.0f, 100.0f );
    }
    else {
        fprintf(stderr, "Warning: Projection matrix is identity. Object may not be visible.\n");
    }

    // Load it to the shader program
    int projHandle = glGetUniformLocation(lightProgramID, "projection_matrix");

    if (projHandle == -1) {
        std::cout << "Uniform: projection_matrix was not an active uniform label\n";
    }
    glUniformMatrix4fv( projHandle, 1, false, glm::value_ptr(projection) );
        
}

/**
 * Sets the shader uniforms and generic attributes
 * @param program, Shader program object to use
 */
int setupLight() {

    SetCamera();

//Sphere
    int projHandle = glGetUniformLocation(lightProgramID, "projection_matrix");
    if (projHandle == -1) {
        fprintf(stderr, "Error updating proj matrix\n");
        return 1;
    }
    glUniformMatrix4fv( projHandle, 1, false, glm::value_ptr(projection) );
    sphere = new Sphere(1.0f, 16, 16);
    createSphereVAO();  

    // Uniform lighting variables
    int lightambientHandle = glGetUniformLocation(lightProgramID, "light_ambient");
    int lightdiffuseHandle = glGetUniformLocation(lightProgramID, "light_diffuse");
    int lightspecularHandle = glGetUniformLocation(lightProgramID, "light_specular");
    if ( lightambientHandle == -1 ||
         lightdiffuseHandle == -1 ||
         lightspecularHandle == -1) {
        fprintf(stderr, "Error: can't find light uniform variables\n");
        return 1;
    }
    float lightambient[3] = { 0.12f, 0.1f, 0.1f };  // ambient light components
    float lightdiffuse[3] = { 0.0f, 1.0f, 0.0f };   // diffuse light components
    float lightspecular[3] = { 1.0f, 1.0f, 1.0f };  // specular light components
    glUniform3fv(lightambientHandle, 1, lightambient);
    glUniform3fv(lightdiffuseHandle, 1, lightdiffuse);
    glUniform3fv(lightspecularHandle, 1, lightspecular);    
    //glUseProgram(lightProgramID);

    return 0;
} 

int setupTable() {

    tableVaoHandle = createTableVAO();
    int lightambientHandle = glGetUniformLocation(lightProgramID, "light_ambient");
    int lightdiffuseHandle = glGetUniformLocation(lightProgramID, "light_diffuse");
    int lightspecularHandle = glGetUniformLocation(lightProgramID, "light_specular");
    if ( lightambientHandle == -1 ||
         lightdiffuseHandle == -1 ||
         lightspecularHandle == -1) {
        fprintf(stderr, "Error: can't find light uniform variables\n");
        return 1;
    }
    float lightambient[3] = { 0.12f, 0.1f, 0.1f };  // ambient light components
    float lightdiffuse[3] = { 0.0f, 1.0f, 0.0f };   // diffuse light components
    float lightspecular[3] = { 1.0f, 1.0f, 1.0f };  // specular light components
    glUniform3fv(lightambientHandle, 1, lightambient);
    glUniform3fv(lightdiffuseHandle, 1, lightdiffuse);
    glUniform3fv(lightspecularHandle, 1, lightspecular);  

    glUseProgram(tableprogramID);


    return 0;   // return success
}

int setup() {

//Skybox
    cubeVaoHandle = sky.createSkyVAO(skyID);  
    // Sky -assign cubemap textures
    sky.initCubeMap();
    glUseProgram(skyID);

    return 0;   // return success
}

/********************************************************************************************Render*/
/**
 * Renders a frame of the state and shaders we have set up to the window
 */
void render() {

    // GL state
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(skyID);

    int modelviewHandle = glGetUniformLocation(skyID, "modelview_matrix");
    if ((modelviewHandle == -1))
        exit(1);

    // We reset the camera for this frame
    glm::mat4 cameraMatrix(1.0f);
    
    // Rotations are always relative to the origin, so we step back from the origin
    // (move the whole world AWAY from us 5 units in the z direction)
    // Circle the camera around the scene by spinning the whole world around the origin
    std::printf("%f \n", xRot);
    cameraMatrix = glm::translate(cameraMatrix, glm::vec3(0.0f, 0.0f, -1.0f+xRot));
    cameraMatrix = glm::rotate(cameraMatrix, yRot, glm::vec3(0.0f, 1.0f, 0.0f));
    //cameraMatrix = glm::rotate(cameraMatrix, xRot, glm::vec3(1.0f, 0.0f, 0.0f));
 
//Lighting render
    glUseProgram(lightProgramID);
    int mvHandle = glGetUniformLocation(lightProgramID, "modelview_matrix");
    int normHandle = glGetUniformLocation(lightProgramID, "normal_matrix");
    int lightposHandle = glGetUniformLocation(lightProgramID, "light_pos");
    if (mvHandle == -1 || normHandle==-1 || lightposHandle == -1) {
        fprintf(stderr, "Error: can't find matrix uniforms\n");
        exit(1);
    }
    
    // Update the light position
    float lightPos[4] = { lightX, lightY, lightZ, 1.0f };   
    glUniform4fv(lightposHandle, 1, lightPos); 

    // Uniform variables defining material colours
    // These can be changed for each sphere, to compare effects
    int mtlambientHandle = glGetUniformLocation(lightProgramID, "mtl_ambient");
    int mtldiffuseHandle = glGetUniformLocation(lightProgramID, "mtl_diffuse");
    int mtlspecularHandle = glGetUniformLocation(lightProgramID, "mtl_specular");
    if ( mtlambientHandle == -1 ||
         mtldiffuseHandle == -1 ||
         mtlspecularHandle == -1) {
        fprintf(stderr, "Error: can't find material uniform variables\n");
        exit(1);
    }
    
    float mtlambient[3] = { 0.12f, 0.1f, 0.1f };    // ambient material
    float mtldiffuse[3] = { 0.0f, 1.0f, 0.0f }; // diffuse material
    float mtlspecular[3] = { 1.0f, 1.0f, 1.0f };    // specular material

    glUniform3fv(mtlambientHandle, 1, mtlambient);
    glUniform3fv(mtldiffuseHandle, 1, mtldiffuse);
    glUniform3fv(mtlspecularHandle, 1, mtlspecular);    

    
    glm::mat4 mvMatrix=cameraMatrix;
    glm::mat3 normMatrix;
       // Set VAO to the sphere mesh
    glBindVertexArray(sphereVaoHandle);

    // Render three spheres in a row
    // We compute the normal matrix from the current modelview matrix
    // and give it to our program
    mvMatrix = glm::translate(mvMatrix, glm::vec3(0.0f, -0.0f, -20.0f));
    mvMatrix = glm::scale(mvMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
    normMatrix = glm::mat3(mvMatrix);
    glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );  // Middle
    glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
    glDrawElements(GL_TRIANGLES, sphere->indCount, GL_UNSIGNED_INT, 0);


    // Set VAO to the sphere mesh

    // mvMatrix = glm::translate(mvMatrix, glm::vec3(-3.0f, 0.0f, 0.0f));
    // normMatrix = glm::mat3(mvMatrix);
    // glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );  // Left
    // glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
    // glDrawElements(GL_TRIANGLES, sphere->indCount, GL_UNSIGNED_INT, 0);

    // mvMatrix = glm::translate(mvMatrix, glm::vec3(6.0f, 0.0f, 0.0f));
    // normMatrix = glm::mat3(mvMatrix);
    // glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );  // right
    // glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
    // glDrawElements(GL_TRIANGLES, sphere->indCount, GL_UNSIGNED_INT, 0);
    
//Table
    glUseProgram(tableprogramID);

    modelviewHandle = glGetUniformLocation(tableprogramID, "modelview_matrix");
    if (modelviewHandle == -1)
        exit(1);

    // We reset the camera for this frame
    //glm::mat4 cameraMatrix;
    
    // Rotations are always relative to the origin, so we step back from the origin
    // (move the whole world AWAY from us 5 units in the z direction)

    //******* Use the view matrix to move the cube in front of the camera *******
    mvMatrix = glm::translate(mvMatrix, glm::vec3(0.0f, 0.0f, 10.0f));

    // Circle the camera around the scene by spinning the whole world around the origin
    mvMatrix = glm::scale(mvMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
    
    // Set VAO to the square model and draw three in different positions
    glBindVertexArray(tableVaoHandle);

    // We just want to draw the cube unmodified at the origin, so we just
    // take a copy of the camera matrix with its current transformations
    //  glm::mat4 modelview;
    //  modelview = glm::translate(cameraMatrix, glm::vec3(0.0f, 0.0f, 0.0f));    
    //    modelview = glm::rotate(modelview, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));    
    glUniformMatrix4fv( modelviewHandle, 1, false, glm::value_ptr(cameraMatrix) );

    //******** Drawing the cube *******
    glDrawElements(GL_TRIANGLES, CUBE_NUM_TRIS * 3, GL_UNSIGNED_INT, 0);    // New call
    

    glBindVertexArray(0);


//SkyBox
    glDepthMask (GL_FALSE);
    glUseProgram (skyID);
    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_CUBE_MAP, *tex_cube);
    glDrawArrays (GL_TRIANGLES, 0, 36);
    glDepthMask (GL_TRUE);

    // Set VAO to the square model and draw three in different positions
    glBindVertexArray(cubeVaoHandle);
    
    glm::mat4 skyCube;
    skyCube = glm::mat4( cameraMatrix );
    skyCube = glm::scale( skyCube, glm::vec3(40.0f, 40.0f, 40.0f ));
    glUniformMatrix4fv( modelviewHandle, 1, false, glm::value_ptr(skyCube) );
    glDrawElements(GL_TRIANGLES, CUBE_NUM_TRIS * VALS_PER_VERT, GL_UNSIGNED_INT, 0);    // New call
    glBindVertexArray(0);


    glutSwapBuffers();
    glFlush();

}

void SpecialFunc( int key, int x, int y )
{
    switch(key) 
    {
        case GLUT_KEY_LEFT:
            yRot -= 0.05;
            lightX -= LIGHT_SPEED;
            break;
        case GLUT_KEY_RIGHT:
            yRot += 0.05;
            lightX += LIGHT_SPEED;
            break;
        case GLUT_KEY_UP:
            if(xRot>=-41&& xRot<38){
                xRot += 0.5;
                lightY -= LIGHT_SPEED;
            }
            else if (xRot==41){
                xRot -= 0.5;
                lightY -= LIGHT_SPEED;
            }
            break;
        case GLUT_KEY_DOWN:
            if(xRot>-41&& xRot<=38){
                xRot -= 0.5;
                lightY += LIGHT_SPEED;
            }
            else if (xRot==-41){
                xRot += 0.5;
                lightY += LIGHT_SPEED;
            }
            break;
    }
    glutPostRedisplay();
}


/**
 * Called while a keyboard key press is detected
 * This GLUT functions is not OpenGL specific, but allows interactivity to our programs
 * @param key, the keyboard key that made the event
 * @param x, not used
 * @param y, not used
 */
void keyboardDown(unsigned char key, int x, int y) {

    // We simply check the key argument against characters we care about, in this case A and D
    switch(key) 
    {
        case 27: // escape key pressed
            exit(0);
            break;

        case 'd': // Toggle depth test
            if ( glIsEnabled( GL_DEPTH_TEST ) ) {
                glDisable( GL_DEPTH_TEST );
            }
            else {
                glEnable( GL_DEPTH_TEST );
            }
            glutPostRedisplay();
            break;

        case 'p': // Toggle default/ortho/persp view
            if ( Camera == CAM_DEFAULT ) {
                Camera = CAM_ORTHO;
            }
            else if ( Camera == CAM_ORTHO ) {
                Camera = CAM_PERSP;
            }
            else {
                Camera = CAM_DEFAULT;
            }
            SetCamera();
            glutPostRedisplay();
            break;
    }
}


void MouseFunc(int button, int state, int x, int y)
{
    // Update a global set of variables 
    // to avoid cluttering up the callbacks
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            std::cout<<"Click"<<std::endl;
            if (state == GLUT_DOWN)
            {
                Input.lMousePressed = true;
                Input.prevX = x;
                Input.prevY = y;
            }
            else
                Input.lMousePressed = false;
            break;
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN)
                Input.rMousePressed = true;
            else 
                Input.rMousePressed = false;
            break;

        default:
            break;
    }
}

void MotionFunc(int x, int y)
{
    int xDiff = x - Input.prevX;
    int yDiff = y - Input.prevY;
    Input.deltaX += xDiff;
    Input.deltaY += yDiff;
    Input.prevX = x;
    Input.prevY = y;
}

void reshapeFunc( int x, int y ) 
{
    winX = x;
    winY = y;
    glViewport(0, 0, (GLsizei)x, (GLsizei)y); // Set our viewport to the size of our window  

    SetCamera();
    
}


/**
 * Program entry. Sets up OpenGL state, GLSL Shaders and GLUT window and function call backs
 * Takes no arguments
 */
int main(int argc, char **argv) {
    std::cout << "Controls: arrow keys rotate cube\n";

    // Set up GLUT window
    glutInit(&argc, argv);
    glutInitWindowPosition(100, 0);
    glutInitWindowSize(winX, winY);

#ifdef __APPLE__    
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
#else
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
#endif
    glutCreateWindow("Many Attributes");

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    lightProgramID = LoadShaders("pv-light.vert", "pv-light.frag");
    if (lightProgramID == 0) {
        fprintf(stderr, "Can't compile shaders!\n");
        return 1;
    }
    glUseProgram(lightProgramID);
    if (setupLight() !=0)
        return 1;

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    tableprogramID = LoadShaders("pv-light.vert", "pv-light.frag");
    if (tableprogramID == 0)
        return 1;
    
    if (setupTable() !=0)
        return 1;
    
    skyID = LoadShaders("sky.vert", "sky.frag");
    if (skyID == 0)
        return 1;
    glUseProgram(skyID);
    if (setup() !=0)
        return 1;

    

    // Here we set a new function callback which is the GLUT handling of keyboard input
    glutKeyboardFunc(keyboardDown);
    glutSpecialFunc(SpecialFunc);
    glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc); 
    glutDisplayFunc(render);
    glutReshapeFunc(reshapeFunc);
    glutMainLoop();

    return 0;
}

