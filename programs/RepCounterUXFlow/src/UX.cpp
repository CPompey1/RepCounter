
#include "UX.h"

static int getMenuItemCount(const MenuItem* menu) {
    int count = 0;
    while (count < MAX_MENU_ITEMS && menu->items[count] != NULL) {
        count++;
    }
    return count;
}

void goDownMenuItem(UXState* state){
    const int itemCount = getMenuItemCount(state->currentMenu);
    if (itemCount == 0) {
        return;
    }

    if (state->currentMenu->currentItemSelection + 1 < itemCount) {
        state->currentMenu->currentItemSelection++;
    } else {
        state->currentMenu->currentItemSelection = 0; // Wrap around to the first item
    }
    adjustCursorPosition(state);
}

void goUpMenuItem(UXState* state){
    const int itemCount = getMenuItemCount(state->currentMenu);
    if (itemCount == 0) {
        return;
    }

    if(state->currentMenu->currentItemSelection > 0){
        state->currentMenu->currentItemSelection--;
    }else {
        state->currentMenu->currentItemSelection = itemCount - 1;
    }
    adjustCursorPosition(state);
}

void selectMenuItem(UXState* state){
    const int itemCount = getMenuItemCount(state->currentMenu);
    if (itemCount == 0) {
        displayMenu(state);
        return;
    }

    struct MenuItem* selectedItem = state->currentMenu->items[state->currentMenu->currentItemSelection];
    if(selectedItem != NULL){
        if(selectedItem->action != NULL){
            selectedItem->action(state);
            displayMenu(state);
        } else if(selectedItem->items[0] != NULL){
            // Navigate to the submenu
            state->previousMenu = state->currentMenu;
            state->currentMenu = selectedItem;
            if (state->currentMenu->currentItemSelection >= getMenuItemCount(state->currentMenu)) {
                state->currentMenu->currentItemSelection = 0;
            }
            displayMenu(state);
        } else {
            displayMenu(state);
        }
    } else {
        displayMenu(state);
    }
}

void itemGoBack(UXState* state){
    if(state->previousMenu != NULL){
        state->currentMenu = state->previousMenu;
        state->previousMenu = NULL; // Clear previous menu reference
    }
}
