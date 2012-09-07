#ifndef RADAR_H
#define RADAR_H

#include <SDL/SDL.h>
#include "gamedefs.h"

int InitRadarDisplay(void);
/* Initializes the radar display system.
   Returns 0 on success, -1 on failure.
   Must be called after setting a video mode. */

void CleanupRadarDisplay(void);
/* Shuts down the status display system. */

void UpdateRadarDisplay(SDL_Surface *screen, int player_x, int player_y, int opp_x, int opp_y);
/* Updates and redraws the radar display. */

#endif
