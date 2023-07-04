#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#define GVM_POSITION 1
#define GVM_ANGLES_ROTATION 2
#define GVM_QUATERNION_ROTATION 4
#define GVM_IDENTITY 8//Initialiase The given matrix to identity

typedef struct{
    float Xpos;
    float Ypos;
    float Zpos;

    union{
        struct{
            float RotX;
            float RotY;
            float RotZ;
        }Angles3;

        struct{
            float Xcomp;
            float Ycomp;
            float Zcomp;
            float wcomp;
        }Quaternion;
    }Rotation;
}CAMERADATA;

void GenViewMatrix(CAMERADATA *Camera,float (*ReturnMatrix)[4], int Flags);

void Mul4x4Matrix(float (*FirstMatrix)[4],float (*SecondMatrix)[4],float (*ResultMatrix)[4]);


#endif // CAMERA_H_INCLUDED
