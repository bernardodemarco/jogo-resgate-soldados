#include <stdio.h>
#include <SDL2/SDL.h>
#include <pthread.h>
#include <stdbool.h>
#include "./constants.h"

typedef struct {
    int id;
    float x;
    float y;
    float width;
    float height;
    float velocity;
} GameObject;

typedef struct {
    float x;
    float y;
    float width;
    float height;
} Bridge;

SDL_Window *window;
SDL_Renderer *renderer;
Bridge bridge;
pthread_mutex_t bridge_mutex;
bool game_is_running = false;

// --------------------WINDOW---------------------------

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

// --------------------INPUT---------------------------

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

// --------------------ANTI AIRCRAFT---------------------------

void setup_aircraft(GameObject *aircraft, int i) {
    aircraft -> id = i;
    aircraft -> width = 140;
    aircraft -> height = 90;
    aircraft -> y = WINDOW_HEIGHT - aircraft -> height;
    if (i == 0) {
        aircraft -> x = 20;
        aircraft -> velocity = 2;
    } else {
        aircraft -> x = WINDOW_WIDTH - aircraft -> width - 20;
        aircraft -> velocity = -3;
    }
}
                                     
void render_aircrafts(GameObject aircrafts[]) {
    SDL_SetRenderDrawColor(renderer, 183, 239, 197, 255);
    SDL_RenderClear(renderer);
    for (int i = 0; i < NUM_OF_ANTI_AIRCRAFTS; i++) {
        SDL_Rect sdl_obj = {
            (int) aircrafts[i].x,
            (int) aircrafts[i].y,
            (int) aircrafts[i].width,
            (int) aircrafts[i].height
        };
        SDL_SetRenderDrawColor(renderer, 16, 69, 29, 255);
        SDL_RenderFillRect(renderer, &sdl_obj);
    }
}

void move_aircrafts(GameObject *aircraft) {
    aircraft -> x += aircraft -> velocity;
    if (aircraft -> x < 0 || aircraft -> x + aircraft -> width > WINDOW_WIDTH) {
        aircraft -> velocity *= -1;
    }
}

void move_aircraft_out_of_bridge(GameObject *aircraft) {
    if (aircraft -> velocity > 0) {
        while (aircraft -> x < bridge.x + bridge.width) {
            aircraft -> x += aircraft -> velocity;
            SDL_Delay(10);
        }
    } else {
        while (aircraft -> x + aircraft -> width > bridge.x) {
            aircraft -> x += aircraft -> velocity;
            SDL_Delay(10);
        }
    }
}

void *anti_aircraft_thread(void *args) {
    GameObject *anti_aircraft = (GameObject *) args;

    while (game_is_running) {
        bool has_collided = 
            (anti_aircraft -> x < bridge.x + bridge.width) &&
            (anti_aircraft -> x + anti_aircraft -> width > bridge.x);

        if (has_collided) {
            pthread_mutex_lock(&bridge_mutex);
            move_aircraft_out_of_bridge(anti_aircraft);
            pthread_mutex_unlock(&bridge_mutex);
        } else {
            move_aircrafts(anti_aircraft);
            SDL_Delay(10);
        }
    }
    pthread_exit(NULL);
}

// --------------------BRIDGE---------------------------

void setup_bridge(Bridge *bridge) {
    bridge -> width = 500;
    bridge -> height = 10;
    bridge -> y = WINDOW_HEIGHT - bridge -> height;
    bridge -> x = (WINDOW_WIDTH / 2) - (bridge -> width / 2);
}   

void render_bridge(Bridge bridge) {
    SDL_Rect sdl_obj = {
        (int) bridge.x,
        (int) bridge.y,
        (int) bridge.width,
        (int) bridge.height
    };
    SDL_SetRenderDrawColor(renderer, 102, 66, 40, 255);
    SDL_RenderFillRect(renderer, &sdl_obj);
    SDL_RenderPresent(renderer);
}

// --------------------MAIN---------------------------

int main() {
    pthread_mutex_init(&bridge_mutex, NULL);

    game_is_running = initialize_window();
    setup_bridge(&bridge);

    GameObject anti_aircrafts[NUM_OF_ANTI_AIRCRAFTS];
    pthread_t anti_aircraft_threads[NUM_OF_ANTI_AIRCRAFTS];
    
    for (int i = 0; i < NUM_OF_ANTI_AIRCRAFTS; i++) {
        setup_aircraft(&anti_aircrafts[i], i);
    }

    for (int i = 0; i < NUM_OF_ANTI_AIRCRAFTS; i++) {
        pthread_create(&anti_aircraft_threads[i], NULL, anti_aircraft_thread, &anti_aircrafts[i]);
    }

    while (game_is_running) {
        process_input();
        render_aircrafts(anti_aircrafts);
        render_bridge(bridge);
    }

    destroy_window();

    for (int i = 0; i < 2; i++) {
        pthread_join(anti_aircraft_threads[i], NULL);
    }

    pthread_mutex_destroy(&bridge_mutex);
    return 0;
}