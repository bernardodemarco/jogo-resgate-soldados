#define WINDOW_WIDTH 1650
#define WINDOW_HEIGHT 800

#define BRIDGE_WIDTH 500
#define BRIDGE_HEIGHT 8
#define BRIDGE_Y (WINDOW_HEIGHT - BRIDGE_HEIGHT)
#define BRIDGE_X (WINDOW_WIDTH / 2) - (BRIDGE_WIDTH / 2)

#define NUM_OF_ANTI_AIRCRAFTS 2
#define ANTI_AIRCRAFT_WIDTH 125
#define ANTI_AIRCRAFT_HEIGHT 75
#define ANTI_AIRCRAFT_Y WINDOW_HEIGHT - ANTI_AIRCRAFT_HEIGHT

#define HELICOPTER_WIDTH 175
#define HELICOPTER_HEIGHT 75
#define HELICOPTER_Y 15
#define HELICOPTER_X 15
#define HELICOPTER_VELOCITY 5

#define NUM_OF_BUILDINGS 2
#define BUILDING_WIDTH 200
#define BUILDING_HEIGHT 500
#define BUILDING_Y WINDOW_HEIGHT - BUILDING_HEIGHT
#define LEFT_BUILDING_X 0
#define RIGHT_BUILDING_X WINDOW_WIDTH - BUILDING_WIDTH

#define NUM_OF_INITIAL_HOSTAGES 10
#define HOSTAGE_WIDTH 10
#define HOSTAGE_HEIGHT 50
#define HOSTAGE_Y WINDOW_HEIGHT - BUILDING_HEIGHT - HOSTAGE_HEIGHT

#define TIME_BETWEEN_SHOTS 500
#define MISSILE_SIZE 30
#define AMMUNITION 10

#define SHORT_RELOAD_TIME 100
#define MEDIUM_RELOAD_TIME 250
#define LONG_RELOAD_TIME 500
