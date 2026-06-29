#include "RetroEngine.hpp"

void ExtrasButton_Create(void *objPtr)
{
    RSDK_THIS(ExtrasButton);
    self->textureCircle = LoadTexture("Data/Game/Menu/Circle.png", TEXFMT_RGBA4444);

    int texture          = LoadTexture("Data/Game/Models/Extras.png", TEXFMT_RGBA4444);
    self->meshCartridge = LoadMesh("Data/Game/Models/Extras.bin", texture);
    self->x              = 0.0;
    self->y              = 16.0;
    self->z              = 160.0;
    self->r              = 0xFF;
    self->g              = 0xFF;
    self->b              = 0x00;
    self->labelPtr       = CREATE_ENTITY(TextLabel);
    self->labelPtr->fontID = FONT_HEADING;
    self->labelPtr->scale  = 0.15;
    self->labelPtr->alpha  = 0;
    self->labelPtr->state  = TEXTLABEL_STATE_IDLE;
    if (strExtras)
        SetStringToFont(self->labelPtr->text, strExtras, FONT_HEADING);
    else
        SetStringToFont8(self->labelPtr->text, "EXTRAS", FONT_HEADING);
    self->labelPtr->alignPtr(self->labelPtr, ALIGN_CENTER);
}
void ExtrasButton_Main(void *objPtr)
{
    RSDK_THIS(ExtrasButton);

    if (self->visible) {
        if (self->scale < 0.2) {
            self->scale += ((0.25 - self->scale) / ((60.0 * Engine.deltaTime) * 16.0));
            if (self->scale > 0.2)
                self->scale = 0.2;
        }
        SetRenderBlendMode(RENDER_BLEND_ALPHA);
        SetRenderVertexColor(self->r, self->g, self->b);
        RenderImage(self->x, self->y, self->z, self->scale, self->scale, 256.0, 256.0, 512.0, 512.0, 0.0, 0.0, 255, self->textureCircle);
        SetRenderVertexColor(0xFF, 0xFF, 0xFF);
        SetRenderBlendMode(RENDER_BLEND_NONE);

        self->angle -= Engine.deltaTime;
        if (self->angle < -(M_PI * 2.0))
            self->angle += (M_PI * 2.0);

        NewRenderState();
        MatrixRotateXYZF(&self->renderMatrix, 0.0, 0.0, self->angle);
        MatrixTranslateXYZF(&self->matrixTemp, self->x, self->y, self->z - 8.0);
        MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
        SetRenderMatrix(&self->renderMatrix);
        RenderMesh(self->meshCartridge, MESH_COLORS, true);
        SetRenderMatrix(NULL);

        NativeEntity_TextLabel *label = self->labelPtr;
        label->x                      = self->x;
        label->y                      = self->y - 72.0;
        label->z                      = self->z;
        if (label->x <= -8.0 || label->x >= 8.0) {
            if (label->alpha > 0)
                label->alpha -= 8;
        }
        else {
            if (label->alpha < 0x100)
                label->alpha += 8;
        }
    }
}
