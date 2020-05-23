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
#include "CGraphics.h"
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "CLog.h"
#include "DirPath.h"
#include "sConfig.h"
#include "cColor.h"
#include "cTimer.h"

extern cConfig cfg;

CGraphics::CGraphics()
{
    m_FPS = std::make_unique<cTimer>();
	m_Screen = nullptr;
	m_CurrentTime = 0;
}

CGraphics::~CGraphics()
{
	Free();
}

void CGraphics::Free()
{
    TTF_Quit();
    SDL_Quit();
}

void CGraphics::Begin()
{
	m_FPS->Start();
	m_CurrentTime = SDL_GetTicks();
	// Fill the screen black
	SDL_FillRect(m_Screen, &m_Screen->clip_rect, SDL_MapRGB(m_Screen->format, 0, 0, 0));


    m_BackgroundImage.DrawSurface(0, 0);
}

bool CGraphics::End()
{
	// flip the surface
	if(SDL_Flip(m_Screen) == -1)
	{
		g_LogFile.write(SDL_GetError());
		return false;
	}

	// Maintain framerate
	if((m_FPS->GetTicks() < 1000 / FRAMES_PER_SECOND))
	{
		//Sleep the remaining frame time
		SDL_Delay((1000/FRAMES_PER_SECOND)-m_FPS->GetTicks());
	}

	return true;
}

bool CGraphics::InitGraphics(string caption, int Width, int Height, int BPP)
{
	if ((Width == 0 || Height == 0) && cfg.resolution.configXML())
	{
		m_ScreenWidth = cfg.resolution.width();
		m_ScreenHeight = cfg.resolution.height();
        m_ScreenScaleX = (float)m_ScreenWidth / (float)cfg.resolution.scalewidth();
        m_ScreenScaleY = (float)m_ScreenHeight / (float)cfg.resolution.scaleheight();
		m_Fullscreen = cfg.resolution.fullscreen();
	}
	else
	{
		g_LogFile.write("Skipping Screen Mode");
		m_ScreenWidth = Width;
		m_ScreenHeight = Height;
        m_Fullscreen = false;
	}

	// init SDL
	g_LogFile.write("Initializing SDL");
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		g_LogFile.write("Poop. SDL_INIT_EVERYTHING failed.");
		g_LogFile.write(SDL_GetError());
		return false;
	}
	m_ScreenBPP = BPP;

	// set window icon
	g_LogFile.write("Setting Window Icon");
	SDL_Surface* loadIcon = IMG_Load(ImagePath("window_icon.png"));
	if(!loadIcon)
	{
		g_LogFile.write("Error setting window icon (window_icon.png)");
		g_LogFile.write(SDL_GetError());
	}
	else
		SDL_WM_SetIcon(loadIcon, nullptr);

	// Setup the screen
	g_LogFile.write("Determining Fullscreen or Windowed Mode");
    if (m_Fullscreen)
        m_Screen = SDL_SetVideoMode(m_ScreenWidth, m_ScreenHeight, m_ScreenBPP, SDL_SWSURFACE | SDL_FULLSCREEN);
    else
        m_Screen = SDL_SetVideoMode(m_ScreenWidth, m_ScreenHeight, m_ScreenBPP, SDL_SWSURFACE);
    if(!m_Screen)	// check for error
    {
        g_LogFile.write(SDL_GetError());
        return false;
    }

    // set window caption
    g_LogFile.write("Setting Window Caption");
    SDL_WM_SetCaption(caption.c_str(), nullptr);

    // Init TTF
    g_LogFile.write("Initializing TTF");
    if(TTF_Init() == -1)
    {
        g_LogFile.write("Error with TTF_Init()");
        g_LogFile.write(TTF_GetError());
        return false;
    }

    // Load the universal background image
    m_BackgroundImage = LoadImage(ImagePath("background.jpg"), GetWidth(), GetHeight(), false, false);
    g_LogFile.write("Background Image Set");

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
