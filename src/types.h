typedef struct {
    SDL_Rect sdl_obj;
    bool is_active;
} Missile;

typedef struct {
    int id;
    int velocity;
    sem_t ammunition_sem;
    Uint32 last_shot;
    Missile *missiles;
    SDL_Rect sdl_obj;
} AntiAircraft;

typedef struct {
    int velocity;
    SDL_Rect sdl_obj;
} Helicopter;

typedef struct {
    SDL_Rect sdl_obj;
} Bridge;

typedef struct {
    SDL_Rect sdl_obj;
} Building;

typedef struct {
    SDL_Rect sdl_obj;
} Hostage;

typedef struct {
    Helicopter *helicopter;
    Building *buildings;
    Hostage *hostages;
} HelicopterThreadArgs;

typedef struct {
    AntiAircraft *anti_aircraft;
    Helicopter *helicopter;
} AntiAircraftThreadArgs;

typedef struct {
    Missile *missile;
    Helicopter *helicopter;
} MissileThreadArgs;
