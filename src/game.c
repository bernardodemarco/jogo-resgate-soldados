#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <pthread.h>
#include <semaphore.h>
#include "./constants.h"
#include "./types.h"

SDL_Window *window;
SDL_Renderer *renderer;

Bridge bridge;
pthread_mutex_t bridge_mutex;

pthread_mutex_t is_missile_active_mutex;
pthread_mutex_t left_building_mutex;

bool game_is_running = false;
bool is_helicopter_destroyed = false;
bool is_helicopter_with_hostage = false;
bool has_missile_collided_with_helicopter = false;
int right_building_hostages = 0;

// --------------GAME DIFFICULTY----------------

int reload_time = 0;
int time_between_shots = 0;
int ammunition = 0;

void init_difficulty_vars(int difficulty) {
    if (difficulty == EASY) {
        reload_time = EASY_RELOAD_TIME;
        time_between_shots = EASY_TIME_BETWEEN_SHOTS;
        ammunition = EASY_AMMUNITION;
    } else if (difficulty == MEDIUM) {
        reload_time = MEDIUM_RELOAD_TIME;
        time_between_shots = MEDIUM_TIME_BETWEEN_SHOTS;
        ammunition = MEDIUM_AMMUNITION;
    } else if (difficulty == HARD) {
        reload_time = HARD_RELOAD_TIME;
        time_between_shots = HARD_TIME_BETWEEN_SHOTS;
        ammunition = HARD_AMMUNITION;
    }
}

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

// --------------------BRIDGE---------------------------

void setup_bridge() {
    bridge.sdl_obj.w = BRIDGE_WIDTH;
    bridge.sdl_obj.h = BRIDGE_HEIGHT;
    bridge.sdl_obj.y = BRIDGE_Y;
    bridge.sdl_obj.x = BRIDGE_X;
}   

void render_bridge(Bridge bridge) {
    SDL_SetRenderDrawColor(renderer, 102, 66, 40, 255);
    SDL_RenderFillRect(renderer, &(bridge.sdl_obj));
}

// --------------------BUILDINGS---------------------------

Building setup_buildings(int w, int h, int x, int y) {
    Building building;
    building.sdl_obj.w = w;
    building.sdl_obj.h = h;
    building.sdl_obj.x = x;
    building.sdl_obj.y = y;
    return building;
}   

void render_buildings(Building buildings[]) {
    for (int i = 0; i < NUM_OF_BUILDINGS; i++) {
        SDL_SetRenderDrawColor(renderer, 102, 66, 40, 255);
        SDL_RenderFillRect(renderer, &(buildings[i].sdl_obj));
    }
}

// --------------------HOSTAGES---------------------------

Hostage setup_hostages(int w, int h, int x, int y) {
    Hostage hostage;
    hostage.sdl_obj.w = w;
    hostage.sdl_obj.h = h;
    hostage.sdl_obj.x = x;
    hostage.sdl_obj.y = y;
    return hostage;
}   

void render_hostages(Hostage hostages[]) {
    for (int i = 0; i < NUM_OF_INITIAL_HOSTAGES; i++) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &(hostages[i].sdl_obj));
    }
}

// --------------------SCENARIO-------------------
void setup_scenario(Building *buildings, Hostage *hostages) {
    setup_bridge();
    
    buildings[0] = setup_buildings(
        BUILDING_WIDTH,
        BUILDING_HEIGHT,
        LEFT_BUILDING_X,
        BUILDING_Y
    );
    
    buildings[1] = setup_buildings(
        BUILDING_WIDTH,
        BUILDING_HEIGHT,
        RIGHT_BUILDING_X,
        BUILDING_Y
    );

    int x = 8; // posição X do primeiro refém
    for (int i = 0; i < NUM_OF_INITIAL_HOSTAGES; i++) {
        hostages[i] = setup_hostages(
            HOSTAGE_WIDTH,
            HOSTAGE_HEIGHT,
            x,
            HOSTAGE_Y
        );
        x += 20; // reféns 20px à direita do 1o refém 
    }
}

// --------------------HELICOPTER---------------------------

Helicopter setup_helicopter() {
    Helicopter helicopter;

    helicopter.sdl_obj.w = HELICOPTER_WIDTH;
    helicopter.sdl_obj.h = HELICOPTER_HEIGHT;
    helicopter.sdl_obj.y = HELICOPTER_Y;
    helicopter.sdl_obj.x = HELICOPTER_X;
    helicopter.velocity = HELICOPTER_VELOCITY;
    
    return helicopter;
}

void render_helicopter(Helicopter helicopter) {
    SDL_SetRenderDrawColor(renderer, 16, 69, 29, 255);
    SDL_RenderFillRect(renderer, &(helicopter.sdl_obj));
}

void move_helicopter(Helicopter *helicopter, const Uint8 *keyboardState, Building buildings[NUM_OF_BUILDINGS], Hostage hostages[]) {
    if (keyboardState[SDL_SCANCODE_LEFT]) {
        helicopter -> sdl_obj.x -= helicopter -> velocity;
    }

    if (keyboardState[SDL_SCANCODE_RIGHT]) {
        helicopter -> sdl_obj.x += helicopter -> velocity;
    }
  
    if (keyboardState[SDL_SCANCODE_UP]) {
        helicopter -> sdl_obj.y -= helicopter -> velocity;
    }
  
    if (keyboardState[SDL_SCANCODE_DOWN]) {
        helicopter -> sdl_obj.y += helicopter -> velocity;
    }

    bool has_collided_with_viewport = 
        (helicopter -> sdl_obj.x <= 0) ||
        (helicopter -> sdl_obj.x + helicopter -> sdl_obj.w >= WINDOW_WIDTH) ||
        (helicopter -> sdl_obj.y <= 0) ||
        (helicopter -> sdl_obj.y + helicopter -> sdl_obj.h >= WINDOW_HEIGHT - ANTI_AIRCRAFT_HEIGHT);
    
    bool has_collided_with_buildings = 
        SDL_HasIntersection(&(helicopter -> sdl_obj), &(buildings[0].sdl_obj)) ||
        SDL_HasIntersection(&(helicopter -> sdl_obj), &(buildings[1].sdl_obj));

    if (has_collided_with_viewport || has_collided_with_buildings) {
        is_helicopter_destroyed = true;
    }

    bool has_picked_hostage =
        (helicopter -> sdl_obj.x <= 100) &&
        (helicopter -> sdl_obj.y < WINDOW_HEIGHT - BUILDING_HEIGHT) &&
        (helicopter -> sdl_obj.y > WINDOW_HEIGHT - BUILDING_HEIGHT - 175);
   
    bool has_left_hostage =
        (helicopter -> sdl_obj.x > WINDOW_WIDTH - HELICOPTER_WIDTH - 100) &&
        (helicopter -> sdl_obj.y < WINDOW_HEIGHT - BUILDING_HEIGHT) &&
        (helicopter -> sdl_obj.y > WINDOW_HEIGHT - BUILDING_HEIGHT - 175);

    if (has_picked_hostage && !is_helicopter_with_hostage) {
        is_helicopter_with_hostage = true;
        hostages[right_building_hostages].sdl_obj.w = 0;
    }

    if (has_left_hostage && is_helicopter_with_hostage) {
        is_helicopter_with_hostage = false;
        hostages[right_building_hostages].sdl_obj.w = HOSTAGE_WIDTH;
        hostages[right_building_hostages].sdl_obj.x = WINDOW_WIDTH - 195 + (right_building_hostages * 15);
        right_building_hostages += 1;
    }
}   

void* helicopter_thread_func(void* args) {
    HelicopterThreadArgs *helicopter_thread_args = (HelicopterThreadArgs *) args;

    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

    while (true) {
        move_helicopter(helicopter_thread_args -> helicopter, keyboardState, helicopter_thread_args -> buildings, helicopter_thread_args -> hostages);
        SDL_Delay(10);
    }

    pthread_exit(NULL);
}
// --------------------MISSILE---------------------------
void render_missile(SDL_Rect *sdl_obj) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, sdl_obj);
}

void setup_missile(Missile *missile, AntiAircraft *anti_aircraft) {
    missile -> sdl_obj.w = MISSILE_SIZE;
    missile -> sdl_obj.h = MISSILE_SIZE;
    missile -> sdl_obj.x = anti_aircraft -> sdl_obj.x + (anti_aircraft -> sdl_obj.w / 2);
    missile -> sdl_obj.y = anti_aircraft -> sdl_obj.y;
}

void *missile_thread_func(void *args) {
    MissileThreadArgs *thread_args = (MissileThreadArgs *) args;
    Missile *missile = thread_args -> missile;
    Helicopter *helicopter = thread_args -> helicopter;

    pthread_mutex_lock(&is_missile_active_mutex);
    missile -> is_active = true;
    pthread_mutex_unlock(&is_missile_active_mutex);
    while (missile -> sdl_obj.y > -MISSILE_SIZE) {
        bool has_collided_with_helicopter = SDL_HasIntersection(&(helicopter -> sdl_obj), &(missile -> sdl_obj));

        if (has_collided_with_helicopter) {
            has_missile_collided_with_helicopter = true;
            pthread_exit(NULL);
        }

        missile -> sdl_obj.y -= 10;
        SDL_Delay(10);
    }
    pthread_mutex_lock(&is_missile_active_mutex);
    missile -> is_active = false;
    pthread_mutex_unlock(&is_missile_active_mutex);
    pthread_exit(NULL);
}

// --------------------ANTI AIRCRAFT---------------------------

AntiAircraft setup_aircraft(int id) {
    AntiAircraft aircraft;
    aircraft.id = id;
    aircraft.sdl_obj.w = ANTI_AIRCRAFT_WIDTH;
    aircraft.sdl_obj.h = ANTI_AIRCRAFT_HEIGHT;
    aircraft.sdl_obj.y = ANTI_AIRCRAFT_Y;
    aircraft.last_shot = SDL_GetTicks();
    if (id == 0) {
        aircraft.sdl_obj.x = BUILDING_WIDTH + 10;
        aircraft.velocity = 2;
    } else {
        aircraft.sdl_obj.x = WINDOW_WIDTH - BUILDING_WIDTH - ANTI_AIRCRAFT_WIDTH;
        aircraft.velocity = -3;
    }

    aircraft.missiles = malloc(sizeof(Missile) * ammunition);

    return aircraft;
}
                                     
void render_aircrafts(AntiAircraft aircrafts[]) {
    for (int i = 0; i < NUM_OF_ANTI_AIRCRAFTS; i++) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &(aircrafts[i].sdl_obj));
    }

    for (int i = 0; i < ammunition; i++) {
        Missile missile = aircrafts[0].missiles[i];

        bool should_render_missile = 
            (missile.sdl_obj.w == MISSILE_SIZE) &&
            (missile.sdl_obj.h == MISSILE_SIZE) &&
            (missile.is_active == 1);

        if (should_render_missile) {
            render_missile(&missile.sdl_obj);
        }
    }
 
    for (int i = 0; i < ammunition; i++) {
        Missile missile = aircrafts[1].missiles[i];

        bool should_render_missile = 
            (missile.sdl_obj.w == MISSILE_SIZE) &&
            (missile.sdl_obj.h == MISSILE_SIZE) &&
            (missile.is_active == 1);

        if (should_render_missile) {
            render_missile(&missile.sdl_obj);
        }
    }
}

void move_aircrafts(AntiAircraft *aircraft) {
    aircraft -> sdl_obj.x += aircraft -> velocity;
    if (aircraft -> sdl_obj.x < BUILDING_WIDTH || aircraft -> sdl_obj.x + aircraft -> sdl_obj.w > WINDOW_WIDTH - BUILDING_WIDTH) {
        aircraft -> velocity *= -1;
    }
}

void leave_building(AntiAircraft *aircraft) {
    while (aircraft -> sdl_obj.x < BUILDING_WIDTH) {
        if (aircraft -> velocity < 0) {
            aircraft -> sdl_obj.x += -(aircraft -> velocity);
        } else {
            aircraft -> sdl_obj.x += aircraft -> velocity;
        }
        SDL_Delay(10);
    }
}

void move_aircraft_out_of_bridge(AntiAircraft *aircraft) {
    if (aircraft -> velocity > 0) {
        while (aircraft -> sdl_obj.x < bridge.sdl_obj.x + bridge.sdl_obj.w) {
            aircraft -> sdl_obj.x += aircraft -> velocity;
            SDL_Delay(10);
        }
    } else {
        while (aircraft -> sdl_obj.x + aircraft -> sdl_obj.w > bridge.sdl_obj.x) {
            aircraft -> sdl_obj.x += aircraft -> velocity;
            SDL_Delay(10);
        }
    }
}

void handle_bridge_on_move_to_left_building(AntiAircraft *aircraft) {
    if (aircraft -> velocity > 0) {
        aircraft -> velocity *= -1;
    }

    while (aircraft -> sdl_obj.x + aircraft -> sdl_obj.w > bridge.sdl_obj.x) {
        aircraft -> sdl_obj.x += aircraft -> velocity;
        SDL_Delay(10);
    }
}

void move_to_left_building(AntiAircraft *aircraft) {
    while (aircraft -> sdl_obj.x > aircraft -> sdl_obj.w) {
        bool has_collided_with_bridge = 
            (aircraft -> sdl_obj.x < bridge.sdl_obj.x + bridge.sdl_obj.w) &&
            (aircraft -> sdl_obj.x + aircraft -> sdl_obj.w > bridge.sdl_obj.x);
        
        if (has_collided_with_bridge) {
            pthread_mutex_lock(&bridge_mutex);
            handle_bridge_on_move_to_left_building(aircraft);
            pthread_mutex_unlock(&bridge_mutex);
        }

        if (aircraft -> velocity > 0) {
            aircraft -> sdl_obj.x -= aircraft -> velocity;
        } else {
            aircraft -> sdl_obj.x += aircraft -> velocity;
        }
        SDL_Delay(10);
    }
}

void reload_ammunition(sem_t *sem) {
    for (int i = 0; i < ammunition; i++) {
        sem_post(sem);
    }
    SDL_Delay(reload_time);
}

void *anti_aircraft_thread(void *args) {
    AntiAircraftThreadArgs *thread_args = (AntiAircraftThreadArgs *) args;
    AntiAircraft *anti_aircraft = thread_args -> anti_aircraft;
    Helicopter *helicopter = thread_args -> helicopter;
    
    sem_init(&(anti_aircraft -> ammunition_sem), 0, ammunition);
    bool needs_to_reload = false;
    bool is_left_building_occupied = false;

    int missile_index = 0;
    pthread_mutex_t missile_index_mutex;
    pthread_mutex_init(&missile_index_mutex, NULL);

    while (true) {
        bool has_collided_with_bridge = 
            (anti_aircraft -> sdl_obj.x < bridge.sdl_obj.x + bridge.sdl_obj.w) &&
            (anti_aircraft -> sdl_obj.x + anti_aircraft -> sdl_obj.w > bridge.sdl_obj.x);

        if (has_collided_with_bridge) {
            pthread_mutex_lock(&bridge_mutex);
            move_aircraft_out_of_bridge(anti_aircraft);
            pthread_mutex_unlock(&bridge_mutex);
        } else if (needs_to_reload) {
            int mutex_return = pthread_mutex_trylock(&left_building_mutex);
            is_left_building_occupied = mutex_return != 0;
            if (is_left_building_occupied) {
                move_aircrafts(anti_aircraft);
                SDL_Delay(10);
                continue;
            }

            move_to_left_building(anti_aircraft);
            reload_ammunition(&(anti_aircraft -> ammunition_sem));
            leave_building(anti_aircraft);
            pthread_mutex_unlock(&left_building_mutex);
        } else {
            move_aircrafts(anti_aircraft);
            SDL_Delay(10);
        }

        Uint32 now = SDL_GetTicks();
        if (now - anti_aircraft -> last_shot > time_between_shots) {
            int sem_return = sem_trywait(&(anti_aircraft -> ammunition_sem));
            needs_to_reload = sem_return == -1;
            if (needs_to_reload) {
                continue;
            }

            pthread_mutex_lock(&missile_index_mutex);
            int current_index = missile_index;
            missile_index = (missile_index + 1) % ammunition;
            pthread_mutex_unlock(&missile_index_mutex);

            pthread_t missile_thread;
            setup_missile(
                &(anti_aircraft -> missiles[current_index]),
                anti_aircraft
            );
            MissileThreadArgs missile_thread_args = {
                &(anti_aircraft -> missiles[current_index]),
                helicopter,
            };
            pthread_create(&missile_thread, NULL, missile_thread_func, &missile_thread_args);
            anti_aircraft -> last_shot = SDL_GetTicks();
        }
    }
    sem_destroy(&(anti_aircraft -> ammunition_sem));
    pthread_mutex_destroy(&missile_index_mutex);
    pthread_exit(NULL);
}

// --------------------RENDER GAME OBJECTS---------------------------

void render_game(Bridge bridge, Building buildings[], Hostage hostages[], AntiAircraft anti_aircrafts[], Helicopter helicopter) {
    SDL_SetRenderDrawColor(renderer, 52, 150, 235, 0.4);
    SDL_RenderClear(renderer);
    
    render_bridge(bridge);
    render_buildings(buildings);
    render_hostages(hostages);
    render_aircrafts(anti_aircrafts);
    render_helicopter(helicopter);

    SDL_RenderPresent(renderer);
}

// --------------------MAIN---------------------------

int main(int argc, char *argv[]) {
    init_difficulty_vars(atoi(argv[1]));
    game_is_running = initialize_window();

    pthread_mutex_init(&is_missile_active_mutex, NULL);
    pthread_mutex_init(&bridge_mutex, NULL);
    pthread_mutex_init(&left_building_mutex, NULL);

    Building buildings[NUM_OF_BUILDINGS];
    Hostage hostages[NUM_OF_INITIAL_HOSTAGES];
    setup_scenario(buildings, hostages);

    pthread_t helicopter_thread;
    Helicopter helicopter = setup_helicopter();
    HelicopterThreadArgs helicopter_thread_args = {
        &helicopter,
        buildings,
        hostages
    };
    pthread_create(&helicopter_thread, NULL, helicopter_thread_func, &helicopter_thread_args);

    AntiAircraft anti_aircrafts[NUM_OF_ANTI_AIRCRAFTS];    
    for (int i = 0; i < NUM_OF_ANTI_AIRCRAFTS; i++) {
        anti_aircrafts[i] = setup_aircraft(i);
    }

    pthread_t anti_aircraft_threads[NUM_OF_ANTI_AIRCRAFTS];
    AntiAircraftThreadArgs anti_aircraft_args[NUM_OF_ANTI_AIRCRAFTS];
    for (int i = 0; i < NUM_OF_ANTI_AIRCRAFTS; i++) {
        anti_aircraft_args[i].helicopter = &helicopter;
        anti_aircraft_args[i].anti_aircraft = &(anti_aircrafts[i]);
        pthread_create(&anti_aircraft_threads[i], NULL, anti_aircraft_thread, &anti_aircraft_args[i]);
    }

    while (game_is_running && !is_helicopter_destroyed && right_building_hostages < NUM_OF_INITIAL_HOSTAGES && !has_missile_collided_with_helicopter) {
        process_input();
        render_game(bridge, buildings, hostages, anti_aircrafts, helicopter);
    }

    destroy_window();

    pthread_cancel(helicopter_thread);
    for (int i = 0; i < NUM_OF_ANTI_AIRCRAFTS; i++) {
        free(anti_aircrafts[i].missiles);
        pthread_cancel(anti_aircraft_threads[i]);
    }

    pthread_mutex_destroy(&is_missile_active_mutex);
    pthread_mutex_destroy(&bridge_mutex);
    pthread_mutex_destroy(&left_building_mutex);
    return 0;
}
