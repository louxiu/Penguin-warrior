#ifndef GAMEDEFS_H
#define GAMEDEFS_H

#include <SDL/SDL.h>
#include <AL/al.h>

/* Dimensions of the game window. */
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
/* #define SCREEN_WIDTH	800 */
/* #define SCREEN_HEIGHT	600 */

/* Number of particles allowed in the particle system. */
#define MAX_PARTICLES	30000

/* Dimensions of the player's ship. The graphics data must correspond to this. */
#define PLAYER_WIDTH	96
#define PLAYER_HEIGHT	96

/* Dimensions of the map tiles. */
#define TILE_WIDTH	64
#define TILE_HEIGHT	64

/* Total size (in pixels) of the complete playfield. */
#define WORLD_WIDTH	  2000
#define WORLD_HEIGHT  2000

/* Limits on the player. */
#define PLAYER_MAX_VELOCITY	(15.0)
#define PLAYER_MIN_VELOCITY	(-10.0)

/* Limits on the devil. */
#define DEVIL_MAX_VELOCITY	(13.0)
#define DEVIL_MIN_VELOCITY	(-10.0)

#define PLAYER_FORWARD_THRUST	(3)
#define PLAYER_REVERSE_THRUST	(-1)

/* These define the scrolling speeds of the front and back background
   layers, relative to the movement of the camera. */
#define PARALLAX_BACK_FACTOR		4
#define PARALLAX_FRONT_FACTOR		2

/* These define the sizes of the backgrond tile grids. We don't really need a one to one
   mapping between the size of the playing field and the size of the tile grids;
   we can wrap around at some point, and nobody will notice a difference. */
#define PARALLAX_GRID_WIDTH		100
#define PARALLAX_GRID_HEIGHT	100

/* Amount of damage a single shot does.  warrior*/
#define PHASER_DAMAGE                   16

/* Amount of damage a single shot does. devil*/
#define PHASER_DAMAGE_DEVIL             1

/* Charge level required to fire. */
#define PHASER_CHARGE_FIRE              10

/* Maximum charge the phasers can hold. */
#define PHASER_CHARGE_MAX               30

/* Roughly the number of charge units per second. */
#define PHASER_CHARGE_RATE              30

/* Duration of each phaser shot, in ticks. (30 ticks/second) */ 
#define PHASER_FIRE_TIME                5

/* Time to delay before respawning, in ticks. */
#define RESPAWN_TIME                    60

/* Time the player is invincible, */
#define INVINCIBLE_TIME                 300

#define PI (3.141592654F)

/* Our time scaling factor. This is defined in main.c, but
   it needs to be accessible to the other game modules. */
extern double time_scale;

typedef enum {WARRIOR, DEVIL} player_type;
typedef enum {ENVADE, ATTACK, UNDER_ATTACK, INVINCIBLE, DEAD, DEADING} player_state;

/* Data structure for player ships */
typedef struct player_s
{
    player_type type;
    player_state state;
    double angle;		    /* in clockwise degrees, 0 -> due east */
    double world_x,world_y;	/* coordinates in the world */
    int screen_x,screen_y;	/* onscreen coordinates */
    double velocity;    	/* velocity in pixels per frame */
    double accel;		    /* acceleration in pixels/frame^2 */
    int shields;		    /* shield strength left */
    double firing;          /* phaser is currently active */
    double charge;          /* phaser charge level */
    int score;              /* kill counter */
    int hit;                /* player hit, cause particle burst */
} player_t, *player_p;

/* Data structure for audio resources */
typedef struct sound_s
{
    ALint format;           /* OpenAL format enum, such as AL_FORMAT_STEREO8. */
    ALuint size;            /* size, in bytes, of this buffer */
    ALfloat freq;           /* frequency */
    ALuint name;            /* OpenAL buffer name. */
    /* The actual sound data is stored within OpenAL. */
} sound_t, *sound_p;

#endif

