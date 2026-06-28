#include "RetroEngine.hpp"
#include "String.hpp"

void ExtrasMenu_Create(void *objPtr)
{
    RSDK_THIS(ExtrasMenu);
    self->menuControl = (NativeEntity_MenuControl *)GetNativeObject(0);

    self->labelPtr         = CREATE_ENTITY(TextLabel);
    self->labelPtr->fontID = FONT_HEADING;
    self->labelPtr->scale  = 0.2;
    self->labelPtr->alpha  = 0;
    self->labelPtr->z      = 0;
    self->labelPtr->state  = TEXTLABEL_STATE_IDLE;
    if (strExtras)
        SetStringToFont(self->labelPtr->text, strExtras, FONT_HEADING);
    else
        SetStringToFont8(self->labelPtr->text, "EXTRAS", FONT_HEADING);
    self->labelPtr->alignOffset = 512.0;

    self->rotationY = DegreesToRad(22.5);
    MatrixRotateYF(&self->labelPtr->renderMatrix, self->rotationY);
    MatrixTranslateXYZF(&self->matrixTemp, -128.0, 80.0, 160.0);
    MatrixMultiplyF(&self->labelPtr->renderMatrix, &self->matrixTemp);
    self->labelPtr->useRenderMatrix = true;

    float y = 48.0;
    for (int i = 0; i < EXTRASMENU_BUTTON_COUNT; ++i) {
        self->buttons[i]          = CREATE_ENTITY(SubMenuButton);
        self->buttons[i]->matXOff = 512.0;
        self->buttons[i]->textY   = -4.0;
        self->buttons[i]->matZ    = 0.0;
        self->buttons[i]->scale   = 0.1;

        self->buttonRotationY = DegreesToRad(16.0);
        MatrixRotateYF(&self->buttons[i]->matrix, self->buttonRotationY);
        MatrixTranslateXYZF(&self->matrixTemp, -128.0, y, 160.0);
        MatrixMultiplyF(&self->buttons[i]->matrix, &self->matrixTemp);
        self->buttons[i]->useMatrix = true;
        y -= 30.0;
    }

    if (strDAGarden)
        SetStringToFont(self->buttons[EXTRASMENU_BUTTON_DAGARDEN]->text, strDAGarden, FONT_LABEL);
    else
        SetStringToFont8(self->buttons[EXTRASMENU_BUTTON_DAGARDEN]->text, "D.A. GARDEN", FONT_LABEL);

    if (strSoundTest)
        SetStringToFont(self->buttons[EXTRASMENU_BUTTON_SOUNDTEST]->text, strSoundTest, FONT_LABEL);
    else
        SetStringToFont8(self->buttons[EXTRASMENU_BUTTON_SOUNDTEST]->text, "SOUND TEST", FONT_LABEL);

    if (strStageSelect)
        SetStringToFont(self->buttons[EXTRASMENU_BUTTON_STAGESELECT]->text, strStageSelect, FONT_LABEL);
    else
        SetStringToFont8(self->buttons[EXTRASMENU_BUTTON_STAGESELECT]->text, "STAGE SELECT", FONT_LABEL);

    self->state = EXTRASMENU_STATE_ENTER;
}

void ExtrasMenu_Main(void *objPtr)
{
    RSDK_THIS(ExtrasMenu);

    switch (self->state) {
        case EXTRASMENU_STATE_ENTER: {
            self->labelPtr->alignOffset /= (1.125 * (60.0 * Engine.deltaTime));
            self->timer += (float)(Engine.deltaTime * 2.0);
            self->labelPtr->alpha = (int)(self->timer * 256.0);

            float div = (float)(60.0 * Engine.deltaTime * 16.0);
            for (int i = 0; i < EXTRASMENU_BUTTON_COUNT; ++i) {
                self->buttons[i]->matXOff += ((-176.0 - self->buttons[i]->matXOff) / div);
            }

            if (self->timer > 1.0) {
                self->timer    = 0.0;
                self->state    = EXTRASMENU_STATE_MAIN;
                keyPress.start = false;
                keyPress.A     = false;
            }
            break;
        }

        case EXTRASMENU_STATE_MAIN: {
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.up) {
                        PlaySfxByName("Menu Move", false);
                        self->selectedButton--;
                        if (self->selectedButton < 0)
                            self->selectedButton = EXTRASMENU_BUTTON_COUNT - 1;
                    }
                    else if (keyPress.down) {
                        PlaySfxByName("Menu Move", false);
                        self->selectedButton++;
                        if (self->selectedButton >= EXTRASMENU_BUTTON_COUNT)
                            self->selectedButton = 0;
                    }

                    for (int i = 0; i < EXTRASMENU_BUTTON_COUNT; ++i) self->buttons[i]->b = 0xFF;
                    self->buttons[self->selectedButton]->b = 0x00;

                    if (keyPress.start || keyPress.A) {
                        PlaySfxByName("Menu Select", false);
                        self->buttons[self->selectedButton]->state = SUBMENUBUTTON_STATE_FLASHING2;
                        self->state                                = EXTRASMENU_STATE_ACTION;
                    }
                }
            }
            else {
                float y = 48.0;
                for (int i = 0; i < EXTRASMENU_BUTTON_COUNT; ++i) {
                    if (touches > 0) {
                        if (CheckTouchRect(-64.0, y, 96.0, 12.0) < 0)
                            self->buttons[i]->b = 0xFF;
                        else
                            self->buttons[i]->b = 0x00;
                    }
                    else if (!self->buttons[i]->b) {
                        self->selectedButton = i;
                        PlaySfxByName("Menu Select", false);
                        self->buttons[self->selectedButton]->state = SUBMENUBUTTON_STATE_FLASHING2;
                        self->state                                = EXTRASMENU_STATE_ACTION;
                        break;
                    }
                    y -= 30.0;
                }

                if (self->state == EXTRASMENU_STATE_MAIN && (keyDown.up || keyDown.down)) {
                    usePhysicalControls = true;
                }
            }

            if (self->menuControl->state == MENUCONTROL_STATE_EXITSUBMENU) {
                self->state = EXTRASMENU_STATE_EXIT;
            }
            break;
        }

        case EXTRASMENU_STATE_ACTION: {
            if (self->buttons[self->selectedButton]->state == SUBMENUBUTTON_STATE_IDLE) {
                SetGlobalVariableByName("options.saveSlot", 0);
                SetGlobalVariableByName("options.gameMode", 0);
                SetGlobalVariableByName("player.lives", 3);
                SetGlobalVariableByName("player.score", 0);
                SetGlobalVariableByName("player.scoreBonus", 50000);
                SetGlobalVariableByName("specialStage.emeralds", 0);
                SetGlobalVariableByName("specialStage.listPos", 0);
                SetGlobalVariableByName("specialStage.nextZone", 0);
                SetGlobalVariableByName("timeAttack.result", 0);
                SetGlobalVariableByName("lampPostID", 0);
                SetGlobalVariableByName("starPostID", 0);

                int stageID = 0;
                switch (self->selectedButton) {
                    case EXTRASMENU_BUTTON_DAGARDEN: stageID = 5; break;
                    case EXTRASMENU_BUTTON_SOUNDTEST: stageID = 4; break;
                    case EXTRASMENU_BUTTON_STAGESELECT: stageID = 3; break;
                }

                BackupNativeObjects();
                InitStartingStage(STAGELIST_PRESENTATION, stageID, 0);
                CREATE_ENTITY(FadeScreen);
                self->state = EXTRASMENU_STATE_SETUP; // just to stop processing
            }
            break;
        }

        case EXTRASMENU_STATE_EXIT: {
            self->timer += (float)(Engine.deltaTime * 2.0);
            self->labelPtr->alignOffset += (float)(10.0 * (60.0 * Engine.deltaTime));
            for (int i = 0; i < EXTRASMENU_BUTTON_COUNT; ++i) {
                self->buttons[i]->matXOff += (float)(11.0 * (60.0 * Engine.deltaTime));
            }

            if (self->timer > 1.0) {
                self->timer = 0.0;
                RemoveNativeObject(self->labelPtr);
                for (int i = 0; i < EXTRASMENU_BUTTON_COUNT; ++i) RemoveNativeObject(self->buttons[i]);
                RemoveNativeObject(self);
            }
            break;
        }
        default: break;
    }
}
