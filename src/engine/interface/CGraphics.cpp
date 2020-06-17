/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "interface/CGraphics.h"
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "CLog.h"
#include "utils/DirPath.h"
#include "sConfig.h"
#include "interface/cColor.h"
#include "cTimer.h"
#include "interface/cFont.h"

extern cConfig cfg;

CGraphics::CGraphics() : m_ImageCache(this)
{
    m_FPS = std::make_unique<cTimer>();
}

CGraphics::~CGraphics()
{
    TTF_Quit();
    SDL_Quit();
}

void CGraphics::Begin()
{
    m_FPS->Start();
    m_BackgroundImage.DrawSurface(0, 0);
}

bool CGraphics::End()
{
    if(SDL_UpdateTexture(m_Screen, nullptr, m_ScreenBuffer->pixels, m_ScreenBuffer->pitch)) {
        g_LogFile.error("interface", "Could not update screen buffer: ", SDL_GetError());
    }
    if(SDL_RenderClear(m_Renderer)) {
        g_LogFile.error("interface", "Could not clear render: ", SDL_GetError());
    }
    if(SDL_RenderCopy(m_Renderer, m_Screen, nullptr, nullptr)) {
        g_LogFile.error("interface", "Could not copy screen buffer: ", SDL_GetError());
    }
    SDL_RenderPresent(m_Renderer);


    // Maintain framerate
    if((m_FPS->GetTicks() < 1000 / FRAMES_PER_SECOND))
    {
        //Sleep the remaining frame time
        SDL_Delay((1000/FRAMES_PER_SECOND)-m_FPS->GetTicks());
    }

    return true;
}

bool CGraphics::InitGraphics(std::string caption, int Width, int Height, int BPP)
{
    m_ScreenWidth = Width;
    m_ScreenHeight = Height;
    if (cfg.resolution.configXML())
    {
        m_ScreenScaleX = (float)m_ScreenWidth / (float)cfg.resolution.scalewidth();
        m_ScreenScaleY = (float)m_ScreenHeight / (float)cfg.resolution.scaleheight();
        m_Fullscreen = cfg.resolution.fullscreen();
    }
    else
    {
        g_LogFile.info("interface","Skipping Screen Mode");
        m_Fullscreen = false;
    }

    // init SDL
    g_LogFile.info("interface", "Initializing SDL");
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        g_LogFile.error("interface", "Poop. SDL_INIT_EVERYTHING failed.");
        g_LogFile.error("interface", SDL_GetError());
        return false;
    }
    m_ScreenBPP = BPP;

    // set window icon
    g_LogFile.info("interface","Setting Window Icon");
    SDL_Surface* loadIcon = IMG_Load(ImagePath("window_icon.png"));
    if(!loadIcon)
    {
        g_LogFile.error("interface", "Error setting window icon (window_icon.png)");
        g_LogFile.error("interface", SDL_GetError());
    }

    // Setup the screen
    g_LogFile.info("interface", "Determining Fullscreen or Windowed Mode");
    if (m_Fullscreen) {
        SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &m_Window, &m_Renderer);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
        SDL_RenderSetLogicalSize(m_Renderer, 640, 480);
    }
    else
        SDL_CreateWindowAndRenderer(m_ScreenWidth, m_ScreenHeight, 0, &m_Window, &m_Renderer);
    if(!(m_Window && m_Renderer))    // check for error
    {
        g_LogFile.error("interface", "Could not SDL_CreateWindowAndRenderer");
        g_LogFile.error("interface", SDL_GetError());
        return false;
    }

    // prepare the screen
    m_ScreenBuffer = SDL_CreateRGBSurface(0, m_ScreenWidth, m_ScreenHeight, 32,
            0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);

    m_Screen = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,
            m_ScreenWidth, m_ScreenHeight);
    if(!(m_ScreenBuffer && m_Screen))    // check for error
    {
        g_LogFile.error("interface", "Could not prepare screen buffer");
        g_LogFile.error("interface", SDL_GetError());
        return false;
    }

    if(loadIcon)
        SDL_SetWindowIcon(m_Window, loadIcon);

    // set window caption
    g_LogFile.info("interface","Setting Window Caption");
    SDL_SetWindowTitle(m_Window, caption.c_str());

    // Init TTF
    g_LogFile.info("interface","Initializing TTF");
    if(TTF_Init() == -1)
    {
        g_LogFile.error("interface", "Could not initialize SDL_TTF");
        g_LogFile.error("interface", TTF_GetError());
        return false;
    }

    // Load the universal background image
    m_BackgroundImage = LoadImage(ImagePath("background.jpg"), GetWidth(), GetHeight(), false, false);
    g_LogFile.info("interface","Background Image Set");

    return true;
}

cSurface CGraphics::LoadImage(std::string filename, int width, int height, bool transparency, bool keep_ratio)
{
    return m_ImageCache.LoadImage(std::move(filename), width, height, transparency, keep_ratio);
}

cSurface CGraphics::CreateSurface(int width, int height, sColor color, bool transparent)
{
    return m_ImageCache.CreateSurface(width, height, color, transparent);
}

cFont CGraphics::LoadFont(const std::string& font, int size)
{
    cFont f{this};
    f.LoadFont(font, size);
    return std::move(f);
}
