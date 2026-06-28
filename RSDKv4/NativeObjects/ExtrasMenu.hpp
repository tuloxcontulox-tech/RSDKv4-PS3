#ifndef NATIVE_EXTRASMENU_H
#define NATIVE_EXTRASMENU_H

enum ExtrasMenuButtons {
    EXTRASMENU_BUTTON_DAGARDEN,
    EXTRASMENU_BUTTON_SOUNDTEST,
    EXTRASMENU_BUTTON_STAGESELECT,
    EXTRASMENU_BUTTON_COUNT,
};

enum ExtrasMenuStates {
    EXTRASMENU_STATE_SETUP,
    EXTRASMENU_STATE_ENTER,
    EXTRASMENU_STATE_MAIN,
    EXTRASMENU_STATE_EXIT,
    EXTRASMENU_STATE_ACTION,
};

struct NativeEntity_ExtrasMenu : NativeEntityBase {
    ExtrasMenuStates state;
    float timer;
    NativeEntity_MenuControl *menuControl;
    NativeEntity_TextLabel *labelPtr;
    NativeEntity_SubMenuButton *buttons[EXTRASMENU_BUTTON_COUNT];
    int selectedButton;
    float rotationY;
    float buttonRotationY;
    float rotationYVel;
    float buttonRotationYVelocity;
    float targetRotationY;
    float targetButtonRotationY;
    MatrixF matrixTemp;
};

void ExtrasMenu_Create(void *objPtr);
void ExtrasMenu_Main(void *objPtr);

#endif // !NATIVE_EXTRASMENU_H
