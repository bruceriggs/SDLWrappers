#include "SDLInput.h"
#include <cstring>

SDLInput::SDLInput() {
    
}

// Destructor
SDLInput::~SDLInput() {
    
}

// Instance
SDLInput* SDLInput::Instance() {
    // Lazy instantiation
    static SDLInput instance;
    return &instance;
}

// Initializes the Input
void SDLInput::Init() {
    // Wipe out the keys
    keys.clear();
    prevKeys.clear();
    memset(buttons, 0, sizeof(bool) * NUMBUTTONS);
    memset(prevButtons, 0, sizeof(bool) * NUMBUTTONS);
    mods = prevMods = KMOD_NONE;
    doubleClicked = false;
    mousePos.x = mousePos.y = 0;
    quit = false;
    anyKeyDown = anyKeyPress = anyKeyRelease = false;
    // Start the class by polling the input at least once
    Update();
}

// Shuts down the input
void SDLInput::Shutdown() {
    // Wipe out the keys
    keys.clear();
    prevKeys.clear();
    memset(buttons, 0, sizeof(bool) * NUMBUTTONS);
    memset(prevButtons, 0, sizeof(bool) * NUMBUTTONS);
    mods = prevMods = KMOD_NONE;
    doubleClicked = false;
    mousePos.x = mousePos.y = 0;
    quit = false;
    anyKeyDown = anyKeyPress = anyKeyRelease = false;
}

// Updates the controls
void SDLInput::Update() {
    // Update the previous keys/buttons first
    prevKeys.clear();
    prevKeys = keys;
    anyKeyDown = anyKeyPress = anyKeyRelease = false;
    memcpy(prevButtons, buttons, sizeof(bool) * NUMBUTTONS);
    prevMods = mods;

    // Check the controls
    SDL_PollEvent(&controlEvent);

    // Handle the events
    switch (controlEvent.type)
    {
        case SDL_KEYDOWN:
        {
            // Key DOWN bool
            anyKeyDown = true;
            UpdateKeyPress((SDL_KeyboardEvent*)&controlEvent);
            break;
        }
        case SDL_KEYUP:
        {
            anyKeyRelease = true;
            UpdateKeyRelease((SDL_KeyboardEvent*)&controlEvent);
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        {
            UpdateMousePress((SDL_MouseButtonEvent*)&controlEvent);
            break;
        }
        case SDL_MOUSEBUTTONUP:
        {
            UpdateMouseRelease((SDL_MouseButtonEvent*)&controlEvent);
            break;
        }
        case SDL_MOUSEMOTION:
        {
            UpdateMousePosition((SDL_MouseMotionEvent*)&controlEvent);
            break;
        }
        case SDL_QUIT:
        {
            quit = true;
            break;
        }
        default:
        {
            break;
        }
    };

    // Determine key PRESS bool here
    // For every currently down key
    for (std::map<SDL_Keycode, bool>::iterator retardedIter = keys.begin();
    retardedIter != keys.end();) {
        // Was it down last frame?
        if (prevKeys.count(retardedIter->first) == 0) {
            // If not, anyKeyPress is true
            anyKeyPress = true;
            // Stop the loop early
            retardedIter = keys.end();
        }

        // Our code is fucking retarded
        if (retardedIter != keys.end()) {
            // Hold his fucking hand while
            // he takes a fucking piss
            // so he doesn't piss the front
            // of his fucking pants
            retardedIter++;
        }
    }

}

// Sets a key to the status of "pressed"
void SDLInput::UpdateKeyPress(SDL_KeyboardEvent* keyEvent) {
    keys[keyEvent->keysym.sym] = true;
    mods = keyEvent->keysym.mod;
}

// Sets a key to the status of "released"
void SDLInput::UpdateKeyRelease(SDL_KeyboardEvent* keyEvent) {
    keys[keyEvent->keysym.sym] = false;
    mods = keyEvent->keysym.mod;
}

// Sets a button to the status of "pressed"
void SDLInput::UpdateMousePress(SDL_MouseButtonEvent* mouseEvent) {
    if (mouseEvent->button < NUMBUTTONS) {
        buttons[mouseEvent->button] = true;
        doubleClicked = false;
        if (mouseEvent->clicks > 1) {
            doubleClicked = true;
        }
    }
}

// Sets a button to the status of "released"
void SDLInput::UpdateMouseRelease(SDL_MouseButtonEvent* mouseEvent) {
    if (mouseEvent->button < NUMBUTTONS) {
        buttons[mouseEvent->button] = false;
    }
}

// Updates the mouse position from movement
void SDLInput::UpdateMousePosition(SDL_MouseMotionEvent* mouseEvent) {
    // The position
    mousePos.x = mouseEvent->x;
    mousePos.y = mouseEvent->y;
    // Also update the buttons (if they are holding a button down and dragging)
    buttons[SDL_BUTTON_LEFT] = (mouseEvent->state & SDL_BUTTON_LMASK) > 0;
    buttons[SDL_BUTTON_MIDDLE] = (mouseEvent->state & SDL_BUTTON_MMASK) > 0;
    buttons[SDL_BUTTON_RIGHT] = (mouseEvent->state & SDL_BUTTON_RMASK) > 0;
    buttons[SDL_BUTTON_X1] = (mouseEvent->state & SDL_BUTTON_X1MASK) > 0;
    buttons[SDL_BUTTON_X2] = (mouseEvent->state & SDL_BUTTON_X2MASK) > 0;
}

// Returns true if a key is down
bool SDLInput::KeyDown(const SDL_Keycode key) const {
    // A key is down if it's down, period
    if (keys.count(key) > 0) {
        return keys.at(key);
    }
    return false;
}

// Returns true if a key is down (modifiers)
bool SDLInput::KeyDown(const SDL_Keymod key) const {
    return (mods & key) > 0;
}

// Returns true if a key is up
bool SDLInput::KeyUp(const SDL_Keycode key) const {
    // A key is up if it is simply not down THIS frame
    if (keys.count(key) == 0 || keys.at(key) == false) {
        return true;
    }
    return false;
}

// Returns true if a key is down (modifiers)
bool SDLInput::KeyUp(const SDL_Keymod key) const {
    return (mods & key) == 0;
}

// Returns true if a key is pressed
bool SDLInput::KeyPressed(const SDL_Keycode key) const {
    // A key is PRESSED if it WASN'T down last frame, but it IS down this frame
    if ((prevKeys.count(key) == 0 || prevKeys.at(key) == false) 
        && keys.count(key) > 0 && keys.at(key) == true) {
        return true;
    }
    return false;
}

// Returns true if a key is pressed (modifiers)
bool SDLInput::KeyPressed(const SDL_Keymod key) const {
   return (((mods & key) > 0) && ((prevMods & key) == 0));
}

// Returns true if a key is released
bool SDLInput::KeyReleased(const SDL_Keycode key) const {
    // A key is considered RELEASED if it was down last frame, but released this frame
    if (prevKeys.count(key) > 0 && prevKeys.at(key) && keys.at(key) == false) {
        return true;
    }
    return false;
}

// Returns true if a key is released (modifiers)
bool SDLInput::KeyReleased(const SDL_Keymod key) const {
    return (((mods & key) == 0) && ((prevMods & key) > 0));
}

// Returns true if a button is down
bool SDLInput::ButtonDown(const int button) const {
    if (button < NUMBUTTONS) {
        return buttons[button];
    }
    return false;
}

// Returns true if a button is up
bool SDLInput::ButtonUp(const int button) const {
    if (button < NUMBUTTONS) {
        return !buttons[button];
    }
    return true;
}

// Returns true if a button is pressed
bool SDLInput::ButtonPressed(const int button) const {
    if (button < NUMBUTTONS) {
        return (buttons[button] && !prevButtons[button]);
    }
    return false;
}

// Returns true if a button is released
bool SDLInput::ButtonReleased(const int button) const {
    if (button < NUMBUTTONS) {
        return (!buttons[button] && prevButtons[button]);
    }
    return true;
}
