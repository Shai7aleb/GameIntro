#include <windows.h>
#include <windowsx.h>
//#define GL_GLEXT_PROTOTYPES
#include<GL/glcorearb.h>
#include<GL/wglext.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include "main.h"
#include "Camera.h"
#define MaxVertCount 100
#define IntsPerVertex 4
#define FrameCap 144
#define FOV 90
#include<shellapi.h>


LRESULT CALLBACK WindowProcedure(HWND WindowHandle, UINT Message, WPARAM wparam, LPARAM lparam);

unsigned char Quit = 0u;

GLuint VSID; //Shaders are global to the whole program
GLuint FSID;
GLuint SPID;

GLuint RatioLocation; //Uniform locations for SPID
GLuint AlphaLocation;
GLuint ProjViewMatrixLocation;

GLuint VSID2; //Shaders are global to the whole program
GLuint FSID2;
GLuint SPID2;

GLuint RatioLocation2; //Uniform locations for SPID2
GLuint AlphaLocation2;
GLuint ProjViewMatrixLocation2;


GLuint VSID3; //Shaders are global to the whole program
GLuint FSID3;
GLuint SPID3;

GLuint ColorLocation3; //Uniform locations for SPID3

GLuint VSID4; //Shaders are global to the whole program
GLuint FSID4;
GLuint SPID4;

GLuint SamplerLocation4; //Uniform locations for SPID4
GLuint ProjMatrixLocation4;


GLuint SkyBoxVAO; //The sky "box" model is global so put here
GLuint SkyBoxVBO;
GLuint SkyBoxEBO;
unsigned short SkyBoxCount = (unsigned short)sizeof(SkyBoxInd);//sizeof(SkyboxInd)

GLuint CurtainVAO; //white background
GLuint CurtainVBO;
GLuint CurtainEBO;
unsigned short CurtainCount = (unsigned short)sizeof(CurtainInd); //sizeof(curtainInd)


float DeltaTime = 0.0; //almost all modules will use this




float ViewMatrix[4][4] = {{1.0,0.0,0.0,0.0},//row major order
                          {0.0,1.0,0.0,0.0},
                          {0.0,0.0,1.0,0.0},
                          {0.0,0.0,0.0,1.0}};



float ProjMatrix[4][4] = {{0.0,0.0,0.0,0.0}, //row major order;
                          {0.0,1.0,0.0,0.0},
                          {0.0,0.0,1.0,-0.02f}, //-0.02 being double negative the near Z clipping distance(0.01)
                          {0.0,0.0,1.0,0.0}};


void AnimateCutscene(void);
void InitCutscene(void);

HWND MyWindowHandle;

void OpenOriginalGame(void){ShellExecuteW(NULL,NULL,L"https://scratch.mit.edu/projects/15945630/",NULL,NULL,SW_NORMAL);ExitProcess(0);Quit = 1;}


PFNGLCREATESHADERPROC glCreateShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLSHADERSOURCEPROC glShaderSource ;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLVALIDATEPROGRAMPROC glValidateProgram;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
PFNGLUNIFORM4FPROC glUniform4f;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDELETEPROGRAMPROC glDeleteProgram;

GLAPI void APIENTRY glClear (GLbitfield mask);
GLAPI void APIENTRY glClearColor (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GLAPI void APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const void *indices);
GLAPI void APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI void APIENTRY glEnable (GLenum cap);
GLAPI void APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor);
GLAPI void APIENTRY glDisable( GLenum cap );

float MouseX,MouseY;
int MouseDown;



DWORD WINAPI ActualThread(LPVOID Params) {
    HDC MyWindowContext = **(HDC**)Params;      //we are clearly assuming all pointers have the same size
    HGLRC MyGLContext = **(((HGLRC**)Params) + 1);
    HWND MyWindowHandle = **(((HWND**)Params) + 2);
    HANDLE MiscEvent = **(((HANDLE**)Params) + 3); //named as such for its use for multiple purposes

    wglMakeCurrent(MyWindowContext, MyGLContext);

    glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");    //loading GL functions related
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram"); //to Shader Creation
    glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
    glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
    glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
    glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)wglGetProcAddress("glValidateProgram");
    glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
    glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
    glUniform4f = (PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f");
    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
    glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
    glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
    glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");

    if(!wglSwapIntervalEXT(-1))MessageBoxA(NULL,"Couldn't disable Vsync","Vsync Problem",MB_OK);

    {
        VSID = glCreateShader(GL_VERTEX_SHADER);
        FSID = glCreateShader(GL_FRAGMENT_SHADER);


        glShaderSource(VSID, 1, &VShaderSrc1, NULL);
        glShaderSource(FSID, 1, &FShaderSrc1, NULL);

        glCompileShader(VSID);
        glCompileShader(FSID);

        GLint ShaderChecker;
        char Abort = 0;

        glGetShaderiv(VSID, GL_COMPILE_STATUS, &ShaderChecker);
        if (!ShaderChecker) {
            Abort = 1;
            char Errormsgs[1000];
            glGetShaderInfoLog(VSID, 1000, NULL, Errormsgs);
            MessageBoxA(NULL, Errormsgs, "Vertex Shader Error", MB_OK);
            wglMakeCurrent(MyWindowContext, NULL);
        }

        glGetShaderiv(FSID, GL_COMPILE_STATUS, &ShaderChecker);
        if (!ShaderChecker) {
            Abort = 1;
            char Errormsgs[1000];
            glGetShaderInfoLog(FSID, 1000, NULL, Errormsgs);
            MessageBoxA(NULL, Errormsgs, "Fragment Shader Error", MB_OK);
            wglMakeCurrent(MyWindowContext, NULL);
        }

        if (Abort) {
            wglMakeCurrent(MyWindowContext, NULL);
            wglDeleteContext(MyGLContext);
            ExitProcess(-1);
        }

        SPID = glCreateProgram();

        glAttachShader(SPID, VSID);
        glAttachShader(SPID, FSID);

        glBindAttribLocation(SPID, 0, "position");
        glBindAttribLocation(SPID, 1, "I_Color");

        glLinkProgram(SPID);

        glValidateProgram(SPID);

        glGetProgramiv(SPID, GL_LINK_STATUS, &ShaderChecker);

        if (!ShaderChecker) {
            MessageBoxA(NULL, "There is an error in linking", "Linking Error", MB_OK);
            wglMakeCurrent(MyWindowContext, NULL);
            wglDeleteContext(MyGLContext);
            ExitProcess(-1);
        }

        VSID2 = glCreateShader(GL_VERTEX_SHADER);
        FSID2 = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(VSID2,1,&VShaderSrc2,NULL);
        glShaderSource(FSID2,1,&FShaderSrc2,NULL);

        glCompileShader(VSID2);
        glCompileShader(FSID2);

        SPID2 = glCreateProgram();

        glAttachShader(SPID2,VSID2);
        glAttachShader(SPID2,FSID2);

        glBindAttribLocation(SPID2,0,"position");
        glBindAttribLocation(SPID2,1,"I_Color");

        glLinkProgram(SPID2);
        glValidateProgram(SPID2);

        glGetShaderiv(VSID2, GL_COMPILE_STATUS, &ShaderChecker);
        if (!ShaderChecker) {
            Abort = 1;
            char Errormsgs[1000];
            glGetShaderInfoLog(VSID2, 1000, NULL, Errormsgs);
            MessageBoxA(NULL, Errormsgs, "Vertex Shader Error", MB_OK);
            wglMakeCurrent(MyWindowContext, NULL);
        }

        glGetShaderiv(FSID2, GL_COMPILE_STATUS, &ShaderChecker);
        if (!ShaderChecker) {
            Abort = 1;
            char Errormsgs[1000];
            glGetShaderInfoLog(FSID2, 1000, NULL, Errormsgs);
            MessageBoxA(NULL, Errormsgs, "Fragment Shader Error", MB_OK);
            wglMakeCurrent(MyWindowContext, NULL);
        }

        if (Abort) {
            wglMakeCurrent(MyWindowContext, NULL);
            wglDeleteContext(MyGLContext);
            ExitProcess(-1);
        }

        glGetProgramiv(SPID2, GL_LINK_STATUS, &ShaderChecker);

        if (!ShaderChecker) {
            MessageBoxA(NULL, "There is an error in linking", "Linking Error", MB_OK);
            wglMakeCurrent(MyWindowContext, NULL);
            wglDeleteContext(MyGLContext);
            ExitProcess(-1);
        }

        glGetProgramiv(SPID2, GL_VALIDATE_STATUS, &ShaderChecker);

        if (!ShaderChecker) {
            MessageBoxA(NULL, "Program not valid", "Validation Error", MB_OK);
            wglMakeCurrent(MyWindowContext, NULL);
            wglDeleteContext(MyGLContext);
            ExitProcess(-1);
        }

        VSID3 = glCreateShader(GL_VERTEX_SHADER);
        FSID3 = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(VSID3,1,&VShaderSrc3,NULL);
        glShaderSource(FSID3,1,&FShaderSrc3,NULL);

        glCompileShader(VSID3);
        glCompileShader(FSID3);

        SPID3 = glCreateProgram();

        glAttachShader(SPID3,VSID3);
        glAttachShader(SPID3,FSID3);

        glBindAttribLocation(SPID3,0,"position");

        glLinkProgram(SPID3);

        glGetShaderiv(VSID3, GL_COMPILE_STATUS, &ShaderChecker);
        if (!ShaderChecker) {
            Abort = 1;
            char Errormsgs[1000];
            glGetShaderInfoLog(VSID3, 1000, NULL, Errormsgs);
            MessageBoxA(NULL, Errormsgs, "Vertex Shader Error", MB_OK);
            wglMakeCurrent(MyWindowContext, NULL);
        }

        glGetShaderiv(FSID3, GL_COMPILE_STATUS, &ShaderChecker);
        if (!ShaderChecker) {
            Abort = 1;
            char Errormsgs[1000];
            glGetShaderInfoLog(FSID3, 1000, NULL, Errormsgs);
            MessageBoxA(NULL, Errormsgs, "Fragment Shader Error", MB_OK);
            wglMakeCurrent(MyWindowContext, NULL);
        }

        if (Abort) {
            wglMakeCurrent(MyWindowContext, NULL);
            wglDeleteContext(MyGLContext);
            ExitProcess(-1);
        }

        glGetProgramiv(SPID3, GL_LINK_STATUS, &ShaderChecker);

        if (!ShaderChecker) {
            MessageBoxA(NULL, "There is an error in linking", "Linking Error", MB_OK);
            wglMakeCurrent(MyWindowContext, NULL);
            wglDeleteContext(MyGLContext);
            ExitProcess(-1);
        }

        SPID4 = glCreateProgram();

        VSID4 = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(VSID4,1,&VShaderSrc4,NULL);
        glCompileShader(VSID4);

        FSID4 = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(FSID4,1,&FShaderSrc4,NULL);
        glCompileShader(FSID4);

        glAttachShader(SPID4,VSID4);
        glAttachShader(SPID4,FSID4);

        glBindAttribLocation(SPID4,0,"position");
        glBindAttribLocation(SPID4,1,"TexCoord");

        glLinkProgram(SPID4);

        SamplerLocation4 = glGetUniformLocation(SPID4,"MySampler");
        ProjMatrixLocation4 = glGetUniformLocation(SPID4,"ProjMatrix");

        glUseProgram(SPID4);
        glUniform1i(SamplerLocation4,0); //set only once

        glGetShaderiv(VSID4, GL_COMPILE_STATUS, &ShaderChecker);
        if (!ShaderChecker) {
            Abort = 1;
            char Errormsgs[1000];
            glGetShaderInfoLog(VSID4, 1000, NULL, Errormsgs);
            MessageBoxA(NULL, Errormsgs, "Vertex Shader Error", MB_OK);
            wglMakeCurrent(MyWindowContext, NULL);
        }

        glGetShaderiv(FSID4, GL_COMPILE_STATUS, &ShaderChecker);
        if (!ShaderChecker) {
            Abort = 1;
            char Errormsgs[1000];
            glGetShaderInfoLog(FSID4, 1000, NULL, Errormsgs);
            MessageBoxA(NULL, Errormsgs, "Fragment Shader Error", MB_OK);
            wglMakeCurrent(MyWindowContext, NULL);
        }

        if (Abort) {
            wglMakeCurrent(MyWindowContext, NULL);
            wglDeleteContext(MyGLContext);
            ExitProcess(-1);
        }

        glGetProgramiv(SPID4, GL_LINK_STATUS, &ShaderChecker);

        if (!ShaderChecker) {
            MessageBoxA(NULL, "There is an error in linking", "Linking Error", MB_OK);
            wglMakeCurrent(MyWindowContext, NULL);
            wglDeleteContext(MyGLContext);
            ExitProcess(-1);
        }
    }

    ColorLocation3 = glGetUniformLocation(SPID3,"I_Color");

    glUseProgram(SPID);

    //                -------End of shader Creation-----


    //                -------Begin of Mesh Creation(Fill perameters appropriately)---

    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays"); //loading GL functions related
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray"); //to mesh creation/destruction
    glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
    glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");



    glGenVertexArrays(1,&SkyBoxVAO); //loading Data for the sky"box"
    glBindVertexArray(SkyBoxVAO);

    glGenBuffers(1,&SkyBoxVBO);
    glBindBuffer(GL_ARRAY_BUFFER,SkyBoxVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(SkyBoxMesh),SkyBoxMesh,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat)*6,0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat)*6,(void*)(sizeof(GLfloat) * 3));

    glGenBuffers(1,&SkyBoxEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,SkyBoxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(SkyBoxInd),SkyBoxInd,GL_STATIC_DRAW);

    glGenVertexArrays(1,&CurtainVAO);
    glBindVertexArray(CurtainVAO);

    glGenBuffers(1,&CurtainVBO);
    glBindBuffer(GL_ARRAY_BUFFER,CurtainVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(CurtainMesh),CurtainMesh,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(GLfloat),0);

    glGenBuffers(1,&CurtainEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,CurtainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(CurtainInd),CurtainInd,GL_STATIC_DRAW);

    //                -------End of Mesh Creation


    //glEnable(GL_CULL_FACE);

    RECT PreviousArea;


    RECT CurrentArea = { 0,0,2,0 };


    RatioLocation = glGetUniformLocation(SPID, "ratio"); //get uniform locations
    AlphaLocation = glGetUniformLocation(SPID,"alpha");
    ProjViewMatrixLocation = glGetUniformLocation(SPID,"ProjViewMat");

    RatioLocation2 = glGetUniformLocation(SPID2, "ratio"); //get uniform locations
    AlphaLocation2 = glGetUniformLocation(SPID2,"alpha");
    ProjViewMatrixLocation2 = glGetUniformLocation(SPID2,"ProjViewMat");

    glUniform1f(AlphaLocation,1.0);

    glUseProgram(SPID2);
    glUniform1f(AlphaLocation2,1.0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);

    InitCutscene();

    char FrameNum = 0;

    while (!Quit)
    {
        PreviousArea = CurrentArea;

        LARGE_INTEGER timer;
        LARGE_INTEGER comparasion;
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&timer);

        GetClientRect(MyWindowHandle,&CurrentArea);

        if ((CurrentArea.right - CurrentArea.left != PreviousArea.right - PreviousArea.left) || (CurrentArea.bottom - CurrentArea.top != PreviousArea.bottom - PreviousArea.top)) {
            glViewport(0, 0, CurrentArea.right - CurrentArea.left, CurrentArea.bottom - CurrentArea.top);
            ProjMatrix[0][0] = ((float)(CurrentArea.bottom - CurrentArea.top) / (float)(CurrentArea.right - CurrentArea.left) )/(float)tan((FOV*M_PI/180)/2);
            ProjMatrix[1][1] = 1/(float)tan((FOV*M_PI/180)/2);
            glUseProgram(SPID4);
            glUniformMatrix4fv(ProjMatrixLocation4,1,GL_TRUE,&(ProjMatrix[0][0]));
        }


        AnimateCutscene();

        SwapBuffers(MyWindowContext);

        if(FrameNum == 0){ //We dont want to show the first and second frame
            FrameNum = 1;
        }else if(FrameNum == 1){
            FrameNum = 2;
            SetEvent(MiscEvent); //show the window
        }

        QueryPerformanceCounter(&comparasion);
        while ((comparasion.QuadPart - timer.QuadPart) * FrameCap < frequency.QuadPart) {
            QueryPerformanceCounter(&comparasion); //spinning
        }
        DeltaTime = (float)(comparasion.QuadPart - timer.QuadPart)/(float)frequency.QuadPart;
    }

    //Deleting Bufferes and arrays
    glDeleteVertexArrays(1,&SkyBoxVAO);
    glDeleteBuffers(1,&SkyBoxVBO);
    glDeleteBuffers(1,&SkyBoxEBO);

    glDeleteVertexArrays(1,&CurtainVAO);
    glDeleteBuffers(1,&CurtainVBO);
    glDeleteBuffers(1,&CurtainEBO);

    //Deleting Shaders

    glDeleteProgram(SPID);
    glDeleteShader(VSID);
    glDeleteShader(FSID);

    glDeleteProgram(SPID2);
    glDeleteShader(VSID2);
    glDeleteShader(FSID2);

    glDeleteProgram(SPID3);
    glDeleteShader(VSID3);
    glDeleteShader(FSID3);

    glDeleteProgram(SPID4);
    glDeleteShader(VSID4);
    glDeleteShader(FSID4);


    wglMakeCurrent(MyWindowContext, NULL);
    wglDeleteContext(MyGLContext);
    //Deleting Shaders

    ExitProcess(0);
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE uUseless, LPSTR pCmdLine, int nCmdShow)
{
    hInst = (HINSTANCE)GetModuleHandle(NULL);

//   SetProcessDPIAware();

    WNDCLASSW MyClass = { 0 };

    MyClass.hInstance = hInst;
    MyClass.lpszClassName = L"TripleA";
    MyClass.lpfnWndProc = WindowProcedure;
    MyClass.style = CS_OWNDC;
    MyClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    MyClass.hIcon = LoadImageW(hInst, L"MyIcon", IMAGE_ICON, 0, 0, 0);
    if (!MyClass.hIcon) {
        MessageBoxA(NULL, "Couldn't find the icon", "Icon Missing", MB_OK);
        ExitProcess((UINT)hInst);
    }

    RegisterClassW(&MyClass);

    RECT WD = {40,50,40+800,50+450}; //Window Dimensions

    AdjustWindowRect(&WD,WS_OVERLAPPEDWINDOW,FALSE);

    MyWindowHandle = CreateWindowExW(0,
        L"TripleA",
        L"Hello!",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInst,
        NULL);


    HDC MyWindowContext = GetDC(MyWindowHandle);

    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;



    HGLRC MyGLContext;
    //            -------Context Creation-----
    {
        HWND TempWindowHandle = CreateWindowExW(0,
            L"TripleA",
            L"The Real Hello!!",
            WS_OVERLAPPED | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MAXIMIZE,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            NULL,
            NULL,
            hInst,
            NULL);


        HDC TempWindowContext = GetDC(TempWindowHandle);


        PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
            PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
            32,                   // Colordepth of the framebuffer.
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,                   // Number of bits for the depthbuffer
            8,                    // Number of bits for the stencilbuffer
            0,                    // Number of Aux buffers in the framebuffer.
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };

        int TempPixelFormat = ChoosePixelFormat(TempWindowContext, &pfd);

        SetPixelFormat(TempWindowContext, TempPixelFormat, &pfd);

        HGLRC TempGLContext = wglCreateContext(TempWindowContext);

        wglMakeCurrent(TempWindowContext, TempGLContext);


        wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
        wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
        wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

        if (!(wglChoosePixelFormatARB && wglCreateContextAttribsARB))
        {
            return 1234;
        }

        int MyPixelFormat;

        {
            const int IntAttribs[] = {
                                    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                                    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                                    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                                    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                                    WGL_COLOR_BITS_ARB, 32,
                                    WGL_DEPTH_BITS_ARB, 24,
                                    WGL_STENCIL_BITS_ARB, 8,
                                    0 };
            unsigned int NumFormats;



            wglChoosePixelFormatARB(MyWindowContext, IntAttribs, NULL, 1, &MyPixelFormat, &NumFormats);
        }

        SetPixelFormat(MyWindowContext, MyPixelFormat, &pfd);


        {
            const int IntAttribs[] = { WGL_CONTEXT_MAJOR_VERSION_ARB,3,
                                      WGL_CONTEXT_MINOR_VERSION_ARB,0,
                                      0 };

            MyGLContext = wglCreateContextAttribsARB(MyWindowContext, NULL, IntAttribs);
            if(!MyGLContext){
                MessageBoxA(NULL,"Opengl 3.0 not supported","Gl insufficient",MB_OK);
                ExitProcess(100);
            }
        }

        wglMakeCurrent(TempWindowContext, NULL);
        wglDeleteContext(TempGLContext);
        DestroyWindow(TempWindowHandle);
    }

    HANDLE MiscEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

    //            -------End of Context Creation-----

    void* Params[] = {&MyWindowContext,&MyGLContext,&MyWindowHandle,&MiscEvent};

    DWORD MainThreadID;

    CreateThread(NULL, 0, &ActualThread, Params, 0, &MainThreadID);

    WaitForSingleObject(MiscEvent,INFINITE);

    ShowWindow(MyWindowHandle, SW_MAXIMIZE);

    MSG MessageData;

    while (GetMessageW(&MessageData, NULL, 0, 0))
    {
        //if(MessageData.message == WM_QUIT)ShouldClose = TRUE;
        TranslateMessage(&MessageData);
        DispatchMessage(&MessageData);
    }

    while(1){};//keep spinning until the other thread closes the process

    return 0;
}

LRESULT CALLBACK WindowProcedure(HWND WindowHandle, UINT Message, WPARAM wparam, LPARAM lparam)
{
    //    printf("%i\n",Message);

    //    switch(Message)
    //    {
    //    case WM_PAINT:
    //    printf("WM_PAINT\n");
    //    case WM_NCPAINT:
    //    printf("WM_NCPAINT\n");
    //    case WM_ERASEBKGND:
    //    printf("WM_ERASEBKGND\n");
    //    case BM_CLICK:
    //    printf("MSG: %i",Message);
    //    case WM_QUIT:
    //    printf("WM_QUIT\n");
    //    }

    switch (Message)
    {
        case WM_GETMINMAXINFO:{
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lparam;
            lpMMI->ptMinTrackSize.x = 800;
            lpMMI->ptMinTrackSize.y = 450;
            break;
        }
        case WM_MOUSEMOVE:{
            RECT ClientArea;
            GetClientRect(WindowHandle,&ClientArea);
            MouseX = ((float)GET_X_LPARAM(lparam)/(float)ClientArea.right)*2.0f - 1.0f; //From Pixel to normalised Device Coordinates
            MouseX *= (float)ClientArea.right/(float)ClientArea.bottom;
            MouseY = ((float)GET_Y_LPARAM(lparam)/(float)ClientArea.bottom)*-2.0f + 1.0f; //Must take the inverted Y axis into account
            break;
        }
        case WM_LBUTTONDOWN:{
            SetCapture(WindowHandle);
            MouseDown = 1;
            break;
        }
        case WM_LBUTTONUP:{
            ReleaseCapture();
            MouseDown = 0;
            break;
        }
        case WM_CLOSE:{
            Quit = 1;
            break;
        }
    }
    return DefWindowProcW(WindowHandle, Message, wparam, lparam);
}
