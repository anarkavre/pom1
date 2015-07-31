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
#include "keyboard.h"
#include "m6502.h"
#include "memory.h"
#include "screen.h"
#include "config.h"

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

int main(int argc, char *argv[])
{
	int i, temp;
	char *romdir = getenv("POM1ROMDIR");

	atexit(freeRomDirectory);

	if (romdir)
		setRomDirectory(romdir);
	else
		setRomDirectory("roms");

	loadConfiguration();

	atexit(saveConfiguration);

	if (argc > 1)
	{
		for (i = 1; i < argc; i++)
		{
			if (!strcasecmp("-romdir", argv[i]) && i + 1 < argc)
				setRomDirectory(argv[i + 1]);
			else if (!strcasecmp("-pixelsize", argv[i]) && i + 1 < argc)
			{
				temp = atoi(argv[i + 1]);

				if (temp == 1 || temp == 2)
				{
					if (temp == 1)
						setScanlines(0);

					setPixelSize(temp);
				}
			}
			else if (!strcasecmp("-scanlines", argv[i]))
				if (getPixelSize() > 1)
					setScanlines(1);
			else if (!strcasecmp("-terminalspeed", argv[i]) && i + 1 < argc)
			{
				temp = atoi(argv[i + 1]);

				if (temp >= 1 && temp <= 120)
					setTerminalSpeed(temp);
			}
			else if (!strcasecmp("-ram8k", argv[i]))
				setRam8k(1);
			else if (!strcasecmp("-writeinrom", argv[i]))
				setWriteInRom(1);
			else if (!strcasecmp("-fullscreen", argv[i]))
				setFullscreen(1);
			else if (!strcasecmp("-blinkcursor", argv[i]))
				setBlinkCursor(1);
			else if (!strcasecmp("-blockcursor", argv[i]))
				setBlockCursor(1);
		}
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "stderr: Could not initialize SDL\n");
		return 1;
	}

	atexit(SDL_Quit);

	SDL_WM_SetCaption(PACKAGE_NAME, NULL);

	if (!SDL_SetVideoMode(280 * getPixelSize(), 192 * getPixelSize(), 8, SDL_HWSURFACE | (getFullscreen() ? SDL_FULLSCREEN : 0)))
	{
		fprintf(stderr, "stderr: Could not set video mode to %dx%dx8\n", 280 * getPixelSize(), 192 * getPixelSize());
		return 1;
	}

	initScreen();

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	SDL_EnableUNICODE(1);

	SDL_ShowCursor(!getFullscreen());

	if (!loadCharMap())
	{
		fprintf(stderr, "stderr: Could not load character map\n");
		return 1;
	}

	resetScreen();
	resetMemory();
	setSpeed(1000, 50);
	resetM6502();
	startM6502();

	atexit(stopM6502);
	atexit(closeInputFile);

	while (handleInput())
		updateScreen();

	return 0;
}
