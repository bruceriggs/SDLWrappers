#include "SDLWindow.h"

// Constructor
SDLWindow::SDLWindow()
{
   screen = nullptr;
   window = nullptr;
   renderer = nullptr;
}

// Destructor
SDLWindow::~SDLWindow()
{
   Shutdown();
}

// Gets an instance of the SDLWindow
SDLWindow* SDLWindow::Instance()
{
   static SDLWindow myWindow;
   return &myWindow;
}

// Initializes the window/video
void SDLWindow::Init(const char* title, const int screenWidth, const int screenHeight,
   const bool fullscreen, const bool vsync, const bool centerX, const bool centerY, const bool borderless,
   const bool resizable)
{
   // Initialize SDL Video
   SDL_Init(SDL_INIT_VIDEO);

   // Setup screen settings
   this->screenWidth = screenWidth;
   this->screenHeight = screenHeight;
   this->fullscreen = fullscreen;

   // Set up display flags
   Uint32 displayFlags = 0;
   displayFlags |= (fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
   displayFlags |= (borderless ? SDL_WINDOW_BORDERLESS : 0);
   displayFlags |= (resizable ? SDL_WINDOW_RESIZABLE : 0);

   // Create the window
   window = SDL_CreateWindow(title, //  Title of the window
      (centerX ? SDL_WINDOWPOS_CENTERED : SDL_WINDOWPOS_UNDEFINED), // Centered on the X?
      (centerY ? SDL_WINDOWPOS_CENTERED : SDL_WINDOWPOS_UNDEFINED), // Centered on the Y?
      screenWidth, // Width of the screen
      screenHeight, // Height of the screen
      displayFlags); // All the flags set up earlier

   // Create the Renderer
   int renderVals = 0;
   renderVals = renderVals | (vsync ? SDL_RENDERER_PRESENTVSYNC : 0);
   renderer = SDL_CreateRenderer(window, -1, renderVals);

   // Get the screen
   screen = SDL_GetWindowSurface(window);

   // Default the background color to black
   SetBackgroundColor();
}

void SDLWindow::Clear()
{
   // Clear the screen
   SDL_RenderClear(renderer);
}

void SDLWindow::SetBackgroundColor(const int red, const int green, const int blue)
{
   // Set the background color
   if (screen && screen->format) {
      bgColor = SDL_MapRGB(screen->format, red, green, blue);
   }
   SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
}

// Cleans up and shuts down
void SDLWindow::Shutdown()
{
   // Background back to black
   SetBackgroundColor();

   // Delete any existing screen
   if (screen) {
      SDL_FreeSurface(screen);
      screen = nullptr;
   }

   // Delete any existing renderer
   if (renderer) {
      SDL_DestroyRenderer(renderer);
      renderer = nullptr;
   }

   // Delete the window
   if (window) {
      SDL_DestroyWindow(window);
      window = nullptr;
   }

   // Shutdown the video
   SDL_QuitSubSystem(SDL_INIT_VIDEO);

   // Shut it all down
   SDL_Quit();
}

// Sets the fullscreen mode
void SDLWindow::Fullscreen(const bool fullscreen)
{
   this->fullscreen = fullscreen;
   SDL_SetWindowFullscreen(window, (fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
}

// Changes the screen resolution
void SDLWindow::Resolution(const int screenWidth, const int screenHeight)
{
   // Fullscreen?
   if (fullscreen) {
      // Do it this way
      SDL_DisplayMode currentMode;
      SDL_GetCurrentDisplayMode(0, &currentMode);
      currentMode.w = screenWidth;
      currentMode.h = screenHeight;
      SDL_SetWindowDisplayMode(window, &currentMode);
   }
   // Windowed?
   else {
      // Do it that way
      SDL_SetWindowSize(window, screenWidth, screenHeight);
   }
}

void SDLWindow::Present() {
   SDL_RenderPresent(this->Renderer());
}