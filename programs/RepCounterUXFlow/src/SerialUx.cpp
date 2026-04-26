#include "UX.h"
#include "SerialUx.h"
#include <stdio.h>
#include <stdlib.h>

static int getMenuItemCount(const MenuItem* menu) {
    int count = 0;
    while (count < MAX_MENU_ITEMS && menu->items[count] != NULL) {
        count++;
    }
    return count;
}
 
// Menu definitions  
struct MenuItem goBackMenuItem = {
    .name = "Go Back",
    .action = itemGoBack,  // Will be set after function is defined
    .currentItemSelection = 0,
    .items = { NULL }
};

struct MenuItem downloadDataMenu = {
    .name = "Download Data",
    .action = NULL,
    .currentItemSelection = 0,
    .items = {  &goBackMenuItem, NULL }
};

struct MenuItem recordDataMenu = {
    .name = "Record Data",
    .action = NULL,
    .currentItemSelection = 0,
    .items = { &goBackMenuItem, NULL }
};

struct MenuItem mainMenu = {
    .name = "Main Menu",
    .action = NULL,
    .currentItemSelection = 0,
    .items = { &downloadDataMenu, &recordDataMenu, NULL }
};

UXState* initializeUXState(void* buf, UXDevice device){
    ((UXState*) buf)->device = device;
    ((UXState*) buf)->currentMenu = &mainMenu;
    ((UXState*) buf)->previousMenu = NULL;
    goBackMenuItem.action = itemGoBack;  // Set the function pointer to the itemGoBack function defined in UX.cpp
    add_workouts_to_menu(&recordDataMenu); // Add workouts to the record data menu
    return (UXState*) buf;
}

void displayMenu(UXState* state){
    char buffer[128];
    int itemCount = getMenuItemCount(state->currentMenu);
    
    sprintf(buffer, "%s\n", state->currentMenu->name);
    Serial.print(buffer);
    
    if (itemCount == 0) {
        Serial.print("(empty)\n");
    } else {
        for (int i = 0; i < itemCount; i++) {
            sprintf(buffer, "%c %d. %s\n",
                    (i == state->currentMenu->currentItemSelection) ? '>' : ' ',
                    i + 1,
                    state->currentMenu->items[i]->name);
            Serial.print(buffer);
        }
    }
    Serial.print("Use w and s to navigate, r to repeat menu, and enter to choose an option.\n");
}

void adjustCursorPosition(UXState* state){
    displayMenu(state);
}



void downloadDataAction(UXState* state){
    Serial.print("Downloading data...\n");
    // Simulate download action here
    Serial.print("Data downloaded successfully!\n");
}

void recordWorkoutAction(UXState* state){
    Serial.print("Recording workout...\n");
    // Simulate workout recording action here
    Serial.print("Workout recorded successfully!\n");
}

// Set the itemGoBack action after the function is defined in UX.cpp
// This will be done in initializeUXState

void handle_input(UXState* state){
    if (Serial.available() == 0) {
        return;
    }

    char input = Serial.read();
    if (input >= 'A' && input <= 'Z') {
        input = input - 'A' + 'a';
    }

    switch(input){
        case 'w': // Up
            goUpMenuItem(state);
            break;
        case 's': // Down
            goDownMenuItem(state);
            break;
        case 'r': //display menu again
            displayMenu(state);
            break;
        case '\r':
            if (Serial.peek() == '\n') {
                Serial.read();
            }
            selectMenuItem(state);
            break;
        case '\n': // Enter
            selectMenuItem(state);
            break;
        case ' ':
            break;
        default:
            Serial.print("Invalid input. Use 'w' to go up, 's' to go down, and 'Enter' to select.\n");
    }
}

void add_workouts_to_menu(struct MenuItem* menu) {
    // Code to add workouts to the menu
    const char *workouts[] = WORKOUTS;
    const size_t workoutCount = sizeof(workouts) / sizeof(workouts[0]);
    const size_t maxWorkoutSlots = MAX_MENU_ITEMS - 2; // reserve index 0 for "Go Back" and one slot for NULL

    for (size_t i = 0; i < workoutCount && i < maxWorkoutSlots; i++) {
        menu->items[i + 1] = (struct MenuItem*)malloc(sizeof(struct MenuItem));
        menu->items[i + 1]->name = workouts[i];
        menu->items[i + 1]->action = recordWorkoutAction;
        menu->items[i + 1]->currentItemSelection = 0;
        for (int j = 0; j < MAX_MENU_ITEMS; j++) {
            menu->items[i + 1]->items[j] = NULL;
        }
    }
    menu->items[(workoutCount < maxWorkoutSlots ? workoutCount : maxWorkoutSlots) + 1] = NULL;
}
