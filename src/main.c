#include "raylib.h"

int UpdateGame(char** errorMessage);

static const unsigned char font_data[] = {
    #embed "resources/TX-02-Regular.otf"
};

const int screenWidth = 1280;
const int screenHeight = 720;

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Game Hot Reloading");

    Font debugFont = LoadFontFromMemory(".otf", font_data, sizeof(font_data), 24, 0, 0);
    char* message = 0;

    float textOffsetX = 0.0f;
    float textOffsetY = 0.0f;

    while (!WindowShouldClose()) {
        if (UpdateGame(&message)) {
            float wheel = GetMouseWheelMove();

            if(IsKeyDown(KEY_LEFT_ALT)) {
                textOffsetX += wheel * 50;
                if(textOffsetX > 0) textOffsetX = 0;
            } else {
                textOffsetY += wheel * 50;
                if(textOffsetY > 0) textOffsetY = 0;
            }

            BeginDrawing();
                ClearBackground(BLACK);
                DrawTextEx(debugFont, message, (Vector2){ 20 + textOffsetX, 20 + textOffsetY}, 24, 0, LIGHTGRAY);
            EndDrawing();
        }
    }

    CloseWindow();
}