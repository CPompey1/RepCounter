#ifndef UX_H
#define UX_H
#define MAX_MENU_ITEMS 10
#include <Arduino.h>

typedef enum UXDevice {
    UX_SERIAL,
    UX_PHONE,
    UX_LCD
} UXDevice;

// Forward declaration
typedef struct UXState UXState;

typedef struct MenuItem {
    const char* name;
    void (*action)(UXState* state);
    int currentItemSelection;
    struct MenuItem* items[MAX_MENU_ITEMS];

} MenuItem;

typedef struct UXState {
    UXDevice device;
    struct MenuItem* currentMenu;
    struct MenuItem* previousMenu;

} UXState;

void itemGoBack(UXState* state);
void adjustCursorPosition(UXState* state);
UXState* initializeUXState(void* buf, UXDevice device);
void displayMenu(UXState* state);
void goDownMenuItem(UXState* state);
void goUpMenuItem(UXState* state);
void selectMenuItem(UXState* state);    

#endif
