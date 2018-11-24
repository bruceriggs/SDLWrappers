#ifndef SDLTEXTURE_H_
#define SDLTEXTURE_H_

// Includes
#include <vector>
#include <Windows.h>
#include <SDL_image.h>

// Using statements
using std::vector;

// Helper constructor since SDL_Rect has no useful constructor
SDL_Rect Rect(int x, int y, int w, int h);

class SDLTexture {
private:

   struct Texture {
      SDL_Surface* image;
      SDL_Texture* texture;
      char* fileName;
      int refCount;

      Texture() {
         refCount = 0;
         fileName = 0;
         texture = 0;
         image = 0;
      }
   };

   // Members
   SDL_Renderer* renderer;
   SDL_Surface* screen;
   vector<Texture> textures;

   // Constructor
   SDLTexture();

   // Copy Constructor
   SDLTexture(const SDLTexture& rhs) {};

   // Assignment Operator
   SDLTexture& operator=(const SDLTexture& rhs) { return *this; }

   // The function that actually loads the image (not accessible by user)
   int load(const char* fileName, Uint32 colorKey, bool useColorkey);

public:
    // Color Struct
    struct TextureColor {
        int r;
        int g;
        int b;
        int a;
        TextureColor() { r = g = b = 0;
        a = 255; }
    };
   // Destructor
   ~SDLTexture();

   // Gets the instance
   static SDLTexture* Instance();

   // Initializes the class
   void Init(SDL_Renderer* renderer, SDL_Surface* screen);

   // Shuts down the class
   void Shutdown();

   // Loads an image without a colorkey
   int LoadImage(const char* fileName);

   // Loads an image with a colorkey
   int LoadImage(const char* fileName, int red, int green, int blue, int alpha = 255);

   // Unloads an image if no one else is using it
   void UnloadImage(const int imageId);

   // Returns the width of an image
   int ImageWidth(int imageId) const;

   // Returns the height of an image
   int ImageHeight(int imageId) const;

   // Sets the color modulation of an image
   void SetColorModulation(int imageId, Uint8 r = 255, Uint8 g = 255, Uint8 b = 255);

   // Gets the color modulation of an image
   void GetColorModulation(int imageId, Uint8* r, Uint8* g, Uint8* b);

   // Sets the alpha modulation of an image
   void SetAlphaModulation(int imageId, Uint8 a);

   // Gets the alpha modulation of an image (0 = transparent, 255 visible)
   void GetAlphaModulation(int imageId, Uint8* a);

   // Sets the blendmode of a texture
   void SetTextureBlendMode(int imageId, int blendMode = 0);

   // Draws an image to the screen, no rotation parameters
   void Draw(int imageId, int posX, int posY, SDL_Rect* section = 0, float scaleX = 1.0f, float scaleY = 1.0f);
   
   // Draws the image to the screen, rotation parameters included
   void Draw(int imageId, int posX, int posY, SDL_Rect* section, float scaleX,
      float scaleY, double angleDegrees, int rotationPointX, int rotationPointY,
      SDL_RendererFlip flipDirection);

   // Returns the color of the pixel at the specified location on the specified image
   TextureColor GetPixelColor(int imageId, int x, int y);
};

#endif // SDLTEXTURE_H_