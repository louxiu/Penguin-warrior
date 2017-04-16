#ifndef SCRIPTING_H
#define SCRIPTING_H

#include "gamedefs.h"

void InitScripting();
void CleanupScripting();
int LoadGameScript();
int RunGameScript();

#endif
