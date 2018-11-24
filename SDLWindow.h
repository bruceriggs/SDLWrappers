#ifndef SDLWINDOW_H_
#define SDLWINDOW_H_

// Includes
#include <SDL.h> // Mostly for the Uint32

class SDLWindow {
private:
   // Members
   int screenWidth;
   int screenHeight;
   bool fullscreen;
   Uint32 bgColor;
   SDL_Window* window;
   SDL_Surface* screen;
   SDL_Renderer* renderer;

   // Constructor
   SDLWindow();

   // Copy Constructor
   SDLWindow(const SDLWindow& rhs) {};

   // Assignment Operator
   SDLWindow& operator=(const SDLWindow& rhs) { return *this; };

public:
   // Destructor
   ~SDLWindow();

   // Singleton
   static SDLWindow* Instance();

   // Initializes the window/video
   void Init(const char* title = "My Window", const int screenWidth = 640, const int screenHeight = 480,
      const bool fullscreen = false, const bool vsync = false, const bool centerX = false, const bool centerY = false, 
      const bool borderless = false, const bool resizable = false);

   // Clears the screen (done every frame)
   void Clear();

   // Set the background color (defaults to black)
   void SetBackgroundColor(const int red = 0, const int green = 0, const int blue = 0);

   // Cleans up and shuts down the video
   void Shutdown();

   // Changes the resolution
   void Resolution(const int screenWidth, const int screenHeight);

   // Sets Fullscreen Mode
   void Fullscreen(const bool fullscreen = false);

   // Renders the screen. Called every frame
   void Present();

   // Accessors/Mutators
   int ScreenWidth() const { return screenWidth; }
   int ScreenHeight() const { return screenHeight; }
   bool Fullscreen() const { return fullscreen; }
   SDL_Window* Window() { return window; }
   SDL_Renderer* Renderer() { return renderer; }
   SDL_Surface* Screen() { return screen; }
};

#endif // SDLWINDOW_H_