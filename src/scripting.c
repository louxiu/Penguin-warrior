#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scripting.h"

extern player_t player, opponent;

#define TCL_ENABLE
#ifdef TCL_ENABLE

#include <tcl.h>

/* Our interpreter. This will be initialized by InitScripting. */
static Tcl_Interp *interp = NULL;

/* Prototype for the "fireWeapon" command handler. */
static int HandleFireWeaponCmd(ClientData client_data, Tcl_Interp * interp,
                               int objc, Tcl_Obj * CONST objv[]);

/* Ship data structures (from main.c). */
extern player_t player, opponent;

/* Phaser handling routines (from main.c). */
void FirePhasers(player_p p);
int CanPlayerFire(player_p p);

/* Sets up a Tcl interpreter for the game. Adds commands to implement our
   scripting interface. */
void InitScripting(void)
{

    /* First, create an interpreter and make sure it's valid. */
    interp = Tcl_CreateInterp();
    if (interp == NULL) {
        fprintf(stderr, "Unable to initialize Tcl.\n");
        exit(EXIT_FAILURE);
    }

    /* Add the "fireWeapon" command. */
    if (Tcl_CreateObjCommand(interp, "fireWeapon",
                             HandleFireWeaponCmd, (ClientData) 0,
                             NULL) == NULL) {
        fprintf(stderr, "Error creating Tcl command.\n");
        exit(EXIT_FAILURE);
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
int LoadGameScript(char *filename)
{
    int status;

    status = Tcl_EvalFile(interp, filename);
    if (status != TCL_OK) {
        fprintf(stderr, "Error executing %s: %s\n", filename,
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

#else /* TCL_ENABLE */
#include <libguile.h>

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

static void scheme_init_random()
{
    seed = time(NULL);
    srand(seed);
}

SCM scheme_random()
{
    return scm_from_double(rand()/(RAND_MAX +1.0));
}

int LoadGameScript(char *filename)
{
    scm_init_guile();
    scm_c_primitive_load ("opponent.scm");

    return 0;
}

SCM current_module;
void InitScripting(void)
{
    scm_init_guile();
    /* current_module = scm_c_define_module("test", NULL, NULL); */
    /* scm_c_use_module("test"); */
    /* current_module = scm_current_module(); */
    scm_c_primitive_load ("opponent.scm");
    
    /// Global variables initialize
    /* scm_c_define("world_width", scm_int2num(WORLD_WIDTH)); */
	/* scm_c_define("world_height", scm_int2num(WORLD_HEIGHT)); */
	/* scm_c_define("player_forward_thrust", scm_int2num(PLAYER_FORWARD_THRUST)); */
	/* scm_c_define("player_reverse_thrust", scm_int2num(PLAYER_REVERSE_THRUST)); */

    /* scheme_init_random(); */
    /* scm_c_define_gsubr( "random", 0, 0, 0, scheme_random);     */
    /* scm_c_define_gsubr( "fireWeapon", 0, 0, 0, HandleFireWeaponCmd); */
}
/* start state is attack 0
 * state evade 1*/
int state = 0;
int RunGameScript()
{
    /* scm_c_use_module("test"); */
    /// scm_set_current_module(current_module);

    /* confused with guile's module and level.
     * This is just a ad-hoc. Needs to change here */
    
    /// Global variables initialize
    scm_c_define("world_width", scm_int2num(WORLD_WIDTH));
	scm_c_define("world_height", scm_int2num(WORLD_HEIGHT));
	scm_c_define("player_forward_thrust", scm_int2num(PLAYER_FORWARD_THRUST));
	scm_c_define("player_reverse_thrust", scm_int2num(PLAYER_REVERSE_THRUST));

    scheme_init_random();
    scm_c_define_gsubr("random", 0, 0, 0, scheme_random);    
    scm_c_define_gsubr("fireWeapon", 0, 0, 0, HandleFireWeaponCmd);
    
    /// Update the variables in scheme script.
    scm_c_define("state", scm_int2num(state));    
    scm_c_define("player_x", scm_double2num(player.world_x));
	scm_c_define("player_y", scm_double2num(player.world_y));
	scm_c_define("player_angle", scm_double2num(player.angle));
	scm_c_define("player_accel", scm_double2num(player.accel));

    scm_c_define("computer_x", scm_double2num(opponent.world_x));
	scm_c_define("computer_y", scm_double2num(opponent.world_y));
	scm_c_define("computer_angle", scm_double2num(opponent.angle));
	scm_c_define("computer_accel", scm_double2num(opponent.accel));

    /// For test
    SCM display = scm_variable_ref(scm_c_lookup("display_vars"));
    scm_call_0(display);

    /// Get back all variables    
    SCM s_symbol, s_value;
    s_symbol = scm_c_lookup("state");
    s_value = scm_variable_ref(s_symbol);
    if (SCM_NUMBERP(s_value)) {
        state = scm_num2int (s_value, 0, "state");
        printf ("state = %d\n", state);
    }
    s_symbol = scm_c_lookup("player_x");
    s_value = scm_variable_ref(s_symbol);
    if (SCM_NUMBERP(s_value)) {
         player.world_x = scm_num2double (s_value, 0, "player_x");
        printf ("player_x = %.1f\n", player.world_x);
    }
    s_symbol = scm_c_lookup("player_y");
    s_value = scm_variable_ref(s_symbol);
    if (SCM_NUMBERP(s_value)) {
         player.world_y = scm_num2double (s_value, 0, "player_y");
        printf ("player_y = %.1f\n", player.world_y);
    }
    s_symbol = scm_c_lookup("player_angle");
    s_value = scm_variable_ref(s_symbol);
    if (SCM_NUMBERP(s_value)) {
         player.angle = scm_num2double (s_value, 0, "player_angle");
        printf ("player_angle = %.1f\n", player.angle);
    }
    s_symbol = scm_c_lookup("player_accel");
    s_value = scm_variable_ref(s_symbol);
    if (SCM_NUMBERP(s_value)) {
         player.accel = scm_num2double (s_value, 0, "player_accel");
        printf ("player_accel = %.1f\n", player.accel);
    }

    s_symbol = scm_c_lookup("computer_x");
    s_value = scm_variable_ref(s_symbol);
    if (SCM_NUMBERP(s_value)) {
         opponent.world_x = scm_num2double (s_value, 0, "computer_x");
        printf ("computer_x = %.1f\n", opponent.world_x);
    }
    s_symbol = scm_c_lookup("computer_y");
    s_value = scm_variable_ref(s_symbol);
    if (SCM_NUMBERP(s_value)) {
         opponent.world_y = scm_num2double (s_value, 0, "computer_y");
        printf ("computer_y = %.1f\n", opponent.world_y);
    }
    s_symbol = scm_c_lookup("computer_angle");
    s_value = scm_variable_ref(s_symbol);
    if (SCM_NUMBERP(s_value)) {
         opponent.angle = scm_num2double (s_value, 0, "computer_angle");
        printf ("computer_angle = %.1f\n", opponent.angle);
    }
    s_symbol = scm_c_lookup("computer_accel");
    s_value = scm_variable_ref(s_symbol);
    if (SCM_NUMBERP(s_value)) {
         opponent.accel = scm_num2double (s_value, 0, "computer_accel");
        printf ("computer_accel = %.1f\n", opponent.accel);
    }    
    
    /// exit(0);
    
    SCM playComputer;    
    playComputer = scm_variable_ref(scm_c_lookup("playComputer"));
    scm_call_0(playComputer);
    
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

void CleanupScripting(void)
{
    /// Do not need in guile.
}

#endif 
