#ifndef SCRIPTING_H
#define SCRIPTING_H

#include "gamedefs.h"

void InitScripting(void);
void CleanupScripting(void);
int LoadGameScript();
int RunGameScript(void);

#endif
