#include "RetroEngine.hpp"
#include "String.hpp"

void ExtrasButton_Create(void *objPtr)
{
    RSDK_THIS(ExtrasButton);

    self->labelPtr = CREATE_ENTITY(TextLabel);
    self->labelPtr->fontID = FONT_LABEL;
    self->labelPtr->scale  = 0.15;
    self->labelPtr->alpha  = 0;
    self->labelPtr->state  = TEXTLABEL_STATE_IDLE;
    if (strExtras)
        SetStringToFont(self->labelPtr->text, strExtras, FONT_LABEL);
    else
        SetStringToFont8(self->labelPtr->text, "EXTRAS", FONT_LABEL);

    self->meshID = LoadMesh("Data/Game/Models/Cartridge.bin", 255);
    self->r      = 0xFF;
    self->g      = 0xFF;
    self->b      = 0xFF;
}

void ExtrasButton_Main(void *objPtr)
{
    RSDK_THIS(ExtrasButton);

    if (self->visible) {
        NewRenderState();
        MatrixTranslateXYZF(&self->renderMatrix, self->x, self->y, self->z);
        SetRenderMatrix(&self->renderMatrix);

        SetRenderVertexColor(self->r, self->g, self->b);
        RenderMesh(self->meshID, MESH_COLORS, false);

        NewRenderState();
        SetRenderMatrix(NULL);
        if (self->z < 256.0) {
            RenderText(self->labelPtr->text, FONT_LABEL, self->x, self->y - 24.0, self->z, 0.15, self->labelPtr->alpha);
        }
    }
}
