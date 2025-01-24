#include "raylib.h"
#include "raymath.h"

typedef struct {
    int init;
    int counter;
} State;

Color background_color = { 16, 32, 64, 255 };

void game_init(State* state) {
    state->init = 1;
    state->counter = 10;
}

void game_update(State* state) {
    if (state->init == 0) game_init(state);
    float deltaTime = GetFrameTime();

    if(IsKeyPressed(KEY_UP)) {
        state->counter += 1;
    }

    BeginDrawing();
        ClearBackground(background_color);
        DrawText(TextFormat("Counter: %d",state->counter), 200, 200, 40, WHITE);
    EndDrawing();
}