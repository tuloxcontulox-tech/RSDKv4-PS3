#include "RetroEngine.hpp"

void ExtrasMenu_Create(void *objPtr)
{
    RSDK_THIS(ExtrasMenu);

    self->label                  = CREATE_ENTITY(TextLabel);
    self->label->useRenderMatrix = true;
    self->label->fontID          = FONT_HEADING;
    self->label->scale           = 0.2;
    self->label->alpha           = 256;
    self->label->x               = -144.0;
    self->label->y               = 100.0;
    self->label->z               = 16.0;
    self->label->state           = TEXTLABEL_STATE_IDLE;
    if (strExtras)
        SetStringToFont(self->label->text, strExtras, FONT_HEADING);
    else
        SetStringToFont8(self->label->text, "EXTRAS", FONT_HEADING);

    self->scale      = 0;
    self->arrowAlpha = 0;

    self->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", 255);
    SetMeshVertexColors(self->meshPanel, 0, 0, 0, 0xC0);
    self->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);

    float y = 24.0f;
    const char *names[] = { "STAGE SELECT", "SOUND TEST", "D.A. GARDEN" };
    for (int i = 0; i < 3; ++i) {
        self->buttons[i]            = CREATE_ENTITY(SubMenuButton);
        self->buttons[i]->matZ      = 0.0;
        self->buttons[i]->useMatrix = true;
        self->buttons[i]->scale     = 0.1;
        self->buttons[i]->textY     = -4.0;
        if (i == 1 && strSoundTest)
             SetStringToFont(self->buttons[i]->text, strSoundTest, FONT_LABEL);
        else
             SetStringToFont8(self->buttons[i]->text, names[i], FONT_LABEL);

        y -= 30.0f;
    }
}

void ExtrasMenu_Main(void *objPtr)
{
    RSDK_THIS(ExtrasMenu);

    switch (self->state) {
        case EXTRASMENU_STATE_ENTER: {
            if (self->arrowAlpha < 0x100)
                self->arrowAlpha += 8;

            self->scale = fminf(self->scale + ((1.05 - self->scale) / ((60.0 * Engine.deltaTime) * 8.0)), 1.0f);

            NewRenderState();
            MatrixScaleXYZF(&self->renderMatrix, self->scale, self->scale, 1.0);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, 0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);

            memcpy(&self->label->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            float y = 48.0;
            for (int i = 0; i < 3; ++i) {
                MatrixRotateYF(&self->buttons[i]->matrix, DegreesToRad(16.0));
                MatrixTranslateXYZF(&self->matrixTemp, -128.0, y, 160.0);
                MatrixMultiplyF(&self->buttons[i]->matrix, &self->matrixTemp);
                MatrixMultiplyF(&self->buttons[i]->matrix, &self->renderMatrix);
                y -= 30.0;
            }

            self->timer += Engine.deltaTime;
            if (self->timer > 0.5) {
                self->arrowAlpha = 0x100;
                self->timer      = 0.0;
                self->state      = EXTRASMENU_STATE_MAIN;
            }
            break;
        }
        case EXTRASMENU_STATE_MAIN: {
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&self->renderMatrix);

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.up) {
                        PlaySfxByName("Menu Move", false);
                        if (--self->selectedButton < 0)
                            self->selectedButton = 2;
                    }
                    else if (keyPress.down) {
                        PlaySfxByName("Menu Move", false);
                        if (++self->selectedButton > 2)
                            self->selectedButton = 0;
                    }

                    for (int i = 0; i < 3; ++i) self->buttons[i]->b = 0xFF;
                    self->buttons[self->selectedButton]->b = 0x00;

                    if (keyPress.start || keyPress.A) {
                        PlaySfxByName("Menu Select", false);
                        StopMusic(true);

                        SetGlobalVariableByName("options.saveSlot", 0);
                        SetGlobalVariableByName("options.gameMode", 0);
                        SetGlobalVariableByName("options.vsMode", 0);
                        SetGlobalVariableByName("player.lives", 3);
                        SetGlobalVariableByName("player.score", 0);
                        SetGlobalVariableByName("player.scoreBonus", 50000);
                        SetGlobalVariableByName("specialStage.listPos", 0);
                        SetGlobalVariableByName("specialStage.emeralds", 0);
                        SetGlobalVariableByName("specialStage.nextZone", 0);
                        SetGlobalVariableByName("timeAttack.result", 0);
                        SetGlobalVariableByName("lampPostID", 0);
                        SetGlobalVariableByName("starPostID", 0);
                        debugMode = false;

                        int id = -1;
                        switch (self->selectedButton) {
                            case 0: // Stage Select
                                id = GetSceneID(STAGELIST_PRESENTATION, "STAGE SELECT");
                                if (id == -1) id = 5; // Fallback
                                break;
                            case 1: // Sound Test
                                id = GetSceneID(STAGELIST_PRESENTATION, "SOUND TEST");
                                if (id == -1) id = 6; // Fallback
                                break;
                            case 2: // DA Garden
                                id = GetSceneID(STAGELIST_PRESENTATION, "DA GARDEN");
                                if (id == -1) id = 4; // Fallback
                                break;
                        }

                        if (id != -1) {
                            BackupNativeObjects();
                            InitStartingStage(STAGELIST_PRESENTATION, id, 0);
                            CREATE_ENTITY(FadeScreen);
                        }
                    }

                    if (keyPress.B) {
                        PlaySfxByName("Menu Back", false);
                        self->backPressed = false;
                        self->state       = EXTRASMENU_STATE_EXIT;
                    }
                }
            }
            else {
                if (touches > 0) {
                    self->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;

                    float y = 48.0;
                    for (int i = 0; i < 3; ++i) {
                        if (CheckTouchRect(-64.0, y, 96.0, 12.0) >= 0) {
                            self->selectedButton = i;
                            for (int j = 0; j < 3; ++j) self->buttons[j]->b = 0xFF;
                            self->buttons[i]->b = 0x00;
                        }
                        y -= 30.0;
                    }
                }
                else {
                    if (self->backPressed || keyPress.B) {
                        PlaySfxByName("Menu Back", false);
                        self->backPressed = false;
                        self->state       = EXTRASMENU_STATE_EXIT;
                    }
                }
            }
            break;
        }
        case EXTRASMENU_STATE_EXIT: {
            if (self->arrowAlpha > 0)
                self->arrowAlpha -= 8;

            if (self->timer < 0.2)
                self->scale = fmaxf(self->scale + ((1.5f - self->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);
            else
                self->scale = fmaxf(self->scale + ((-1.0f - self->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);

            NewRenderState();
            MatrixScaleXYZF(&self->renderMatrix, self->scale, self->scale, 1.0);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, 0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);

            memcpy(&self->label->renderMatrix, &self->renderMatrix, sizeof(MatrixF));

            self->timer += Engine.deltaTime;
            if (self->timer > 0.5) {
                NativeEntity_MenuControl *menuControl = (NativeEntity_MenuControl *)GetNativeObject(0);
                menuControl->state = MENUCONTROL_STATE_EXITSUBMENU;
                RemoveNativeObject(self->label);
                for (int i = 0; i < 3; ++i) RemoveNativeObject(self->buttons[i]);
                RemoveNativeObject(self);
                return;
            }
            break;
        }
    }

    RenderMesh(self->meshPanel, MESH_COLORS, false);
    NewRenderState();
    SetRenderMatrix(NULL);

    if (self->backPressed)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, self->arrowAlpha, self->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, self->arrowAlpha, self->textureArrows);
}
