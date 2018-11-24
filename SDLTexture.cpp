#include "SDLTexture.h"

#include <string>
#include <cassert>
using std::string;

// Constructor
SDLTexture::SDLTexture()
{
}

// Destructor
SDLTexture::~SDLTexture()
{
   // Clean up any memory
   Shutdown();
}

// Gets the instance
SDLTexture* SDLTexture::Instance()
{
   // Lazy instantiation
   static SDLTexture instance;
   return &instance;
}

// Initializes the class
void SDLTexture::Init(SDL_Renderer* renderer, SDL_Surface* screen)
{
   // Cleanup any other existing data before doing this
   Shutdown();

   // Initialize SDL Image
   IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP);

   // Set the renderer and screen
   this->renderer = renderer;
   this->screen = screen;

   // Clear the textures array (should already be empty)
   textures.clear();
}

// Shuts down the class
void SDLTexture::Shutdown()
{
   // For every existing image
   for (unsigned int ii = 0; ii < textures.size(); ++ii) {
      // Delete the texture
      if (textures[ii].texture) {
         SDL_DestroyTexture(textures[ii].texture);
         textures[ii].texture = 0;
      }

      // Delete the Surface
      if (textures[ii].image) {
         SDL_FreeSurface(textures[ii].image);
         textures[ii].image = 0;
      }

      // No name, no count
      textures[ii].fileName = 0;
      textures[ii].refCount = 0;
   }

   // Clear the vector
   textures.clear();

   // Shutdown SDL Image
   IMG_Quit();
}


// Loads an image
int SDLTexture::load(const char* fileName, Uint32 colorKey, bool useColorKey)
{
   // If there's no filename, renderer, or screen
   if (!fileName || !renderer || !screen) {
      // Quit
      return -1;
   }

   // For every pre-existing image
   for (unsigned int ii = 0; ii < textures.size(); ++ii) {
      // Does this image already exist?
      if (textures[ii].fileName != nullptr && strcmp(textures[ii].fileName,fileName) == 0) {
         // Increase the reference count, return the id
         textures[ii].refCount++;
         return (signed)ii;
      }
   }

   // Look for an open slot
   int id = -1;
   for (unsigned int ii = 0; ii < textures.size(); ++ii) {
      if (textures[ii].refCount == 0) {
         id = (signed)ii;
      }
   }

   // We didn't find a slot (common path)
   if (id == -1) {

      // Create our new texture
      Texture newTexture;
      
      // Copy the file name
      newTexture.fileName = (char*)fileName;
      
      // Load the image
      newTexture.image = IMG_Load(fileName);
      assert(newTexture.image && "Failed to load the specified image.");

      // Apply a colorkey?
      if (useColorKey) {
         SDL_SetColorKey(newTexture.image, SDL_TRUE, colorKey);
      }

      // Create a texture out of it
      newTexture.texture = SDL_CreateTextureFromSurface(renderer, newTexture.image);

      // Reference count is now 1
      newTexture.refCount = 1;

      // Push into list
      textures.push_back(newTexture);

      // Return the new index
      return textures.size() - 1;
   }
   // We found an empty slot (rare)
   else {
      // Copy the file name
      textures[id].fileName = (char*)fileName;

      // Load the image
      textures[id].image = IMG_Load(fileName);

      // Apply a colorkey?
      if (useColorKey) {
         SDL_SetColorKey(textures[id].image, SDL_TRUE, colorKey);
      }

      // Create a texture out of it
      textures[id].texture = SDL_CreateTextureFromSurface(renderer, textures[id].image);

      // Reference count is now 1
      textures[id].refCount = 1;
      
      // Return the id
      return id;
   }
   // How to display an error
   //const char* someError = IMG_GetError();
}

// Loads an image without a colorkey
int SDLTexture::LoadImage(const char* fileName) {
   // Load the image without an alpha
   return load(fileName, 0, false);
}

// Loads an image with a colorkey, defaults to the color Magenta
int SDLTexture::LoadImage(const char* fileName, int red, int green, int blue, int alpha)
{
   //  Load the image with an alpha
   return load(fileName, SDL_MapRGBA(screen->format, red, green, blue, alpha), true);
}

// Unloads an image if no one else is using it
void SDLTexture::UnloadImage(const int imageId)
{
   // If we have nothing left to unload
   if (textures.size() <= 0) {
      return;
   }

   // If they provide a bad index, assert
   assert(imageId > -1  && "Invalid Texture ID provided when unloading an image");
   
   textures[imageId].refCount--;

   // If the texture is no longer being used
   if (textures[imageId].refCount <= 0) {
      // Delete the Texture
      SDL_DestroyTexture(textures[imageId].texture);
      textures[imageId].texture = 0;
      // Delete the Surface
      SDL_FreeSurface(textures[imageId].image);
      textures[imageId].image = 0;
      // Delete the file name
      textures[imageId].fileName = 0;
      // Ref count is 0
      textures[imageId].refCount = 0;
   }
}

// Returns the width of an image
int SDLTexture::ImageWidth(int imageId) const
{
   assert(imageId > -1 && imageId < (signed)textures.size() && "Invalid Texture Id when getting Width");
   return textures[imageId].image->w;
}

// Returns the height of an image
int SDLTexture::ImageHeight(int imageId) const
{
   assert(imageId > -1 && imageId < (signed)textures.size() && "Invalid Texture Id when getting Height");
   return textures[imageId].image->h;
}

// Draws the image to the screen, no rotation options
void SDLTexture::Draw(int imageId, int posX, int posY, SDL_Rect* section, float scaleX, float scaleY)
{
   // Error checking
   assert(imageId > -1 && imageId < (signed)textures.size() && "Invalid Texture Id when Drawing");

   // Where on the screen
   SDL_Rect destRect;
   destRect.x = posX;
   destRect.y = posY;
   if (section) {
      destRect.w = (int)(section->w * scaleX);
      destRect.h = (int)(section->h * scaleY);
   }
   else {
      destRect.w = this->ImageWidth(imageId) * scaleX;
      destRect.h = this->ImageHeight(imageId) * scaleY;
   }

   // Draw it to the screen
   SDL_RenderCopy(renderer, textures[imageId].texture, section, &destRect);
}

// Draws the image to the screen, with rotation options
void SDLTexture::Draw(int imageId, int posX, int posY, SDL_Rect* section, float scaleX,
   float scaleY, double angleDegrees, int rotationPointX, int rotationPointY,
   SDL_RendererFlip flipDirection)
{
   // Error checking
   assert(imageId > -1 && imageId < (signed)textures.size() && "Invalid Texture Id when Drawing");

   // Where on the screen
   SDL_Rect destRect;
   destRect.x = posX;
   destRect.y = posY;
   if (section) {
      destRect.w = (int)(section->w * scaleX);
      destRect.h = (int)(section->h * scaleY);
   }
   else {
      destRect.w = this->ImageWidth(imageId);
      destRect.h = this->ImageHeight(imageId);
   }

   // Rotation Point
   SDL_Point rotationPoint;
   rotationPoint.x = rotationPointX;
   rotationPoint.y = rotationPointY;

   // Draw
   SDL_RenderCopyEx(renderer, textures[imageId].texture, section, &destRect, angleDegrees, &rotationPoint, flipDirection);
}

SDL_Rect Rect(int x, int y, int w, int h)
{
   // Make and fill out a rect
   SDL_Rect newRect;
   newRect.x = x;
   newRect.y = y;
   newRect.w = w;
   newRect.h = h;

   // Return it
   return newRect;
}

// Sets the color modulation of an image
void SDLTexture::SetColorModulation(int imageId, Uint8 r, Uint8 g, Uint8 b)
{
   // Error checking
   assert(imageId > -1 && imageId < (signed)textures.size() && "Invalid Texture Id when setting color modulation");

   SDL_SetTextureColorMod(textures[imageId].texture, r, g, b);
}

// Gets the color modulation of an image
void SDLTexture::GetColorModulation(int imageId, Uint8* r, Uint8* g, Uint8* b)
{
   // Error checking
   assert(imageId > -1 && imageId < (signed)textures.size() && "Invalid Texture Id when getting color modulation");

   SDL_GetTextureColorMod(textures[imageId].texture, r, g, b);
}

// Sets the alpha modulation of an image
void SDLTexture::SetAlphaModulation(int imageId, Uint8 a)
{
   // Error checking
   assert(imageId > -1 && imageId < (signed)textures.size() && "Invalid Texture Id when setting alpha modulation");

   SDL_SetTextureAlphaMod(textures[imageId].texture, a);
}

// Gets the alpha modulation of an image
void SDLTexture::GetAlphaModulation(int imageId, Uint8* a)
{
   // Error checking
   assert(imageId > -1 && imageId < (signed)textures.size() && "Invalid Texture Id when getting alpha modulation");

   SDL_GetTextureAlphaMod(textures[imageId].texture, a);
}

void SDLTexture::SetTextureBlendMode(int imageId, int blendMode) {
   
   assert(imageId > -1 && imageId < (signed)textures.size() && "Invalid Texture Id when setting texture blend mode");

   // 0 = NONE
   // 1 = BLEND
   // 2 = ADD
   // 3 = MOD
   SDL_SetTextureBlendMode(textures[imageId].texture, (SDL_BlendMode)blendMode);
}

// Returns the color of the pixel at the specified location on the specified image
SDLTexture::TextureColor SDLTexture::GetPixelColor(int imageId, int x, int y) {
    
    assert(imageId > -1 && imageId < (signed)textures.size() && "Invalid Texture Id while fetching pixel color");

    // Determine the amount of pixels in the image
    int numPixelBytes = textures[imageId].image->w * textures[imageId].image->h;
    int numPixels = numPixelBytes;
    numPixelBytes *= textures[imageId].image->format->BytesPerPixel;
    byte* pixelData = nullptr;
    pixelData = new byte[numPixelBytes];

    // Prepare to copy over pixel information. Lock the image
    SDL_LockSurface(textures[imageId].image);

    // Copy the pixels over
    memcpy(pixelData, textures[imageId].image->pixels, numPixelBytes);

    // Unlock the image and leave it alone
    SDL_UnlockSurface(textures[imageId].image);

    // Calculate the pixel index
    // (row * image width) + column
    int pixelIndex = (y * textures[imageId].image->w) + x;
    pixelIndex *= textures[imageId].image->format->BytesPerPixel;

    // Allocate a color structure and determine colors
    TextureColor theColor;
    byte currentColor = pixelData[pixelIndex];

    string theFormat = string(SDL_GetPixelFormatName(textures[imageId].image->format->format));
    //SDL_GetPixelFormatName(textures[imageId].image->format->format)
    // RGB 24 ?
    if (theFormat == string("SDL_PIXELFORMAT_RGB24")) {
        // R
        theColor.r = pixelData[pixelIndex];

        // G
        theColor.g = pixelData[pixelIndex + 1];

        // B
        theColor.b = pixelData[pixelIndex + 2];

        // No Alpha
    }
    // RGBA 32 ?
    else if (theFormat == string("SDL_PIXELFORMAT_RGBA32")) {

    }

    // Cleanup dynamic memory
    if (pixelData) {
        delete[] pixelData;
    }

    // Return the color
    return theColor;
}