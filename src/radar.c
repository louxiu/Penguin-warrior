#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gamedefs.h"

typedef enum {on, off} icon_state;

typedef struct radar_Display {
    SDL_Surface *radar_surface;
    int physic_w, physic_h;
    int physic_x, physic_y;
    SDL_Surface *player_icon;
    int opp_icon_state;
    SDL_Surface *opp_icon_on;
    SDL_Surface *opp_icon_off;            
} radar_Display;

radar_Display radar;

int InitRadarDisplay(void)
{
    memset(&radar, 0, sizeof (radar_Display));

    if ((radar.radar_surface = SDL_LoadBMP("radar.bmp")) == NULL)
    {
        printf ("Can not load radar.bmp\n");
        return -1;
    }
    /// Filter the Black ground
    Uint32 colorkey = SDL_MapRGB((radar.radar_surface)->format, 0, 0, 0);
    SDL_SetColorKey(radar.radar_surface, SDL_SRCCOLORKEY, colorkey);
    SDL_SetAlpha(radar.radar_surface, SDL_SRCALPHA, 128);
    
    radar.physic_w = 100;
    radar.physic_h = 100;

    radar.physic_x = 0;
    radar.physic_y = SCREEN_HEIGHT - 100;

    if ((radar.player_icon = SDL_LoadBMP("led-green-on.bmp")) == NULL)
    {
        printf ("Can not load on.bmp\n");
        return -1;
    }

    if ((radar.opp_icon_on = SDL_LoadBMP("led-red-on.bmp")) == NULL)
    {
        printf ("Can not load off.bmp\n");
        return -1;
    }

    if ((radar.opp_icon_off = SDL_LoadBMP("led-red-off.bmp")) == NULL)
    {
        printf ("Can not load off.bmp\n");
        return -1;
    }

    radar.opp_icon_state = 0;
    
    return 0;
}

void CleanupRadarDisplay(void)
{
    if (radar.radar_surface != NULL)
        SDL_FreeSurface(radar.radar_surface);
}

double distance(double x, double y, double v, double w)
{
    return sqrt((x -v) * (x -v) + (y - w) * (y - w));
}

void UpdateRadarDisplay(SDL_Surface *screen, int player_x, int player_y, int opp_x, int opp_y)
{
    /// Draw player and opp icon
    SDL_Rect srcrect, destrect;

    srcrect.w = radar.player_icon->w;
    srcrect.h = radar.opp_icon_on->h;
    srcrect.x = 0;
    srcrect.y = 0;
    destrect = srcrect;

    /// Scale the x, y
    destrect.x = player_x / (WORLD_WIDTH /100) + radar.physic_x;
    destrect.y = player_y / (WORLD_HEIGHT / 100) + radar.physic_y;


    if (distance(player_x / (WORLD_WIDTH /100), player_y / (WORLD_WIDTH /100), 50, 50) < 50)
    {
        /// Draw player icon
        SDL_BlitSurface(radar.player_icon, &srcrect,
                        screen, &destrect);
    }    
    destrect.x = opp_x / (WORLD_WIDTH /100) + radar.physic_x;
    destrect.y = opp_y / (WORLD_HEIGHT / 100) + radar.physic_y;

    if (distance(opp_x / (WORLD_WIDTH /100), opp_y / (WORLD_WIDTH /100), 50, 50) < 50)
    {
        /// Draw opponent icon
        if (radar.opp_icon_state < 10)
        {
            SDL_BlitSurface(radar.opp_icon_on, &srcrect,
                            screen, &destrect);
            radar.opp_icon_state ++;
        }
        else if (radar.opp_icon_state <= 20)
        {
            SDL_BlitSurface(radar.opp_icon_off, &srcrect,
                            screen, &destrect);
            radar.opp_icon_state ++;
            if (radar.opp_icon_state == 20)
            {
                radar.opp_icon_state = 0;
            }
        }
    }
    SDL_Rect src, dest;

    src.x = 0;
    src.y = 0;
    src.w = radar.physic_w;
    src.h = radar.physic_h;

    dest.x = radar.physic_x;
    dest.y = radar.physic_y;
    dest.w = radar.physic_w;
    dest.h = radar.physic_h;

    /// Draw radar
    SDL_BlitSurface(radar.radar_surface, &src,
                    screen, &dest);
}

