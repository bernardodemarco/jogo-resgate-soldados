#include <stdio.h>
#include <SDL2/SDL.h>
#include <pthread.h>
#include <stdbool.h>
#include "./constants.h"

SDL_Window *window;
SDL_Renderer *renderer;
int game_is_running = false;

typedef struct {
    int id;
    float x;
    float y;
    float width;
    float height;
    float velocity;
} GameObject;

int initialize_window() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error initializing SDL\n");
        return false;
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
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        printf("Error creating SDL renderer\n");
        return false;
    }


    return true;
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
            case SDL_QUIT:
                game_is_running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    game_is_running = false;
                }
                break;
        }
    }
}
                                     
void render(GameObject *obj1, GameObject *obj2) {
    SDL_SetRenderDrawColor(renderer, 183, 239, 197, 255);
    SDL_RenderClear(renderer);

    SDL_Rect sdl_obj1 = {
        (int) obj1 -> x,
        (int) obj1 -> y,
        (int) obj1 -> width,
        (int) obj1 -> height
    };

    SDL_Rect sdl_obj2 = {
        (int) obj2 -> x,
        (int) obj2 -> y,
        (int) obj2 -> width,
        (int) obj2 -> height
    };
    SDL_SetRenderDrawColor(renderer, 16, 69, 29, 255);
    SDL_RenderFillRect(renderer, &sdl_obj1);
    SDL_RenderFillRect(renderer, &sdl_obj2);

    SDL_RenderPresent(renderer);
}


void move_aircrafts(GameObject *aircraft) {
    if (aircraft -> id == 0) {
        aircraft -> x += aircraft -> velocity;
    } else {
        aircraft -> x -= aircraft -> velocity;
    }
    // if (obj -> x < 0 || obj -> x + obj -> width > WINDOW_WIDTH) {
    //     obj->velocity *= -1;
    // }
}

void *anti_aircraft_thread(void *args) {
    GameObject *anti_aircraft = (GameObject *) args;

    while (game_is_running) {
        move_aircrafts(anti_aircraft);
        SDL_Delay(10);
    }
    pthread_exit(NULL);
}

void setup_aircraft(GameObject *aircraft, int i) {
    aircraft -> id = i;
    aircraft -> width = 140;
    aircraft -> height = 90;
    aircraft -> y = WINDOW_HEIGHT - 100;
    aircraft -> velocity = 2;
    if (i == 0) {
        aircraft -> x = 20;
    } else {
        aircraft -> x = WINDOW_WIDTH - 20;
    }
}


int main() {
    game_is_running = initialize_window();
    GameObject anti_aircrafts[NUM_OF_ANTI_AIRCRAFTS];

    for (int i = 0; i < NUM_OF_ANTI_AIRCRAFTS; i++) {
        setup_aircraft(&anti_aircrafts[i], i);
    }

    pthread_t anti_aircraft_threads[NUM_OF_ANTI_AIRCRAFTS];

    for (int i = 0; i < NUM_OF_ANTI_AIRCRAFTS; i++) {
        pthread_create(&anti_aircraft_threads[i], NULL, anti_aircraft_thread, &anti_aircrafts[i]);
    }

    while (game_is_running) {
        process_input();
        render(&anti_aircrafts[0], &anti_aircrafts[1]);
    }

    destroy_window();

    for (int i = 0; i < 2; i++) {
        pthread_join(anti_aircraft_threads[i], NULL);
    }

    return 0;
}