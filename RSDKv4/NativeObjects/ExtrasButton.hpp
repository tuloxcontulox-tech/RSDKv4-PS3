#ifndef NATIVE_EXTRASBUTTON_H
#define NATIVE_EXTRASBUTTON_H

struct NativeEntity_ExtrasButton : NativeEntityBase {
    int unused1;
    byte visible;
    int unused2;
    int unused3;
    float x;
    float y;
    float z;
    MeshInfo *meshCartridge;
    float angle;
    float scale;
    byte textureCircle;
    byte r;
    byte g;
    byte b;
    MatrixF renderMatrix;
    MatrixF matrixTemp;
    NativeEntity_TextLabel *labelPtr;
};

void ExtrasButton_Create(void *objPtr);
void ExtrasButton_Main(void *objPtr);

#endif // !NATIVE_EXTRASBUTTON_H
