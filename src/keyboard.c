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
#include "m6502.h"
#include "memory.h"
#include "options.h"
#include "pia6820.h"
#include "screen.h"

static FILE *_fp;
static const char *_filename;
static char buffer[1024];
static int i, length;

void setInputFile(FILE *fp, const char *filename)
{
	_fp = fp;
	_filename = filename;
	i = length = 0;
}

void closeInputFile(void)
{
	if (_fp)
	{
		fclose(_fp);
		_fp = NULL;
	}
}

int isInputFileOpen(void)
{
	return (_fp ? 1 : 0);
}

const char *getInputFileName(void)
{
	return _filename;
}

int handleInput(void)
{
	SDL_Event event;
	unsigned char tmp;

	if (readKbdCr() == 0x27 && _fp)
	{
		if (i < length)
		{
			tmp = buffer[i++];

			if (tmp >= 0x61 && tmp <= 0x7A)
				tmp &= 0x5F;
			else if (tmp == 0x0D)
				i++;
			else if (tmp == 0x0A)
				tmp = 0x0D;
			
			if (tmp < 0x60)
			{
				writeKbd((unsigned char)(tmp | 0x80));
				writeKbdCr(0xA7);
			}
		}
		else if (feof(_fp))
		{
			closeInputFile();
			printf("stdout: Successfully loaded \"%s\"\n", _filename);
		}
		else
		{
			i = 0;
			length = fread(buffer, 1, 1024, _fp);
		}
	}

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			return 0;

		if (event.type == SDL_KEYDOWN && event.key.keysym.mod & KMOD_CTRL)
		{
			if (event.key.keysym.sym == SDLK_l)
			{
				loadMemory();
				return 1;
			}
			else if (event.key.keysym.sym == SDLK_s)
			{
				saveMemory();
				return 1;
			}
			else if (event.key.keysym.sym == SDLK_q)
				return 0;
			else if (event.key.keysym.sym == SDLK_r)
			{
				resetPia6820();
				resetM6502();
				return 1;
			}
			else if (event.key.keysym.sym == SDLK_h)
			{
				stopM6502();
				resetM6502();
				resetScreen();
				resetPia6820();
				resetMemory();
				startM6502();
				return 1;
			}
			else if (event.key.keysym.sym == SDLK_p)
			{
				changePixelSize();
				return 1;
			}
			else if (event.key.keysym.sym == SDLK_n)
			{
				if (getPixelSize() > 1)
				{
					setScanlines(!getScanlines());
					printf("stdout: scanlines=%d\n", getScanlines());
					redrawScreen();
				}

				return 1;
			}
			else if (event.key.keysym.sym == SDLK_t)
			{
				changeTerminalSpeed();
				return 1;
			}
			else if (event.key.keysym.sym == SDLK_e)
			{
				setRam8k(!getRam8k());
				printf("stdout: ram8k=%d\n", getRam8k());
				return 1;
			}
			else if (event.key.keysym.sym == SDLK_w)
			{
				setWriteInRom(!getWriteInRom());
				printf("stdout: writeInRom=%d\n", getWriteInRom());
				return 1;
			}
			else if (event.key.keysym.sym == SDLK_v)
			{
				setIrqBrkVector();
				return 1;
			}
			else if (event.key.keysym.sym == SDLK_f)
			{
				setFullscreen(!getFullscreen());
				printf("stdout: fullscreen=%d\n", getFullscreen());
				SDL_SetVideoMode(280 * getPixelSize(), 192 * getPixelSize(), 8, SDL_HWSURFACE | (getFullscreen() ? SDL_FULLSCREEN : 0));
				initScreen();
				SDL_ShowCursor(!getFullscreen());
				redrawScreen();
				return 1;
			}
			else if (event.key.keysym.sym == SDLK_b)
			{
				setBlinkCursor(!getBlinkCursor());
				printf("stdout: blinkCursor=%d\n", getBlinkCursor());
				redrawScreen();
				return 1;
			}
			else if (event.key.keysym.sym == SDLK_c)
			{
				setBlockCursor(!getBlockCursor());
				printf("stdout: blockCursor=%d\n", getBlockCursor());
				redrawScreen();
				return 1;
			}
			else if (event.key.keysym.sym == SDLK_a)
			{
				showAbout();
				return 1;
			}
		}

		if (readKbdCr() == 0x27 && !_fp && event.type == SDL_KEYDOWN && !(event.key.keysym.unicode & 0xFF80) && event.key.keysym.unicode)
		{
			tmp = event.key.keysym.unicode & 0x7F;

			if (tmp >= 0x61 && tmp <= 0x7A)
				tmp &= 0x5F;

			if (tmp < 0x60)
			{
				writeKbd((unsigned char)(tmp | 0x80));
				writeKbdCr(0xA7);
			}
		}
	}

	return 1;
}
