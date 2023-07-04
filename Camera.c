#include "Camera.h"
#include<math.h>

static const float IdentityMatrix[4][4] = {{1.0f,0.0f,0.0f,0.0f},
    {0.0f,1.0f,0.0f,0.0f},
    {0.0f,0.0f,1.0f,0.0f},
    {0.0f,0.0f,0.0f,1.0f}
};

void GenViewMatrix(CAMERADATA *Camera,float (*ReturnMatrix)[4], int Flags)  //this function dictates that the matrix must be a proper 2D array
{
    //Note: Order of these ifs matter, init, translate then rotate
    if(Flags&GVM_IDENTITY)
    {
        for(int i = 0; i < sizeof(IdentityMatrix); i++)
        {
            ((char*)(&ReturnMatrix[0][0]))[i] = ((char*)(&IdentityMatrix[0][0]))[i];
        }
    }

    if(Flags&GVM_POSITION)
    {
        ReturnMatrix[0][3] = -Camera->Xpos;
        ReturnMatrix[1][3] = -Camera->Ypos;
        ReturnMatrix[2][3] = -Camera->Zpos;
    }
    if(Flags&GVM_ANGLES_ROTATION)
    {
        float a = Camera->Rotation.Angles3.RotX;
        float b = -Camera->Rotation.Angles3.RotY;
        float TempMatrix[4][4];
        for(int i = 0; i < sizeof(TempMatrix)/sizeof(float); i++)
        {
            (&TempMatrix[0][0])[i] = (&ReturnMatrix[0][0])[i];
        }

        ReturnMatrix[0][0] = (float)(cos(b)*TempMatrix[0][0] -sin(b)*TempMatrix[2][0]);
        ReturnMatrix[0][1] = (float)(cos(b)*TempMatrix[0][1] -sin(b)*TempMatrix[2][1]);
        ReturnMatrix[0][2] = (float)(cos(b)*TempMatrix[0][2] -sin(b)*TempMatrix[2][2]);
        ReturnMatrix[0][3] = (float)(cos(b)*TempMatrix[0][3] -sin(b)*TempMatrix[2][3]);
        ReturnMatrix[1][0] = (float)(-sin(a)*sin(b)*TempMatrix[0][0] + cos(a)*TempMatrix[1][0] -sin(a)*cos(b)*TempMatrix[2][0]);
        ReturnMatrix[1][1] = (float)(-sin(a)*sin(b)*TempMatrix[0][1] + cos(a)*TempMatrix[1][1] -sin(a)*cos(b)*TempMatrix[2][1]);
        ReturnMatrix[1][2] = (float)(-sin(a)*sin(b)*TempMatrix[0][2] + cos(a)*TempMatrix[1][2] -sin(a)*cos(b)*TempMatrix[2][2]);
        ReturnMatrix[1][3] = (float)(-sin(a)*sin(b)*TempMatrix[0][3] + cos(a)*TempMatrix[1][3] -sin(a)*cos(b)*TempMatrix[2][3]);
        ReturnMatrix[2][0] = (float)(cos(a)*sin(b)*TempMatrix[0][0] + sin(a) * TempMatrix[1][0] + cos(a)*cos(b)*TempMatrix[2][0]);
        ReturnMatrix[2][1] = (float)(cos(a)*sin(b)*TempMatrix[0][1] + sin(a) * TempMatrix[1][1] + cos(a)*cos(b)*TempMatrix[2][1]);
        ReturnMatrix[2][2] = (float)(cos(a)*sin(b)*TempMatrix[0][2] + sin(a) * TempMatrix[1][2] + cos(a)*cos(b)*TempMatrix[2][2]);
        ReturnMatrix[2][3] = (float)(cos(a)*sin(b)*TempMatrix[0][3] + sin(a) * TempMatrix[1][3] + cos(a)*cos(b)*TempMatrix[2][3]);
        ReturnMatrix[3][0] = (float)(TempMatrix[3][0]);
        ReturnMatrix[3][1] = (float)(TempMatrix[3][1]);
        ReturnMatrix[3][2] = (float)(TempMatrix[3][2]);
        ReturnMatrix[3][3] = (float)(TempMatrix[3][3]);

    }

    //for now only implement the "position" aspect
    return;
}


 void Mul4x4Matrix(float (*FirstMatrix)[4],float (*SecondMatrix)[4],float (*ResultMatrix)[4]){ //Mutiply first with second and store in first

    ResultMatrix[0][0] = FirstMatrix[0][0] * SecondMatrix[0][0] + FirstMatrix[0][1] * SecondMatrix[1][0] + FirstMatrix[0][2] * SecondMatrix[2][0] + FirstMatrix[0][3] * SecondMatrix[3][0];
    ResultMatrix[0][1] = FirstMatrix[0][0] * SecondMatrix[0][1] + FirstMatrix[0][1] * SecondMatrix[1][1] + FirstMatrix[0][2] * SecondMatrix[2][1] + FirstMatrix[0][3] * SecondMatrix[3][1];
    ResultMatrix[0][2] = FirstMatrix[0][0] * SecondMatrix[0][2] + FirstMatrix[0][1] * SecondMatrix[1][2] + FirstMatrix[0][2] * SecondMatrix[2][2] + FirstMatrix[0][3] * SecondMatrix[3][0];
    ResultMatrix[0][3] = FirstMatrix[0][0] * SecondMatrix[0][3] + FirstMatrix[0][1] * SecondMatrix[1][3] + FirstMatrix[0][2] * SecondMatrix[2][3] + FirstMatrix[0][3] * SecondMatrix[3][0];

    ResultMatrix[1][0] = FirstMatrix[1][0] * SecondMatrix[0][0] + FirstMatrix[1][1] * SecondMatrix[1][0] + FirstMatrix[1][2] * SecondMatrix[2][0] + FirstMatrix[1][3] * SecondMatrix[3][0];
    ResultMatrix[1][1] = FirstMatrix[1][0] * SecondMatrix[0][1] + FirstMatrix[1][1] * SecondMatrix[1][1] + FirstMatrix[1][2] * SecondMatrix[2][1] + FirstMatrix[1][3] * SecondMatrix[3][1];
    ResultMatrix[1][2] = FirstMatrix[1][0] * SecondMatrix[0][2] + FirstMatrix[1][1] * SecondMatrix[1][2] + FirstMatrix[1][2] * SecondMatrix[2][2] + FirstMatrix[1][3] * SecondMatrix[3][0];
    ResultMatrix[1][3] = FirstMatrix[1][0] * SecondMatrix[0][3] + FirstMatrix[1][1] * SecondMatrix[1][3] + FirstMatrix[1][2] * SecondMatrix[2][3] + FirstMatrix[1][3] * SecondMatrix[3][0];

    ResultMatrix[2][0] = FirstMatrix[2][0] * SecondMatrix[0][0] + FirstMatrix[2][1] * SecondMatrix[1][0] + FirstMatrix[2][2] * SecondMatrix[2][0] + FirstMatrix[2][3] * SecondMatrix[3][0];
    ResultMatrix[2][1] = FirstMatrix[2][0] * SecondMatrix[0][1] + FirstMatrix[2][1] * SecondMatrix[1][1] + FirstMatrix[2][2] * SecondMatrix[2][1] + FirstMatrix[2][3] * SecondMatrix[3][1];
    ResultMatrix[2][2] = FirstMatrix[2][0] * SecondMatrix[0][2] + FirstMatrix[2][1] * SecondMatrix[1][2] + FirstMatrix[2][2] * SecondMatrix[2][2] + FirstMatrix[2][3] * SecondMatrix[3][0];
    ResultMatrix[2][3] = FirstMatrix[2][0] * SecondMatrix[0][3] + FirstMatrix[2][1] * SecondMatrix[1][3] + FirstMatrix[2][2] * SecondMatrix[2][3] + FirstMatrix[2][3] * SecondMatrix[3][0];

    ResultMatrix[3][0] = FirstMatrix[3][0] * SecondMatrix[0][0] + FirstMatrix[3][1] * SecondMatrix[1][0] + FirstMatrix[3][2] * SecondMatrix[2][0] + FirstMatrix[3][3] * SecondMatrix[3][0];
    ResultMatrix[3][1] = FirstMatrix[3][0] * SecondMatrix[0][1] + FirstMatrix[3][1] * SecondMatrix[1][1] + FirstMatrix[3][2] * SecondMatrix[2][1] + FirstMatrix[3][3] * SecondMatrix[3][1];
    ResultMatrix[3][2] = FirstMatrix[3][0] * SecondMatrix[0][2] + FirstMatrix[3][1] * SecondMatrix[1][2] + FirstMatrix[3][2] * SecondMatrix[2][2] + FirstMatrix[3][3] * SecondMatrix[3][0];
    ResultMatrix[3][3] = FirstMatrix[3][0] * SecondMatrix[0][3] + FirstMatrix[3][1] * SecondMatrix[1][3] + FirstMatrix[3][2] * SecondMatrix[2][3] + FirstMatrix[3][3] * SecondMatrix[3][0];
}
