#ifndef NATIVE_EXTRASBUTTON_H
#define NATIVE_EXTRASBUTTON_H

struct NativeEntity_ExtrasButton : NativeEntity_AchievementsButton {
    // No extra fields needed, same as AchievementsButton
};

void ExtrasButton_Create(void *objPtr);
void ExtrasButton_Main(void *objPtr);

#endif // !NATIVE_EXTRASBUTTON_H
