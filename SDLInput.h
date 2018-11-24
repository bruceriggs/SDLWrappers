#ifndef SDLINPUT_H_
#define SDLINPUT_H_

// Defines
#define NUMBUTTONS 6

// Includes
#include <SDL_events.h>
#include <map>
using std::map;

struct MousePos {
   int x;
   int y;
};

class SDLInput
{
private:
   // The control event
   SDL_Event controlEvent;
   
   // The keys
   //bool keys[NUMKEYS];
   map<SDL_Keycode, bool> keys;
   
   // The keys last frame
   //bool prevKeys[NUMKEYS];
   map<SDL_Keycode, bool> prevKeys;

   // If any key was pressed
   bool anyKeyDown;
   bool anyKeyPress;
   bool anyKeyRelease;

   // Keyboard modifiers (shift, control, alt)
   Uint16 mods;

   // Keyboard modifiers last frame
   Uint16 prevMods;

   // The mouse buttons
   bool buttons[NUMBUTTONS];

   // The mouse buttons last frame
   bool prevButtons[NUMBUTTONS];

   // True if double clicked, else false
   bool doubleClicked;

   // Mouse coords, relative to window (0,0) = TL corner
   MousePos mousePos;

   // Did we get the quit event
   bool quit;

   // Constructor
   SDLInput();

   // Copy constructor
   SDLInput(const SDLInput& rhs) {};
   
   // Assignment operator
   SDLInput& operator=(const SDLInput& rhs) { return *this; };

   // Sets a key to the status of "pressed"
   void UpdateKeyPress(SDL_KeyboardEvent* keyEvent);

   // Sets a key to the status of "released"
   void UpdateKeyRelease(SDL_KeyboardEvent* keyEvent);

   // Sets a button to the status of "pressed"
   void UpdateMousePress(SDL_MouseButtonEvent* mouseEvent);

   // Sets a button to the status of "released"
   void UpdateMouseRelease(SDL_MouseButtonEvent* mouseEvent);

   // Update the mouse position
   void UpdateMousePosition(SDL_MouseMotionEvent* mouseEvent);
public:
   // Destructor
   ~SDLInput();

   // Instance
   static SDLInput* Instance();

   // Initializes the Input
   void Init();

   // Shuts down the input
   void Shutdown();

   // Updates the controls
   void Update();

   // Returns true if a key is down
   bool KeyDown(const SDL_Keycode key) const;

   // Returns true if a key is down (modifiers)
   bool KeyDown(const SDL_Keymod key) const;

   // Returns true if a key is up
   bool KeyUp(const SDL_Keycode key) const;

   // Returns true if a key is down (modifiers)
   bool KeyUp(const SDL_Keymod key) const;

   // Returns true if a key is pressed
   bool KeyPressed(const SDL_Keycode key) const;

   // Returns true if a key is pressed (modifiers)
   bool KeyPressed(const SDL_Keymod key) const;

   // Returns true if a key is released
   bool KeyReleased(const SDL_Keycode key) const;

   // Returns true if a key is released (modifiers)
   bool KeyReleased(const SDL_Keymod key) const;

   // Returns true if a button is down
   bool ButtonDown(const int button) const;

   // Returns true if a button is up
   bool ButtonUp(const int button) const;
   
   // Returns true if a button is pressed
   bool ButtonPressed(const int button) const;

   // Returns true if a button is released
   bool ButtonReleased(const int button) const;

   // Returns true if this was a double click
   bool IsDoubleClick() const { return doubleClicked; }

   // Returns the mouse position
   MousePos GetMousePos() const { return mousePos; }

   // Returns true if the SDL_QUIT event was heard
   bool Quit() const { return quit; }

   // Returns true if any key was pressed
   bool AnyKeyPressed() const { return anyKeyPress; }

   // Returns true if any key was down
   bool AnyKeyDown() const { return anyKeyDown; }

   // Returns true if any key was released
   bool AnyKeyRelease() const { return anyKeyRelease; }
};

#endif // SLDINPUT_H_