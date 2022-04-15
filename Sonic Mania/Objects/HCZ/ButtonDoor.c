// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: ButtonDoor Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectButtonDoor *ButtonDoor;

void ButtonDoor_Update(void)
{
    RSDK_THIS(ButtonDoor);
    EntityButton *button = self->taggedButton;
    if (button && button->activated) {

        if (!self->movePos)
            RSDK.PlaySfx(ButtonDoor->sfxOpen, false, 255);

        if (self->movePos < self->duration) {
            switch (self->orientation) {
                case BUTTONDOOR_UP: self->position.y = self->startPos.y - self->movePos * (self->size.y / self->duration); break;
                case BUTTONDOOR_LEFT: self->position.x = self->startPos.x - self->movePos * (self->size.x / self->duration); break;
                case BUTTONDOOR_DOWN: self->position.y = self->startPos.y + self->movePos * (self->size.y / self->duration); break;
                case BUTTONDOOR_RIGHT: self->position.x = self->startPos.x + self->movePos * (self->size.x / self->duration); break;
            }
            self->movePos++;
        }
        else {
            switch (self->orientation) {
                case BUTTONDOOR_UP: self->position.y = self->startPos.y - self->size.y; break;
                case BUTTONDOOR_LEFT: self->position.x = self->startPos.x - self->size.x; break;
                case BUTTONDOOR_DOWN: self->position.y = self->startPos.y + self->size.y; break;
                case BUTTONDOOR_RIGHT: self->position.x = self->startPos.x + self->size.x; break;
            }
        }
    }

    foreach_active(Player, player) { Player_CheckCollisionBox(player, self, &self->hitbox); }
}

void ButtonDoor_LateUpdate(void) {}

void ButtonDoor_StaticUpdate(void) {}

void ButtonDoor_Draw(void) { ButtonDoor_DrawSprites(); }

void ButtonDoor_Create(void *data)
{
    RSDK_THIS(ButtonDoor);

    if (!self->duration)
        self->duration = 6;

    if (!self->length)
        self->length = 2 - (RSDK.CheckStageFolder("HCZ") != true);

    self->active        = ACTIVE_BOUNDS;
    self->drawOrder     = Zone->drawOrderLow;
    self->startPos.x    = self->position.x;
    self->startPos.y    = self->position.y;
    self->visible       = 1;
    self->drawFX        = FX_FLIP;
    self->updateRange.x = self->size.x + 0x800000;
    self->updateRange.y = self->size.y + 0x800000;
    ButtonDoor_SetupSize();
    ButtonDoor_SetupTagLink();
    RSDK.SetSpriteAnimation(ButtonDoor->aniFrames, 0, &self->animator, true, 0);
}

void ButtonDoor_StageLoad(void)
{
    if (RSDK.CheckStageFolder("LRZ1")) {
        ButtonDoor->aniFrames = RSDK.LoadSpriteAnimation("LRZ1/ButtonDoor.bin", SCOPE_STAGE);
        ButtonDoor->sfxOpen   = RSDK.GetSfx("Stage/Open.wav");
    }
    else if (RSDK.CheckStageFolder("LRZ2")) {
        ButtonDoor->aniFrames = RSDK.LoadSpriteAnimation("LRZ2/ButtonDoor.bin", SCOPE_STAGE);
        ButtonDoor->sfxOpen   = RSDK.GetSfx("Stage/Open.wav");
    }
    else if (RSDK.CheckStageFolder("HCZ")) {
        ButtonDoor->aniFrames = RSDK.LoadSpriteAnimation("HCZ/ButtonDoor.bin", SCOPE_STAGE);
        ButtonDoor->sfxOpen   = RSDK.GetSfx("Stage/Clack.wav");
    }
}

void ButtonDoor_SetupSize(void)
{
    RSDK_THIS(ButtonDoor);

    if (RSDK.CheckStageFolder("LRZ1")) {
        self->size.y = 0x800000;
        self->size.x = 0x200000;
    }
    else if (RSDK.CheckStageFolder("LRZ2")) {
        self->size.y = 0x400000;
        self->size.x = 0x200000;
    }
    else if (RSDK.CheckStageFolder("HCZ")) {
        self->size.y = 0x200000;
        self->size.x = 0x200000;
    }

    // Size of one "block"
    self->segmentSize = self->size;

    // Total Size
    if (self->orientation == BUTTONDOOR_UP || self->orientation == BUTTONDOOR_DOWN)
        self->size.y *= self->length;
    else if (self->orientation == BUTTONDOOR_LEFT || self->orientation == BUTTONDOOR_RIGHT)
        self->size.x *= self->length;

    self->hitbox.left   = -(self->size.x >> 17);
    self->hitbox.top    = -(self->size.y >> 17);
    self->hitbox.right  = self->size.x >> 17;
    self->hitbox.bottom = self->size.y >> 17;
}

void ButtonDoor_SetupTagLink(void)
{
    RSDK_THIS(ButtonDoor);

    self->taggedButton = NULL;

    if (RSDK.CheckStageFolder("LRZ2")) {
        self->taggedButton = LRZ2Setup_SetupTagLink(self->buttonTag, (Entity *)self);
    }
    else {
        EntityButton *taggedButton = RSDK.GetEntityByID(RSDK.GetEntityID(self) - 1);
        bool32 tagged              = false;

        if (self->buttonTag > 0) {
            if (Button) {
                foreach_all(Button, button)
                {
                    if (button->tag == self->buttonTag) {
                        taggedButton = button;
                        tagged       = true;
                        foreach_break;
                    }
                }
            }

            if (PullChain && !tagged) {
                foreach_all(PullChain, chain)
                {
                    if (chain->tag == self->buttonTag) {
                        taggedButton = (EntityButton *)chain;
                        tagged       = true;
                        foreach_break;
                    }
                }
            }

            if (TurretSwitch && !tagged) {
                foreach_all(TurretSwitch, turretSwitch)
                {
                    if (turretSwitch->tag == self->buttonTag) {
                        taggedButton = (EntityButton *)turretSwitch;
                        tagged = true;
                        foreach_break;
                    }
                }
            }
        }

        if (taggedButton) {
            if ((Button && taggedButton->objectID == Button->objectID) || (TurretSwitch && taggedButton->objectID == TurretSwitch->objectID)
                || (PullChain && taggedButton->objectID == PullChain->objectID)) {
                if (self) {
                    int32 distX = abs(self->position.x - taggedButton->position.x);
                    int32 distY = abs(self->position.y - taggedButton->position.y);

                    if (self->updateRange.x < 0x800000 + distX)
                        self->updateRange.x = 0x800000 + distX;

                    if (self->updateRange.y < 0x800000 + distY)
                        self->updateRange.y = 0x800000 + distY;
                }
                self->taggedButton = taggedButton;
            }
        }
    }
}

void ButtonDoor_DrawSprites(void)
{
    RSDK_THIS(ButtonDoor);
    Vector2 drawPos;

    drawPos.x = self->position.x;
    drawPos.y = self->position.y;

    int32 incX = 0, incY = 0;
    if (self->orientation == BUTTONDOOR_UP || self->orientation == BUTTONDOOR_DOWN) {
        drawPos.y = (self->segmentSize.y >> 1) + self->position.y - (self->size.y >> 1);
        incY      = self->segmentSize.y;
    }
    else if (self->orientation == BUTTONDOOR_LEFT || self->orientation == BUTTONDOOR_RIGHT) {
        drawPos.x = (self->segmentSize.x >> 1) + self->position.x - (self->size.x >> 1);
        incX      = self->segmentSize.x;
    }

    for (int32 i = 0; i < self->length; ++i) {
        RSDK.DrawSprite(&self->animator, &drawPos, false);
        drawPos.x += incX;
        drawPos.y += incY;
    }
}

#if RETRO_INCLUDE_EDITOR
void ButtonDoor_EditorDraw(void)
{
    RSDK_THIS(ButtonDoor);

    ButtonDoor_SetupSize();
    ButtonDoor_DrawSprites();

    if (showGizmos()) {
        ButtonDoor_SetupTagLink();

        RSDK_DRAWING_OVERLAY(true);
        if (self->taggedButton) 
            DrawHelpers_DrawArrow(self->taggedButton->position.x, self->taggedButton->position.y, self->position.x, self->position.y, 0xFFFF00);
        RSDK_DRAWING_OVERLAY(false);
    }
}

void ButtonDoor_EditorLoad(void)
{
    if (RSDK.CheckStageFolder("LRZ1"))
        ButtonDoor->aniFrames = RSDK.LoadSpriteAnimation("LRZ1/ButtonDoor.bin", SCOPE_STAGE);
    else if (RSDK.CheckStageFolder("LRZ2"))
        ButtonDoor->aniFrames = RSDK.LoadSpriteAnimation("LRZ2/ButtonDoor.bin", SCOPE_STAGE);
    else if (RSDK.CheckStageFolder("HCZ"))
        ButtonDoor->aniFrames = RSDK.LoadSpriteAnimation("HCZ/ButtonDoor.bin", SCOPE_STAGE);

    RSDK_ACTIVE_VAR(ButtonDoor, orientation);
    RSDK_ENUM_VAR("Up", BUTTONDOOR_UP);
    RSDK_ENUM_VAR("Left", BUTTONDOOR_LEFT);
    RSDK_ENUM_VAR("Down", BUTTONDOOR_DOWN);
    RSDK_ENUM_VAR("Right", BUTTONDOOR_RIGHT);
}
#endif

void ButtonDoor_Serialize(void)
{
    RSDK_EDITABLE_VAR(ButtonDoor, VAR_ENUM, buttonTag);
    RSDK_EDITABLE_VAR(ButtonDoor, VAR_ENUM, length);
    RSDK_EDITABLE_VAR(ButtonDoor, VAR_ENUM, duration);
    RSDK_EDITABLE_VAR(ButtonDoor, VAR_UINT8, orientation);
}
