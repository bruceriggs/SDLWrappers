#ifndef UTILITY_H_
#define UTILITY_H_

#include "SDLWindow.h"
#include "SDLTexture.h"
#include "SDLInput.h"

#define win SDLWindow::Instance()
#define tex SDLTexture::Instance()
#define input SDLInput::Instance()

#define HALFWIDTH(imageId) (SDLTexture::Instance()->ImageWidth(imageId) >> 1)
#define HALFHEIGHT(imageId) (SDLTexture::Instance()->ImageHeight(imageId) >> 1)
#define CENTERX (SDLWindow::Instance()->ScreenWidth() >> 1)
#define CENTERY (SDLWindow::Instance()->ScreenHeight() >> 1)

#endif // UTILITY_H_