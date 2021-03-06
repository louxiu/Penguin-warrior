#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scripting.h"

extern player_t player, opponent;

void FirePhasers(player_p p);
int CanPlayerFire(player_p p);

void PrintPlayer(player_t *obj)
{
    fprintf(stdout, "obj->world_x = %f\n", obj->world_x);
    fprintf(stdout, "obj->world_y = %f\n", obj->world_y);
    fprintf(stdout, "obj->angle = %f\n", obj->angle);
    fprintf(stdout, "obj->accel = %f\n", obj->accel);
}

/* #define TCL_ENABLE */
#ifdef TCL_ENABLE

#include <tcl/tcl.h>

/* Our interpreter. This will be initialized by InitScripting. */
static Tcl_Interp *interp = NULL;

/* Prototype for the "fireWeapon" command handler. */
static int HandleFireWeaponCmd(ClientData client_data, Tcl_Interp * interp,
                               int objc, Tcl_Obj * CONST objv[]);

/* Sets up a Tcl interpreter for the game. Adds commands to implement our
   scripting interface. */
void InitScripting(void)
{

    /* First, create an interpreter and make sure it's valid. */
    interp = Tcl_CreateInterp();
    if (interp == NULL) {
        fprintf(stderr, "Unable to initialize Tcl.\n");
        /* TODO: should check return value */
        return;
    }

    /* Add the "fireWeapon" command. */
    if (Tcl_CreateObjCommand(interp, "fireWeapon",
                             HandleFireWeaponCmd, (ClientData) 0,
                             NULL) == NULL) {
        fprintf(stderr, "Error creating Tcl command.\n");
        return;
    }

    /* Link the important parts of our player data structures to global
       variables in Tcl. (Ignore the char * typecast; Tcl will treat the data
       as the requested type, in this case double.) */
    Tcl_LinkVar(interp, "player_state", (char *) &player.state,
                TCL_LINK_INT);
    Tcl_LinkVar(interp, "player_x", (char *) &player.world_x,
                TCL_LINK_DOUBLE);
    Tcl_LinkVar(interp, "player_y", (char *) &player.world_y,
                TCL_LINK_DOUBLE);
    Tcl_LinkVar(interp, "player_angle", (char *) &player.angle,
                TCL_LINK_DOUBLE);
    Tcl_LinkVar(interp, "player_accel", (char *) &player.accel,
                TCL_LINK_DOUBLE);

    Tcl_LinkVar(interp, "computer_state", (char *) &opponent.state,
                TCL_LINK_INT);
    Tcl_LinkVar(interp, "computer_x", (char *) &opponent.world_x,
                TCL_LINK_DOUBLE);
    Tcl_LinkVar(interp, "computer_y", (char *) &opponent.world_y,
                TCL_LINK_DOUBLE);
    Tcl_LinkVar(interp, "computer_angle", (char *) &opponent.angle,
                TCL_LINK_DOUBLE);
    Tcl_LinkVar(interp, "computer_accel", (char *) &opponent.accel,
                TCL_LINK_DOUBLE);

    /* Make the constants in gamedefs.h available to the script. The script
       should play by the game's rules, just like the human player.
       Tcl_SetVar2Ex is part of the Tcl_SetVar family of functions, which
       you can read about in the manpage. It simply sets a variable to a new
       value given by a Tcl_Obj structure. */
    Tcl_SetVar2Ex(interp, "world_width", NULL, Tcl_NewIntObj(WORLD_WIDTH),
                  0);
    Tcl_SetVar2Ex(interp, "world_height", NULL,
                  Tcl_NewIntObj(WORLD_HEIGHT), 0);
    Tcl_SetVar2Ex(interp, "player_forward_thrust", NULL,
                  Tcl_NewIntObj(PLAYER_FORWARD_THRUST), 0);
    Tcl_SetVar2Ex(interp, "player_reverse_thrust", NULL,
                  Tcl_NewIntObj(PLAYER_REVERSE_THRUST), 0);
}

/* Cleans up after our scripting system. */
void CleanupScripting(void)
{
    if (interp != NULL) {
        Tcl_DeleteInterp(interp);
    }

}

/* Executes a script in our customized interpreter. Returns 0 on success.
   Returns -1 and prints a message on standard error on failure.

   We'll use this to preload the procedures in the script. The interpreter's
   state is maintained after Tcl_EvalFile. We will NOT call Tcl_EvalFile after
   each frame - that would be hideously slow. */
int LoadGameScript()
{
    int status;

    status = Tcl_EvalFile(interp, "opponent.tcl");
    if (status != TCL_OK) {
        fprintf(stderr, "Error executing %s: %s\n", "opponent.tcl",
                Tcl_GetStringResult(interp));
        return -1;
    }

    return 0;
}

/* Handles "fireWeapon" commands from the Tcl script. */
static int HandleFireWeaponCmd(ClientData client_data, Tcl_Interp * interp,
                               int objc, Tcl_Obj * CONST objv[])
{
    /* Avoid compiler warnings. */
    objc += 0;
    objv += 0;
    client_data += 0;

    /* Fire, but only if weapons are charged. */
    if (CanPlayerFire(&opponent)) {
        FirePhasers(&opponent);
    }

    /* Return nothing (but make sure it's a valid nothing). */
    Tcl_ResetResult(interp);

    /* Succeed. On failure we would set a result with Tcl_SetResult and
       return TCL_ERROR. */
    return TCL_OK;
}

/* Runs the game script's update function (named "playComputer").
   Returns 0 on success, -1 on failure. */
int RunGameScript()
{
    int status;

    /* Call the script's update procedure. */
    status = Tcl_Eval(interp, "playComputer");
    if (status != TCL_OK) {
        fprintf(stderr, "Error in script: %s\n",
                Tcl_GetStringResult(interp));
        return -1;
    }

    /* Enforce limits on the script. It can still "cheat" by turning its ship
       more quickly than the player or by directly modifying its position
       variables, but that's not too much of a problem. We can more or less
       trust the script (it's part of the game). */
    if (opponent.accel > PLAYER_FORWARD_THRUST)
        opponent.accel = PLAYER_FORWARD_THRUST;
    if (opponent.accel < PLAYER_REVERSE_THRUST)
        opponent.accel = PLAYER_REVERSE_THRUST;
    while (opponent.angle >= 360)
        opponent.angle -= 360;
    while (opponent.angle < 0)
        opponent.angle += 360;

    return 0;
}
#endif

#ifdef SCM_ENABLE
#include <libguile.h>

/* TODO: use scm moudle and split scm script */
/* ERROR: In procedure apply: */
/* ERROR: Wrong type argument in position 1: #<unspecified> */
/* Try lua */
SCM HandleFireWeaponCmd()
{
    if (CanPlayerFire(&opponent)) {
        FirePhasers(&opponent);
    }
    return scm_from_int(0);
}

static Sint32 seed = 0;

/* No random function in scm. */
/* http://stackoverflow.com/questions/14674165/scheme-generate-random */
static void SchemeInitRandom()
{
    seed = time(NULL);
    srand(seed);
}

SCM SchemeRandom()
{
    return scm_from_double(rand()/(RAND_MAX +1.0));
}

int LoadGameScript()
{
    /* TODO: script name as constant */
    return 0;
}

SCM player_x_scm;
SCM player_y_scm;
SCM player_angle_scm;
SCM player_accel_scm;

SCM opponent_x_scm;
SCM opponent_y_scm;
SCM opponent_angle_scm;
SCM opponent_accel_scm;

SCM playOpponent;

void InitScripting()
{
    // TODO: move to LoadGameScript
    scm_init_guile();
    scm_c_primitive_load ("opponent.scm");

    /* Global variables initialize */
    SCM world_width_scm = scm_c_lookup( "world_width" ) ;
    scm_variable_set_x(world_width_scm, scm_int2num(WORLD_WIDTH));

    SCM world_height_scm = scm_c_lookup( "world_height" ) ;
    scm_variable_set_x(world_height_scm, scm_int2num(WORLD_HEIGHT));

    SCM player_forward_thrust_scm = scm_c_lookup( "player_forward_thrust" ) ;
    scm_variable_set_x(player_forward_thrust_scm, scm_int2num(PLAYER_FORWARD_THRUST));

    SCM player_reverse_thrust_scm = scm_c_lookup( "player_reverse_thrust" ) ;
    scm_variable_set_x(player_reverse_thrust_scm, scm_int2num(PLAYER_REVERSE_THRUST));

    /* TODO: use func or loop to init player and opponnent */
    /* player */
    player_x_scm = scm_c_lookup( "player_x" ) ;
    player_y_scm = scm_c_lookup( "player_y" ) ;
    player_angle_scm = scm_c_lookup( "player_angle" ) ;
    player_accel_scm = scm_c_lookup( "player_accel" ) ;

    /* opponent */
    opponent_x_scm = scm_c_lookup( "opponent_x" ) ;
    opponent_y_scm = scm_c_lookup( "opponent_y" ) ;
    opponent_angle_scm = scm_c_lookup( "opponent_angle" ) ;
    opponent_accel_scm = scm_c_lookup( "opponent_accel" ) ;

    /* TODO: rename function */
    playOpponent = scm_variable_ref(scm_c_lookup("playOpponent"));

    SchemeInitRandom();
    /* TODO: rename */
    scm_c_define_gsubr( "random", 0, 0, 0, SchemeRandom);
    scm_c_define_gsubr( "fireWeapon", 0, 0, 0, HandleFireWeaponCmd);
}

int RunGameScript()
{
    /// Update the variables in scheme script.
    scm_variable_set_x(player_x_scm, scm_int2num(player.world_x));
    scm_variable_set_x(player_y_scm, scm_int2num(player.world_y));
    scm_variable_set_x(player_angle_scm, scm_int2num(player.angle));
    scm_variable_set_x(player_accel_scm, scm_int2num(player.accel));

    scm_variable_set_x(opponent_x_scm, scm_int2num(opponent.world_x));
    scm_variable_set_x(opponent_y_scm, scm_int2num(opponent.world_y));
    scm_variable_set_x(opponent_angle_scm, scm_int2num(opponent.angle));
    scm_variable_set_x(opponent_accel_scm, scm_int2num(opponent.accel));

    scm_call_0(playOpponent);

    /* TODO: Guile's deprecation warnings */
    /* update opponent accel and angle */
    opponent.angle= scm_num2int(scm_variable_ref(opponent_angle_scm), 0, NULL);
    opponent.accel= scm_num2int(scm_variable_ref(opponent_accel_scm), 0, NULL);

    if (opponent.accel > PLAYER_FORWARD_THRUST){
        opponent.accel = PLAYER_FORWARD_THRUST;
    }
    if (opponent.accel < PLAYER_REVERSE_THRUST){
        opponent.accel = PLAYER_REVERSE_THRUST;
    }

    /* TODO: keep angle ok */
    while (opponent.angle >= 360){
        opponent.angle -= 360;
    }
    while (opponent.angle < 0){
        opponent.angle += 360;
    }

    return 0;
}

void CleanupScripting()
{
    // TODO: Do not need in guile?
}
#endif

#define LUA_ENABLE
#ifdef LUA_ENABLE

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

typedef struct {
    char *name;
    int   type;
    void *value;
} table_field;

static void SetFields(lua_State *L, int index, const table_field *fields) {
    for (int i = 0; fields[i].name; i++) {
        table_field f = fields[i];
        switch (f.value == NULL ? LUA_TNIL : f.type) {
            case LUA_TFUNCTION:
                lua_pushcfunction(L, (lua_CFunction) f.value);
                break;
            case LUA_TNUMBER:
                lua_pushnumber(L, *((lua_Number *) f.value));
                break;
            case LUA_TSTRING:
                lua_pushstring(L, (const char *) f.value);
                break;
            case LUA_TNIL:
                lua_pushnil(L);
                break;
        }
        lua_setfield(L, index, f.name);
    }
}

int HandleFireWeaponCmd(lua_State *L) {
    (void) L;
    if (CanPlayerFire(&opponent)) {
        FirePhasers(&opponent);
    }
	return 0;
}

lua_State *L = NULL;

void InitScripting()
{
    L = luaL_newstate();
    luaL_openlibs(L);

    char *file = "opponent.lua";
    if (luaL_dofile(L, file)) {
        const char *cause = lua_tostring(L, -1);
        fprintf(stderr, "%s: %s\n", file, cause);
        return;
    }

    /* Global variables initialize */
    double world_width = WORLD_WIDTH;
    double world_height = WORLD_HEIGHT;
    const table_field world_fields[] = {
        { "width", LUA_TNUMBER, &world_width },
        { "height", LUA_TNUMBER, &world_height },
        { NULL,       0,           NULL      },
    };
    lua_getglobal(L, "world");
    SetFields(L, 1, world_fields);

    double forward_thrust = PLAYER_FORWARD_THRUST;
    double reverse_thrust = PLAYER_REVERSE_THRUST;
    const table_field thrust_fields[] = {
        { "forward", LUA_TNUMBER, &forward_thrust },
        { "reverse", LUA_TNUMBER, &reverse_thrust },
        { NULL,       0,           NULL      },
    };
    lua_getglobal(L, "player");
    lua_getfield(L, 2, "thrust");
    SetFields(L, 3, thrust_fields);

    lua_getglobal(L, "opponent");
    lua_getfield(L, 4, "thrust");
    SetFields(L, 5, thrust_fields);

    lua_pop(L, 5);

	lua_register(L, "fireWeapon", HandleFireWeaponCmd);
    // printf("lua_gettop = %d\n", lua_gettop(L));
}

void CleanupScripting()
{
    lua_close(L);
}

int LoadGameScript()
{
    /* TODO: */
    return 0;
}

int RunGameScript()
{
    /* fprintf(stdout, "-------player-------\n"); */
    /* PrintPlayer(&player); */
    /* fprintf(stdout, "-------opponent-------\n"); */
    /* PrintPlayer(&opponent); */

    // Update the variables in scheme script.
    const table_field player_fields[] = {
        { "x", LUA_TNUMBER, &player.world_x },
        { "y", LUA_TNUMBER, &player.world_y },
        { "angle", LUA_TNUMBER, &player.angle },
        { "accel", LUA_TNUMBER, &player.accel },
        { NULL,       0,           NULL      },
    };
    lua_getglobal(L, "player");
    SetFields(L, 1, player_fields);

    const table_field opponent_fields[] = {
        { "x", LUA_TNUMBER, &opponent.world_x },
        { "y", LUA_TNUMBER, &opponent.world_y },
        { "angle", LUA_TNUMBER, &opponent.angle },
        { "accel", LUA_TNUMBER, &opponent.accel },
        { NULL,       0,           NULL      },
    };
    lua_getglobal(L, "opponent");
    SetFields(L, 2, opponent_fields);

    lua_getglobal(L, "playOpponent");
    if (lua_pcall(L, 0, 0, 0))
    {
        fprintf(stderr, "FATAL ERROR: %s\n", lua_tostring(L, -1));
        return 1;
    }

    /* update opponent accel and angle */
    lua_getfield(L, 2, "angle");
    opponent.angle = lua_tonumber(L, -1);
    lua_getfield(L, 2, "accel");
    opponent.accel = lua_tonumber(L, -1);
    lua_pop(L, 4);

    if (opponent.accel > PLAYER_FORWARD_THRUST){
        opponent.accel = PLAYER_FORWARD_THRUST;
    }
    if (opponent.accel < PLAYER_REVERSE_THRUST){
        opponent.accel = PLAYER_REVERSE_THRUST;
    }

    while (opponent.angle >= 360){
        opponent.angle -= 360.0;
    }
    while (opponent.angle < 0){
        opponent.angle += 360;
    }

    return 0;
}
#endif
