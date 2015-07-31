// Pom1 Apple 1 Emulator
// Copyright (C) 2000 Verhille Arnaud
// Copyright (C) 2012 John D. Corrado
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "SDL.h"
#include "configuration.h"
#include "pia6820.h"

static unsigned char charac[1024], screenTbl[960];
static int indexX, indexY, pixelSize = 2, _scanlines = 0, terminalSpeed = 60;
static long lastTime;
static int _fullscreen = 0;
static int _blinkCursor = 1, _blockCursor = 0;
static SDL_Surface *screen;

int loadCharMap(void)
{
	const char *romdir = getRomDirectory();
	char *filename;
	FILE *fp;

	filename = (char *)malloc(strlen(romdir) + 13);
	sprintf(filename, "%s/charmap.rom", romdir);

	fp = fopen(filename, "rb");

	free(filename);

	if (fp)
	{
		fread(charac, 1, 1024, fp);
		fclose(fp);
	}
	else
		return 0;

	return 1;
}

void setPixelSize(int ps)
{
	pixelSize = ps;
}

int getPixelSize(void)
{
	return pixelSize;
}

void setScanlines(int scanlines)
{
	_scanlines = scanlines;
}

int getScanlines(void)
{
	return _scanlines;
}

void setTerminalSpeed(int ts)
{
	terminalSpeed = ts;
}

int getTerminalSpeed(void)
{
	return terminalSpeed;
}

static void synchronizeOutput(void)
{
	int sleepMillis = ((1000 / terminalSpeed) - (SDL_GetTicks() - lastTime));

	if (sleepMillis > 0)
		SDL_Delay(sleepMillis);

	lastTime = SDL_GetTicks();
}

static void newLine(void)
{
	memcpy(&screenTbl, &screenTbl[40], 920);
	memset(&screenTbl[920], 0, 40);
}

static void outputDsp(unsigned char dsp)
{
	unsigned char tmp;

	dsp &= 0x7F;

	tmp = dsp;

	if (dsp >= 0x60 && dsp <= 0x7F)
		tmp &= 0x5F;

	switch (tmp)
	{
	case 0x0D:
		indexX = 0;
		indexY++;
		break;
	default:
		if (tmp >= 0x20 && tmp <= 0x5F)
		{
			screenTbl[indexY * 40 + indexX] = tmp;
			indexX++;
		}
		break;
	}

	if (indexX == 40)
	{
		indexX = 0;
		indexY++;
	}
	if (indexY == 24)
	{
		newLine();
		indexY--;
	}

	writeDsp(dsp);
}

static void drawCharac(int xPosition, int yPosition, unsigned char r, unsigned char g, unsigned char b, unsigned char characNumber)
{
	SDL_Rect rect;
	int k, l;

	for (k = 0; k < 8; k++)
	{
		for (l = 1; l < 8; l++)
		{
			if (charac[characNumber * 8 + k] & (0x01 << l))
			{
				rect.x = xPosition + pixelSize * (l - 1);
				rect.y = yPosition + pixelSize * k;
				rect.w = pixelSize;
				rect.h = pixelSize - (_scanlines ? 1 : 0);

				SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, r, g, b));
			}
		}
	}
}

void setFullscreen(int fullscreen)
{
	_fullscreen = fullscreen;
}

int getFullscreen()
{
	return _fullscreen;
}

void setBlinkCursor(int blinkCursor)
{
	_blinkCursor = blinkCursor;
}

int getBlinkCursor(void)
{
	return _blinkCursor;
}

void setBlockCursor(int blockCursor)
{
	_blockCursor = blockCursor;
}

int getBlockCursor(void)
{
	return _blockCursor;
}

static void drawBlinkingCursor(void)
{
	static int clearCursor = 0;
	static long lastTime = 0;

	SDL_Rect rect;

	if ((SDL_GetTicks() - lastTime) > 500)
	{
		lastTime = SDL_GetTicks();

		rect.x = indexX * pixelSize * 7;
		rect.y = indexY * pixelSize * 8;
		rect.w = pixelSize * 7;
		rect.h = pixelSize * 8;
			
		if (clearCursor)
			SDL_FillRect(screen, &rect, 0);
		else
			drawCharac(rect.x, rect.y, 0, 255, 0, (unsigned char)(_blockCursor ? 0x01 : 0x40));

		SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
			
		clearCursor = !clearCursor;
	}
}

void redrawScreen(void)
{
	int xPosition, yPosition;
	int i, j;

	SDL_FillRect(screen, NULL, 0);
		
	for (i = 0; i < 40; i++)
	{
		for (j = 0; j < 24; j++)
		{
			xPosition = i * pixelSize * 7;
			yPosition = j * pixelSize * 8;
				
			drawCharac(xPosition, yPosition, 0, 255, 0, screenTbl[j * 40 + i]);
		}
	}

	if (!_blinkCursor)
		drawCharac(indexX * pixelSize * 7, indexY * pixelSize * 8, 0, 255, 0, (unsigned char)(_blockCursor ? 0x01 : 0x40));

	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void resetScreen(void)
{
	indexX = indexY = 0;

	memset(screenTbl, 0, 960);
	
	lastTime = SDL_GetTicks();

	redrawScreen();
}

void updateScreen(void)
{
	unsigned char dsp = readDsp();

	if (dsp & 0x80)
	{
		outputDsp(dsp);
		redrawScreen();
		synchronizeOutput();
	}
	else if (_blinkCursor)
		drawBlinkingCursor();
}

void drawCharacter(int xPosition, int yPosition, unsigned char r, unsigned char g, unsigned char b, unsigned char characNumber)
{
	if (_scanlines)
	{
		_scanlines = 0;
		drawCharac(xPosition, yPosition, 0, 0, 0, characNumber);
		_scanlines = 1;
	}
	else
		drawCharac(xPosition, yPosition, 0, 0, 0, characNumber);
}

void initScreen(void)
{
	screen = SDL_GetVideoSurface();
}
