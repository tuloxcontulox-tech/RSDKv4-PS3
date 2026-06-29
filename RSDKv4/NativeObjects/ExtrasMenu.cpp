#include "RetroEngine.hpp"
#include "String.hpp"

void ExtrasMenu_Create(void *objPtr)
{
    RSDK_THIS(ExtrasMenu);

    self->label                  = CREATE_ENTITY(TextLabel);
    self->label->useRenderMatrix = true;
    self->label->fontID          = FONT_HEADING;
    self->label->z      = 0.0;
    self->label->scale  = 0.2;
    self->label->alpha  = 0;
    self->label->fontID = FONT_HEADING;
    self->label->state  = TEXTLABEL_STATE_IDLE;
    if (strExtras)
        SetStringToFont(self->label->text, strExtras, FONT_HEADING);
    else
        SetStringToFont8(self->label->text, "EXTRAS", FONT_HEADING);
    self->label->alignOffset = 512.0;

    MatrixRotateYF(&self->label->renderMatrix, DegreesToRad(22.5));
    MatrixTranslateXYZF(&self->matrixTemp, -128.0, 80.0, 160.0);
    MatrixMultiplyF(&self->label->renderMatrix, &self->matrixTemp);

    self->scale      = 0;
    self->arrowAlpha = 0;

    self->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", 255);
    SetMeshVertexColors(self->meshPanel, 0, 0, 0, 0xC0);
    self->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);
    self->textureCircle = LoadTexture("Data/Game/Menu/Circle.png", TEXFMT_RGBA4444);
    self->textureExtras = LoadTexture("Data/Game/Models/Extras.png", TEXFMT_RGBA4444);

    const char *names[] = { "D.A. GARDEN", "SOUND TEST", "STAGE SELECT" };
    for (int i = 0; i < 3; ++i) {
        self->buttons[i]            = CREATE_ENTITY(SubMenuButton);
        self->buttons[i]->matZ      = 0.0;
        self->buttons[i]->useMatrix = true;
        self->buttons[i]->scale     = 0.1;
        self->buttons[i]->textY     = -4.0;
        self->buttons[i]->matXOff   = 512.0;
        SetStringToFont8(self->buttons[i]->text, names[i], FONT_LABEL);
    }
}

void ExtrasMenu_Main(void *objPtr)
{
    RSDK_THIS(ExtrasMenu);

    switch (self->state) {
        case EXTRASMENU_STATE_ENTER: {
            if (self->arrowAlpha < 0x100)
                self->arrowAlpha += 8;

            self->timer += Engine.deltaTime * 2.0;
            self->scale = fminf(self->timer, 1.0f);

            self->label->alignOffset = self->label->alignOffset / (1.125 * (60.0 * Engine.deltaTime));
            self->label->alpha       = (256.0 * self->timer);

            for (int i = 0; i < 3; ++i) {
                self->buttons[i]->matXOff += ((-176.0 - self->buttons[i]->matXOff) / (16.0 * (60.0 * Engine.deltaTime)));
                MatrixRotateYF(&self->buttons[i]->matrix, DegreesToRad(16.0));
                MatrixTranslateXYZF(&self->matrixTemp, -128.0, 48.0 - i * 30.0, 160.0);
                MatrixMultiplyF(&self->buttons[i]->matrix, &self->matrixTemp);
            }

            if (self->timer > 1.0) {
                self->timer = 0.0;
                self->state = EXTRASMENU_STATE_MAIN;
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
                        if (--self->selectedButton < 0)
                            self->selectedButton = 2;
                    }
                    else if (keyPress.down) {
                        PlaySfxByName("Menu Move", false);
                        if (++self->selectedButton > 2)
                            self->selectedButton = 0;
                    }

                    for (int i = 0; i < 3; ++i) self->buttons[i]->b = 0xFF;
                    self->buttons[self->selectedButton]->b = 0x0;

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
                            case 0: // DA Garden
                                id = GetSceneID(STAGELIST_PRESENTATION, "DA GARDEN");
                                if (id == -1) id = 4; // Fallback
                                break;
                            case 1: // Sound Test
                                id = GetSceneID(STAGELIST_PRESENTATION, "SOUND TEST");
                                if (id == -1) id = 6; // Fallback
                                break;
                            case 2: // Stage Select
                                id = GetSceneID(STAGELIST_PRESENTATION, "STAGE SELECT");
                                if (id == -1) id = 5; // Fallback
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

                    for (int i = 0; i < 3; ++i) {
                        if (CheckTouchRect(-80.0, 48.0 - i * 30.0, 112.0, 12.0) >= 0) {
                            self->selectedButton = i;
                            for (int j = 0; j < 3; ++j) self->buttons[j]->b = 0xFF;
                            self->buttons[i]->b = 0x0;
                        }
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

            self->timer += Engine.deltaTime * 2.0;

            self->label->alignOffset += 10.0 * (60.0 * Engine.deltaTime);

            for (int i = 0; i < 3; ++i) {
                self->buttons[i]->matXOff += (11.0 * (60.0 * Engine.deltaTime));
            }

            if (self->timer > 1.0) {
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

    if (self->state != EXTRASMENU_STATE_EXIT) {
        RenderMesh(self->meshPanel, MESH_COLORS, false);

        SetRenderBlendMode(RENDER_BLEND_ALPHA);
        SetRenderVertexColor(0xFF, 0xFF, 0x00);
        RenderImage(120.0, 48.0, 0.0, 0.2, 0.2, 256.0, 256.0, 512.0, 512.0, 0.0, 0.0, 255, self->textureCircle);
        RenderImage(128.0, -84.0, 0.0, 0.1, 0.1, 256.0, 256.0, 512.0, 512.0, 0.0, 0.0, 255, self->textureCircle);
        SetRenderVertexColor(0xFF, 0xFF, 0xFF);

        NewRenderState();
        MatrixRotateYF(&self->matrixTemp, DegreesToRad(-16.0));
        MatrixTranslateXYZF(&self->renderMatrix, 120.0, 48.0, 160.0);
        MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
        SetRenderMatrix(&self->renderMatrix);
        RenderImage(0.0, 0.0, 0.0, 0.3, 0.3, 256.0, 256.0, 512.0, 512.0, 0.0, 0.0, 255, self->textureExtras);

        NewRenderState();
        SetRenderMatrix(NULL);

        if (self->backPressed)
            RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, self->arrowAlpha, self->textureArrows);
        else
            RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, self->arrowAlpha, self->textureArrows);
    }
}
