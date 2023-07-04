//#define GL_GLEXT_PROTOTYPES
#include<GL/glcorearb.h>
#include<GL/wglext.h>
#include "Intro.h"
#define _USE_MATH_DEFINES
#include<math.h>
#include<stdlib.h>
#include<lodepng.h>
#include "Camera.h"
#define XSCALE 0.09f
#define YSCALE 0.12f

//ALL NUMBERS THAT ARE TESTED TO THE CUTSCENE TIMER
    #define TIMEPT0 2.0f //The Time when the triple triangle stops rotating
    #define TIMEPT1 2.2f //The Time The triple triangle stops being animated
    #define TIMEPT2 4.5f //fade begins
    #define TIMEPT3 5.0f //Camera Animation Starts, Triengine logo dissappears, meshes are deleted/repleaced
    #define TIMEPT4 5.2f //white background fades
    #define TIMEPT5 6.58333f //Camera piecewise animation interval 1
    #define TIMEPT6 9.625f //Camera piecewise animation interval 2, end of white background fade
    #define TIMEPT7 10.7917f //Camera piecewise animation interval 3
    #define TIMEPT8 10.7917f //Camera piecewise animation interval 4, end of camera zoom,camera look-down begin,play icon rises,Credit mesh replaced
    #define TIMEPT9 10.7917f //Camera piecewise animation interval 5
//

static float CS_Timer; //CutScene timer
static int AreMeshesDeleted;

static GLuint M_VAO; //The spinning Triangles
static GLuint M_VBO1;
static GLuint M_VBO2;
static GLuint M_EBO;

static GLuint M2_VAO; //TriEngine logo
static GLuint M2_VBO1;
static GLuint M2_VBO2;
static GLuint M2_EBO;

//static GLuint M3_VAO; //Unused yet
//static GLuint M3_VBO;
//static GLuint M3_EBO;

extern GLuint CurtainVAO; //white background
extern GLuint CurtainVBO;
extern GLuint CurtainEBO;
extern unsigned short CurtainCount; //sizeof(curtainInd)

extern GLuint SkyBoxVAO; //The sky "box" model is global so put here
extern GLuint SkyBoxVBO;
extern GLuint SkyBoxEBO;
extern unsigned short SkyBoxCount;//sizeof(SkyBoxInd)

GLuint TextureVAO; //"Play" and "Ball" button Mesh
GLuint TextureVBO;
GLuint TextureEBO;

GLuint TextureID;
GLuint TextureID2;

extern float ViewMatrix[4][4];
extern float ProjMatrix[4][4];


extern PFNGLCREATESHADERPROC glCreateShader; //inherit all the GL function pointers
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLSHADERSOURCEPROC glShaderSource ;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLVALIDATEPROGRAMPROC glValidateProgram;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLUNIFORM4FPROC glUniform4f;
extern PFNGLACTIVETEXTUREPROC glActiveTexture;

extern float DeltaTime;
extern float MouseX,MouseY;
extern int MouseDown;

extern GLuint SPID;
extern GLuint AlphaLocation;
extern GLuint ProjViewMatrixLocation;
extern GLuint SPID2;
extern GLuint AlphaLocation2; //Uniform locations for SPID2
extern GLuint ProjViewMatrixLocation2;
extern GLuint SPID3;
extern GLuint ColorLocation3; //Uniform locations for SPID3
extern GLuint SPID4;

GLAPI void APIENTRY glClear (GLbitfield mask);
GLAPI void APIENTRY glClearColor (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GLAPI void APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const void *indices);
GLAPI void APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI void APIENTRY glTexParameteri( GLenum target, GLenum pname, GLint param );
GLAPI void APIENTRY glTexParameterf( GLenum target, GLenum pname, GLfloat param );
GLAPI void APIENTRY glTexImage2D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
GLAPI void APIENTRY glBindTexture( GLenum target, GLuint texture );
GLAPI void APIENTRY glGenTextures( GLsizei n, GLuint *textures );

void OpenOriginalGame(void);

unsigned char PlayButtonClicked;

#define ITERATIONS 3


void InitCutscene(void){
    //Initialise the globals
    AreMeshesDeleted = 0;
    CS_Timer = -2.0f; //set as such to delay the cutscene by 1 second(changing the timing of the whole scene will take forever)
    PlayButtonClicked = 0;
    //Adjust The triengine mesh in this for loop
    for(int i = 0;i < sizeof(TrienginePos)/sizeof(GLfloat);i += 3){
        TrienginePos[i] *= XSCALE;
        TrienginePos[i] += 0.36f;
        GLfloat Temp = TrienginePos[i+1]; //swap y and z
        TrienginePos[i+1] = -TrienginePos[i+2] * YSCALE;
        TrienginePos[i+2] = Temp;
    }

    for(int i = 0;i < sizeof(TriengineInd)/sizeof(GLubyte);++i){
        --TriengineInd[i];
    }

    glGenVertexArrays(1, &M_VAO);
    glBindVertexArray(M_VAO);

    glGenBuffers(1, &M_VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, M_VBO1);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(PreTansformedMeshPos), PreTansformedMeshPos, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &M_VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, M_VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PreTansformedMeshCol), PreTansformedMeshCol, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &M_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, M_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MeshIndices), MeshIndices, GL_STATIC_DRAW);

    glGenVertexArrays(1,&M2_VAO);
    glBindVertexArray(M2_VAO);

    glGenBuffers(1,&M2_VBO1);
    glBindBuffer(GL_ARRAY_BUFFER,M2_VBO1);
    //glBufferData(GL_ARRAY_BUFFER,sizeof(TrienginePos),TrienginePos,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);

    glGenBuffers(1,&M2_VBO2);
    glBindBuffer(GL_ARRAY_BUFFER,M2_VBO2);
    //glBufferData(GL_ARRAY_BUFFER,sizeof(TrienginePos),TriengineCol,GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);

    glGenBuffers(1,&M2_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,M2_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(TriengineInd),TriengineInd,GL_STATIC_DRAW);

    glGenVertexArrays(1,&TextureVAO);
    glBindVertexArray(TextureVAO);

    glGenBuffers(1,&TextureVBO);
    glBindBuffer(GL_ARRAY_BUFFER,TextureVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(TextureMesh),TextureMesh,GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(GLfloat)*4,0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(GLfloat)*4,(void*)(sizeof(GLfloat)*2));

    glGenBuffers(1,&TextureEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,TextureEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(TextureInd),TextureInd,GL_STATIC_DRAW);

    glActiveTexture(GL_TEXTURE0);
    {/*Generating first texture*/
        glGenTextures(1,&TextureID);
        glBindTexture(GL_TEXTURE_2D,TextureID);

        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

        unsigned char* DecodedImage;
        unsigned width;
        unsigned height;
        lodepng_decode32(&DecodedImage,&width,&height,Play_Png,sizeof(Play_Png));

        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,DecodedImage);

        free(DecodedImage);

        /*Second Texture*/

        glGenTextures(1,&TextureID2);
        glBindTexture(GL_TEXTURE_2D,TextureID2);

        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

        lodepng_decode32(&DecodedImage,&width,&height,Ball_Png,sizeof(Ball_Png));

        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,DecodedImage);

        free(DecodedImage);
    }
}

void AnimateCutscene(void){
         glUseProgram(SPID);
     //Animate the Camera
        {
            if(CS_Timer >= TIMEPT3 && CS_Timer < TIMEPT8){
                CAMERADATA TempCamData;
                TempCamData.Xpos = 0.04f;
                TempCamData.Ypos = 0.0f;
                if(CS_Timer < TIMEPT5){
                 float x = (CS_Timer - TIMEPT3)/(TIMEPT5 - TIMEPT3);
                 for (int i = ITERATIONS;i > 0; i -= 1){x -= ((((13.330657f)*x+(-13.277727f))*x+(36.947070f))*x+(1.000000f - (CS_Timer - TIMEPT3)*24.0f))/((3*(13.330657f)*x+2*(-13.277727f))*x+(36.947070f));}
                 TempCamData.Zpos = (((-60.802109f)*x+(94.239621f))*x+(-0.000004f))*x+(0.000000f);
                }else if(CS_Timer < TIMEPT6){
                 float x = (CS_Timer - TIMEPT5)/(TIMEPT6 - TIMEPT5);
                 for (int i = ITERATIONS;i > 0; i -= 1){x -= ((((-71.363991f)*x+(107.533951f))*x+(36.830040f))*x+(38.000000f - (CS_Timer - TIMEPT3)*24.0f))/((3*(-71.363991f)*x+2*(107.533951f))*x+(36.830040f));}
                 TempCamData.Zpos = (((5.127213f)*x+(-8.129299f))*x+(4.439255f))*x+(33.437508f);
                }else if(CS_Timer < TIMEPT7){
                 float x = (CS_Timer - TIMEPT6)/(TIMEPT7 - TIMEPT6);
                 for (int i = ITERATIONS;i > 0; i -= 1){x -= ((((50.416962f)*x+(-96.039368f))*x+(73.622406f))*x+(111.000000f - (CS_Timer - TIMEPT3)*24.0f))/((3*(50.416962f)*x+2*(-96.039368f))*x+(73.622406f));}
                 TempCamData.Zpos = (((-12.936253f)*x+(15.935818f))*x+(6.937122f))*x+(34.874676f);
                }

                //TempCamData.Zpos = 34.3;
                GenViewMatrix(&TempCamData,ViewMatrix,GVM_POSITION);
                float ProjViewMatrix[4][4];
                Mul4x4Matrix(ProjMatrix,ViewMatrix,ProjViewMatrix);
                glUniformMatrix4fv(ProjViewMatrixLocation,1,GL_TRUE,&ProjViewMatrix[0][0]);
                glUseProgram(SPID2);
                GenViewMatrix(&TempCamData,ViewMatrix,GVM_IDENTITY);
                Mul4x4Matrix(ProjMatrix,ViewMatrix,ProjViewMatrix);
                glUniformMatrix4fv(ProjViewMatrixLocation2,1,GL_TRUE,&ProjViewMatrix[0][0]);
            }

            if(CS_Timer >= TIMEPT8){
                CAMERADATA TempCamData;
                TempCamData.Xpos = 5.0f;
                TempCamData.Ypos = -15.0f;
                TempCamData.Zpos = 40.0f;
                TempCamData.Rotation.Angles3.RotX = (float)(-(M_PI/2) + ((M_PI/2)/pow(20.0f,CS_Timer - TIMEPT8)));
                TempCamData.Rotation.Angles3.RotY = 0.0f;
                TempCamData.Rotation.Angles3.RotZ = 0.0f;
                GenViewMatrix(&TempCamData,ViewMatrix,GVM_ANGLES_ROTATION|GVM_IDENTITY);
                glUseProgram(SPID2);
                float ProjViewMatrix[4][4];
                Mul4x4Matrix(ProjMatrix,ViewMatrix,ProjViewMatrix);
                glUniformMatrix4fv(ProjViewMatrixLocation2,1,GL_TRUE,&ProjViewMatrix[0][0]);
                glUseProgram(SPID);
                GenViewMatrix(&TempCamData,ViewMatrix,GVM_POSITION|GVM_ANGLES_ROTATION|GVM_IDENTITY);
                Mul4x4Matrix(ProjMatrix,ViewMatrix,ProjViewMatrix);
                glUniformMatrix4fv(ProjViewMatrixLocation,1,GL_TRUE,&ProjViewMatrix[0][0]);
            }
        }




    /*Triangle Animation Code*/{
        float Trans_F; //F for fraction(fraction of triangle)
        float Trans_I; //I for Inwards(Inward displacement of tris)
        float Trans_S; //S for Scale
        float Trans_R;//R for Rotation

        if(CS_Timer < TIMEPT1){ //the reason for this is that we dont bother "animating" the tri triangles after they stop
            if(CS_Timer < TIMEPT0){
                Trans_R = 5.235987756f+ CS_Timer*(-7.9838234045f + CS_Timer *(+3.2245559215f + CS_Timer * -0.401720273f)); //polynomials
                Trans_I = 0.3f+ CS_Timer*(0.46166666666667f + CS_Timer *(-0.125f + CS_Timer * 0.0033333333333333f));
                Trans_S = 3.5f+ CS_Timer*(-4.615f + CS_Timer *(+2.58f + CS_Timer * -0.48f));
            }else{
                Trans_R = (float)(-M_PI/3);
                Trans_I = 0.75f;
                Trans_S = 0.75f;
            }


            GLfloat TempPos[sizeof(PreTansformedMeshPos)/sizeof(GLfloat)];
            for (unsigned int i = 0;i < sizeof(TempPos)/sizeof(GLfloat);i++)TempPos[i] = PreTansformedMeshPos[i];

            TempPos[0] += Trans_I * (0.8660254f);  //Translating the Triangles towards center
            TempPos[1] += Trans_I * (0.5f);        //only 2 vertecies per triangle handled here
            TempPos[3] += Trans_I * (0.8660254f);  //as the other two are calculated based on the previous two
            TempPos[4] += Trans_I * (0.5f);
            TempPos[12] += Trans_I * (-0.8660254f);
            TempPos[13] += Trans_I * (0.5f);
            TempPos[15] += Trans_I * (-0.8660254f);
            TempPos[16] += Trans_I * (0.5f);
            TempPos[25] += Trans_I * (-1.0f);
            TempPos[28] += Trans_I * (-1.0f);

            Trans_F = (TempPos[1] - TempPos[28])/-0.4330127019f;

            if (Trans_F > 1.0f)Trans_F = 1.0f;


            TempPos[6] = TempPos[0] + (0.5f * Trans_F); //Clipping the leftmost triangle
            TempPos[7] = TempPos[1];
            TempPos[9] = TempPos[3] + (0.25f * Trans_F);
            TempPos[10] = TempPos[4] + (0.433013f * Trans_F);

            TempPos[21] = -TempPos[6]; //Clipping Rightmost triangle
            TempPos[22] = TempPos[7];
            TempPos[18] = -TempPos[9];
            TempPos[19] = TempPos[10];

            TempPos[30] = TempPos[27] + (0.25f * Trans_F); //Clipping Topmost triangle
            TempPos[31] = TempPos[28] + (-0.43301f * Trans_F);
            TempPos[33] = -TempPos[30];
            TempPos[34] = TempPos[31];

            for(int i = 0;i < sizeof(TempPos)/sizeof(GLfloat);i += 3){
                //Scaling
                TempPos[i] *= Trans_S;
                TempPos[i + 1] *= Trans_S;

                //Rotating
                GLfloat TempYpos = (float)(sin(Trans_R)*TempPos[i] + cos(Trans_R)*TempPos[i + 1]); //The reason to use a TempYpos
                TempPos[i] = (float)(cos(Trans_R)*TempPos[i] - sin(Trans_R)*TempPos[i + 1] - 0.96);   //is that the transformation
                TempPos[i + 1] = TempYpos;                                                //of the X xomponent depends of the
                                                                                          //*PreRotated* Y component(which is TempPos[i+i]
            }                                                                             //in this case).



            glBindBuffer(GL_ARRAY_BUFFER,M_VBO1);
            glBufferData(GL_ARRAY_BUFFER,sizeof(TempPos),NULL,GL_STATIC_DRAW); //orphaning
            glBufferData(GL_ARRAY_BUFFER,sizeof(TempPos),TempPos,GL_STATIC_DRAW);
        }
    }



    /*Triengine Text Animation*/{
        if(CS_Timer < TIMEPT3){  //no point in animating triengine after it dissappeared
            GLfloat TempTriEpos[sizeof(TrienginePos)/sizeof(GLfloat)];

            GLfloat TempTriEcol[sizeof(TrienginePos)/sizeof(GLfloat)];

            float BoundingX = (-8.6f) + (3.2f)*CS_Timer; //think a vertical line that goes right across screen

            for (int i = 0;i < sizeof(TrienginePos)/sizeof(GLfloat);i += 3){ //Both TriPos and TriCol are arrays of glfloats with same length
                TempTriEpos[i] = TrienginePos[i];                            //so just make them share the for loop
                float DeltaX = BoundingX - TempTriEpos[i];
                if(DeltaX > 0.0f){
                    TempTriEpos[i + 1] = TrienginePos[i + 1] + ((float)rand()/(float)RAND_MAX - 0.5f) * max((1/(4*DeltaX*DeltaX+0.067f)),0.0f);
                }else{
                    TempTriEpos[i + 1] = 0.0f;
                }
                TempTriEpos[i + 2] = TrienginePos[i + 2];

                if(DeltaX < 0.3f){  //ahhhhh magic numbers!
                    TempTriEcol[i] = 0.21569f;
                    TempTriEcol[i+1] = 0.819608f;
                    TempTriEcol[i+2] = 1.0f;
                }else if(DeltaX < 0.6){
                    TempTriEcol[i] = 1.0f;
                    TempTriEcol[i+1] = 0.5686f;
                    TempTriEcol[i+2] = 0.0f;
                }else if(DeltaX < 1){
                    TempTriEcol[i] = 0.5490f;
                    TempTriEcol[i+1] = 0.9961f;
                    TempTriEcol[i+2] = 0.1333f;
                }else{
                    TempTriEcol[i] = 0.639216f;
                    TempTriEcol[i+1] = 0.776471f;
                    TempTriEcol[i+2] = 0.8f;
                }
            }

            glBindBuffer(GL_ARRAY_BUFFER,M2_VBO1);
            glBufferData(GL_ARRAY_BUFFER,sizeof(TempTriEpos),TempTriEpos,GL_DYNAMIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER,M2_VBO2);
            glBufferData(GL_ARRAY_BUFFER,sizeof(TempTriEcol),TempTriEcol,GL_DYNAMIC_DRAW);
        }
    }

    /*Play Button Animation*/{
        if(CS_Timer >= TIMEPT8){
        TextureMesh[1] = (float)(PLAY_TOP - 1.0f/(pow(15,CS_Timer - TIMEPT8)));
        TextureMesh[5] = (float)(PLAY_BOTTOM - 1.0f/(pow(15,CS_Timer - TIMEPT8)));
        TextureMesh[9] = (float)(PLAY_BOTTOM - 1.0f/(pow(15,CS_Timer - TIMEPT8)));
        TextureMesh[13] = (float)(PLAY_TOP - 1.0f/(pow(15,CS_Timer - TIMEPT8)));

        float Ball_X = (1.3f) + (3.0f) * (float)cos((2.0)*(CS_Timer - TIMEPT8));
        float Ball_Y = -20.0f;
        float Ball_Z = (40.0f) + (3.0f) * (float)sin((2.0)*(CS_Timer - TIMEPT8));

        float Ball_X_R = ViewMatrix[0][0] * Ball_X + ViewMatrix[0][1] * Ball_Y + ViewMatrix[0][2] * Ball_Z + ViewMatrix[0][3];
        float Ball_Y_R = ViewMatrix[1][0] * Ball_X + ViewMatrix[1][1] * Ball_Y + ViewMatrix[1][2] * Ball_Z + ViewMatrix[1][3];
        float Ball_Z_R = ViewMatrix[2][0] * Ball_X + ViewMatrix[2][1] * Ball_Y + ViewMatrix[2][2] * Ball_Z + ViewMatrix[2][3];

        TextureMesh[16] = (Ball_X_R + 0.5f)/Ball_Z_R;
        TextureMesh[17] = (Ball_Y_R + 0.5f)/Ball_Z_R;

        TextureMesh[20] = (Ball_X_R - 0.5f)/Ball_Z_R;
        TextureMesh[21] = (Ball_Y_R - 0.5f)/Ball_Z_R;

        TextureMesh[24] = (Ball_X_R + 0.5f)/Ball_Z_R;
        TextureMesh[25] = (Ball_Y_R - 0.5f)/Ball_Z_R;

        TextureMesh[28] = (Ball_X_R - 0.5f)/Ball_Z_R;
        TextureMesh[29] = (Ball_Y_R + 0.5f)/Ball_Z_R;

        glBindBuffer(GL_ARRAY_BUFFER,TextureVBO);
        glBufferData(GL_ARRAY_BUFFER,sizeof(TextureMesh),TextureMesh,GL_DYNAMIC_DRAW);
        if((CS_Timer > TIMEPT8 + 0.5f)&&(!PlayButtonClicked)&&(MouseY < PLAY_TOP)&&(MouseY > PLAY_BOTTOM)&&(MouseX > PLAY_LEFT)&&(MouseX < PLAY_RIGHT)&&(MouseDown)){
            OpenOriginalGame();
            PlayButtonClicked = 1;
        }
        }
    }

    if(AreMeshesDeleted){ //Credits should be drawn below the white curtain
        glUseProgram(SPID2);
        glBindVertexArray(SkyBoxVAO);
        glDrawElements(GL_TRIANGLES,SkyBoxCount/sizeof(GLushort),GL_UNSIGNED_SHORT,0);
        glUseProgram(SPID);
        glUniform1f(AlphaLocation,1.0);
        glBindVertexArray(M_VAO);
        //note that at this point the Profection-View Matrix is handled by the camera code in the beginning
        if(AreMeshesDeleted == 1){
            glDrawElements(GL_TRIANGLES, sizeof(CreditInd) / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
        }else if(AreMeshesDeleted == 2){
            glDrawElements(GL_TRIANGLES, sizeof(CircleInd) / sizeof(GLubyte), GL_UNSIGNED_BYTE, 0);
            glUseProgram(SPID4);
            glBindVertexArray(TextureVAO);
            glBindTexture(GL_TEXTURE_2D,TextureID);
            glDrawElements(GL_TRIANGLES,sizeof(GLubyte) * 6,GL_UNSIGNED_BYTE,0);

            glBindTexture(GL_TEXTURE_2D,TextureID2);
            glDrawElements(GL_TRIANGLES,sizeof(GLubyte) * 6,GL_UNSIGNED_BYTE,(void*)(sizeof(GLubyte)*6));
        }
    }

    glUseProgram(SPID3);
    if(CS_Timer < TIMEPT4){ //transparency for the white curtain
        glUniform4f(ColorLocation3,1.0f,1.0f,1.0f,1.0f);
    }else if(CS_Timer < TIMEPT4 + 0.5f){
        glUniform4f(ColorLocation3,1.0f,1.0f,1.0f,1.0f - (2.0f)*(CS_Timer - TIMEPT4));
    }else{
        glUniform4f(ColorLocation3,1.0f,1.0f,1.0f,0.0f);
    }

    glBindVertexArray(CurtainVAO);
    glDrawElements(GL_TRIANGLES,CurtainCount/sizeof(GLubyte),GL_UNSIGNED_BYTE,0);

    glUseProgram(SPID);

    /*Fade off of the triangle + triengine logo*/{
        if(CS_Timer > TIMEPT2 && CS_Timer < TIMEPT3){
            glUniform1f(AlphaLocation,1.0f - (2.0f)*(CS_Timer - TIMEPT2));
        }
    }

    /*Deleting and Modifying Meshed*/{
        if(CS_Timer >= TIMEPT3 && AreMeshesDeleted == 0){
            AreMeshesDeleted = 1;
            glDeleteVertexArrays(1,&M2_VAO);//first delete the vertex arrays
            glDeleteBuffers(1,&M2_VBO1);//then delete the buffers used by these arrays
            glDeleteBuffers(1,&M2_VBO2);
            glDeleteBuffers(1,&M2_EBO);
            M2_VAO = (int)NULL;
            M2_VBO1 = (int)NULL;
            M2_VBO2 = (int)NULL;
            M2_EBO = (int)NULL;


            glDeleteBuffers(1,&M_VBO2);//only delete second buffer of first VAO
            M_VBO2 = (int)NULL;

            glBindVertexArray(M_VAO);
            glBindBuffer(GL_ARRAY_BUFFER,M_VBO1);
            glBufferData(GL_ARRAY_BUFFER,sizeof(CreditMesh),CreditMesh,GL_STATIC_DRAW);
            glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat)*6,0);
            glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat)*6,(void*)(sizeof(GLfloat)*3));
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,M_EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(CreditInd),CreditInd,GL_STATIC_DRAW);
        }
    }

    /*Changing Credits Mesh*/{
        if(CS_Timer >= TIMEPT8 && AreMeshesDeleted != 2){
            AreMeshesDeleted = 2;
            glBindVertexArray(M_VAO);
            glBindBuffer(GL_ARRAY_BUFFER,M_VBO1);
            glBufferData(GL_ARRAY_BUFFER,sizeof(CircleMesh),CircleMesh,GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,M_EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(CircleInd),CircleInd,GL_STATIC_DRAW);
        }
    }


    //glClear(GL_COLOR_BUFFER_BIT);
    if(!AreMeshesDeleted){ //one obviously doesnt want to draw meshes after they are deleted
        glUniformMatrix4fv(ProjViewMatrixLocation,1,GL_TRUE,&ProjMatrix[0][0]);
        glBindVertexArray(M_VAO);
        glDrawElements(GL_TRIANGLES, sizeof(MeshIndices) / sizeof(GLubyte), GL_UNSIGNED_BYTE, 0);

        glBindVertexArray(M2_VAO);
        glDrawElements(GL_TRIANGLES, sizeof(TriengineInd) / sizeof(GLubyte), GL_UNSIGNED_BYTE, 0);
    }


//    glUniform1f(AlphaLocation,1.0f);
//    glBindVertexArray(M3_VAO);
//    glDrawElements(GL_TRIANGLES, sizeof(IntroMeshInd) / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    CS_Timer += DeltaTime; //Delta time updated by external function
}

