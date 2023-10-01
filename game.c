#include <stdio.h>
#include <SDL2/SDL.h>
#include "./constants.h"

SDL_Window *window;
SDL_Renderer *renderer;
int game_is_running = FALSE;
int last_frame_time = 0;

struct game_object {
    float x;
    float y;
    float width;
    float height;
} helicopter;

int initialize_window() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error initializing SDL\n");
        return FALSE;
    }

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );

    if (!window) {
        printf("Error creating SDL window\n");
        return FALSE;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        printf("Error creating SDL renderer\n");
        return FALSE;
    }


    return TRUE;
}

void destroy_window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void process_input() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    game_is_running = FALSE;
                }
                break;
        }
    }
}

void setup() {
    helicopter.height = 90;
    helicopter.width = 140;
    helicopter.x = 20;
    helicopter.y = WINDOW_HEIGHT - 100;
}
                                     
void render() {
    SDL_SetRenderDrawColor(renderer, 183, 239, 197, 255);
    SDL_RenderClear(renderer);

    SDL_Rect helicopter_object = {
        (int) helicopter.x,
        (int) helicopter.y,
        (int) helicopter.width,
        (int) helicopter.height
    };
    SDL_SetRenderDrawColor(renderer, 16, 69, 29, 255);
    SDL_RenderFillRect(renderer, &helicopter_object);

    SDL_RenderPresent(renderer);
}

void update() {
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
    last_frame_time = SDL_GetTicks();
    helicopter.x += PIXELS_PER_SECOND * delta_time;
}

// PROCESS INPUT -> UPDATE -> RENDER ------
// ^                                      |
// |--------------------------------------- 


int main() {
    game_is_running = initialize_window();

    setup();

    while (game_is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
    return 0;
}