#ifndef SERIALUX_H
#define SERIALUX_H

#include "UX.h"

#define WORKOUTS {"Push-ups"}

// External declarations for menus defined in SerialUx.cpp
extern struct MenuItem mainMenu;
extern struct MenuItem downloadDataMenu;
extern struct MenuItem recordDataMenu;
extern struct MenuItem goBackMenuItem;

void downloadDataAction(UXState* state);
void recordWorkoutAction(UXState* state);
void add_workouts_to_menu(struct MenuItem* menu);
void handle_input(UXState* state);

#endif


