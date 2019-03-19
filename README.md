# SDLWrappers
Wrappers for SDL2 that make using them easier.

#### Purpose
Once you have SDL2 libraries and includes included into your project, these wrappers make creating windows and checking input and loading images pretty easy.

### Library Versions
* SDL2 - 2.0.9
  * https://www.libsdl.org/release/SDL2-devel-2.0.9-VC.zip
* SDL2 Image - 2.0.4
  * https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.4-VC.zip
* SDL2 Mixer - 2.0.4
  * https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-devel-2.0.4-VC.zip

### Project Specifications
Your project will have to include the include directories for
* SDL
* SDL Image
* SDL Mixer

Your project will have to include the following .libs into your project
* SDL2.lib
* SDL2main.lib
* SDL2_image.lib
* SDL2_mixer.lib

After building, your .exe will require the following DLLs
* SDL2.dll
* SDL2_image.dll
  * libjpeg-9.dll
  * libpng16-16.dll
  * libtiff-5.dll
  * libwebp-4.dll
  * zlib1.dll
* SDL2_mixer.dll
  * libFLAC-8.dll
  * libmodplug-1.dll
  * libmpg123-0.dll
  * libogg-0.dll
  * libopus-0.dll
  * libopusfile-0.dll
  * libvorbis-0.dll
  * libvorbisfile-3.dll
