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
    int id;
    int velocity;
    SDL_Rect sdl_obj;
} AntiAircraft;

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

// --------------------AIRCRAFT---------------------------

GameObject setup_helicopter() {
    GameObject helicopter;

    helicopter.width = 200;
    helicopter.height = 200;
    helicopter.y = 0;
    helicopter.x = 0;
    helicopter.velocity = 5;
    
    return helicopter;
}

void render_helicopter(GameObject helicopter) {
    SDL_Rect sdl_obj = {
        (int) helicopter.x,
        (int) helicopter.y,
        (int) helicopter.width,
        (int) helicopter.height
    };
    SDL_SetRenderDrawColor(renderer, 255, 15, 15, 255);
    SDL_RenderFillRect(renderer, &sdl_obj);
}

void move_helicopter(GameObject *helicopter, const Uint8 *keyboardState) {
    if (keyboardState[SDL_SCANCODE_LEFT]) {
        helicopter->x -= helicopter->velocity;
    }
    if (keyboardState[SDL_SCANCODE_RIGHT]) {
        helicopter->x += helicopter->velocity;
    }
    

    if (helicopter->x < 0) {
        helicopter->x = 0;
    }
    if (helicopter->x + helicopter->width > WINDOW_WIDTH) {
        helicopter->x = WINDOW_WIDTH - helicopter->width;
    }
}   

void* helicopter_thread_func(void* args) {
    GameObject *helicopter = (GameObject *) args;

    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

    while (game_is_running) {
        move_helicopter(helicopter, keyboardState);
        SDL_Delay(10);
        }
    pthread_exit(NULL);
}

// --------------------ANTI AIRCRAFT---------------------------

AntiAircraft setup_aircraft(int id) {
    AntiAircraft aircraft;
    aircraft.id = id;
    aircraft.sdl_obj.w = 140;
    aircraft.sdl_obj.h = 90;
    aircraft.sdl_obj.y = WINDOW_HEIGHT - aircraft.sdl_obj.h;
    if (id == 0) {
        aircraft.sdl_obj.x = 20;
        aircraft.velocity = 2;
    } else {
        aircraft.sdl_obj.x = WINDOW_WIDTH - aircraft.sdl_obj.w - 20;
        aircraft.velocity = -3;
    }
    return aircraft;
}
                                     
void render_aircrafts(AntiAircraft aircrafts[]) {
    for (int i = 0; i < NUM_OF_ANTI_AIRCRAFTS; i++) {
        // SDL_Rect sdl_obj = {
        //     (int) aircrafts[i].x,
        //     (int) aircrafts[i].y,
        //     (int) aircrafts[i].width,
        //     (int) aircrafts[i].height
        // };
        SDL_SetRenderDrawColor(renderer, 16, 69, 29, 255);
        SDL_RenderFillRect(renderer, &(aircrafts[i].sdl_obj));
    }
}

void move_aircrafts(AntiAircraft *aircraft) {
    aircraft -> sdl_obj.x += aircraft -> velocity;
    if (aircraft -> sdl_obj.x < 0 || aircraft -> sdl_obj.x + aircraft -> sdl_obj.w > WINDOW_WIDTH) {
        aircraft -> velocity *= -1;
    }
}

void move_aircraft_out_of_bridge(AntiAircraft *aircraft) {
    if (aircraft -> velocity > 0) {
        while (aircraft -> sdl_obj.x < bridge.x + bridge.width) {
            aircraft -> sdl_obj.x += aircraft -> velocity;
            SDL_Delay(10);
        }
    } else {
        while (aircraft -> sdl_obj.x + aircraft -> sdl_obj.w > bridge.x) {
            aircraft -> sdl_obj.x += aircraft -> velocity;
            SDL_Delay(10);
        }
    }
}

void *anti_aircraft_thread(void *args) {
    AntiAircraft *anti_aircraft = (AntiAircraft *) args;

    while (game_is_running) {
        bool has_collided = 
            (anti_aircraft -> sdl_obj.x < bridge.x + bridge.width) &&
            (anti_aircraft -> sdl_obj.x + anti_aircraft -> sdl_obj.w > bridge.x);

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

void setup_bridge() {
    bridge.width = 500;
    bridge.height = 10;
    bridge.y = WINDOW_HEIGHT - bridge.height;
    bridge.x = (WINDOW_WIDTH / 2) - (bridge.width / 2);
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
}

void render_game(AntiAircraft anti_aircrafts[], Bridge bridge, GameObject helicopter) {
    SDL_SetRenderDrawColor(renderer, 183, 239, 197, 255);
    SDL_RenderClear(renderer);
    
    render_bridge(bridge);
    render_aircrafts(anti_aircrafts);
    render_helicopter(helicopter);

    SDL_RenderPresent(renderer);
}

// --------------------MAIN---------------------------

int main() {
    game_is_running = initialize_window();

    pthread_mutex_init(&bridge_mutex, NULL);
    setup_bridge();

    pthread_t helicopter_thread;
    GameObject helicopter = setup_helicopter();
    pthread_create(&helicopter_thread, NULL, helicopter_thread_func, &helicopter);

    AntiAircraft anti_aircrafts[NUM_OF_ANTI_AIRCRAFTS];    
    for (int i = 0; i < NUM_OF_ANTI_AIRCRAFTS; i++) {
        anti_aircrafts[i] = setup_aircraft(i);
    }

    pthread_t anti_aircraft_threads[NUM_OF_ANTI_AIRCRAFTS];
    for (int i = 0; i < NUM_OF_ANTI_AIRCRAFTS; i++) {
        pthread_create(&anti_aircraft_threads[i], NULL, anti_aircraft_thread, &anti_aircrafts[i]);
    }

    while (game_is_running) {
        process_input();
        render_game(anti_aircrafts, bridge, helicopter);
    }

    destroy_window();

    for (int i = 0; i < 2; i++) {
        pthread_join(anti_aircraft_threads[i], NULL);
    }

    pthread_mutex_destroy(&bridge_mutex);
    return 0;
}