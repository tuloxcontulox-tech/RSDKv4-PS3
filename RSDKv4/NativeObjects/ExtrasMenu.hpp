#ifndef NATIVE_EXTRASMENU_H
#define NATIVE_EXTRASMENU_H

enum ExtrasMenuStates { EXTRASMENU_STATE_ENTER, EXTRASMENU_STATE_MAIN, EXTRASMENU_STATE_EXIT };

struct NativeEntity_ExtrasMenu : NativeEntityBase {
    ExtrasMenuStates state;
    float timer;
    float scale;
    int arrowAlpha;
    int selectedButton;
    bool backPressed;
    NativeEntity_TextLabel *label;
    NativeEntity_SubMenuButton *buttons[3];
    MeshInfo *meshPanel;
    int textureArrows;
    MatrixF renderMatrix;
    MatrixF matrixTemp;
};

void ExtrasMenu_Create(void *objPtr);
void ExtrasMenu_Main(void *objPtr);

#endif // !NATIVE_EXTRASMENU_H
