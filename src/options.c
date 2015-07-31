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
#include "memory.h"
#include "keyboard.h"
#include "screen.h"
#include "config.h"

#define TYPE_STRING 1
#define TYPE_DECIMAL 2
#define TYPE_HEXADECIMAL 3
#define TYPE_CHOICE 4

#define isHexDigit(c) ((c >= '0' && c <= '9') || ((c & 0x5F) >= 'A' && (c & 0x5F) <= 'F'))

static int step, type, max, choice;
static unsigned int start;
static char filename[1024], buffer[1024];
static FILE *fp;

static void drawString(const char *str, int x, int y)
{
	char c;
	const char *p = str;
	int X = x;
	int characterWidth = 7 * getPixelSize(), characterHeight = 8 * getPixelSize();

	while (c = *p++)
	{
		if (c == '\n')
		{
			X = x;
			y += characterHeight;
		}
		else
		{
			drawCharacter(X, y, 0, 0, 0, c);
			X += characterWidth;
		}
	}
}

static void inputLoop(const char *str, int (*func)(void))
{
	SDL_Event event;
	SDL_Rect rect;
	SDL_Surface *screen = SDL_GetVideoSurface();
	int screenWidth = screen->w, screenHeight = screen->h;
	int i, c = 0, x = 0;
	char tmp;
	int pixelSize = getPixelSize();
	int characterWidth = 7 * pixelSize, characterHeight = 8 * pixelSize;

	rect.x = 0;
	rect.y = screenHeight - (2 * characterHeight + pixelSize);
	rect.w = screenWidth;
	rect.h = 2 * characterHeight + pixelSize;

	SDL_FillRect(screen, &rect, 255);

	drawString(str, 0, screenHeight - 16 * pixelSize);

	if (type != TYPE_CHOICE)
		drawCharacter(0, screenHeight - characterHeight, 0, 0, 0, 0x01);

	SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);

	step = 1;

	while (1)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.mod & KMOD_CTRL && event.key.keysym.sym == SDLK_q))
				exit(0);

			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					redrawScreen();
					return;
				}
				else if (event.key.keysym.sym == SDLK_SPACE && type == TYPE_STRING)
				{
					if (c < max)
						buffer[c++] = ' ';
					else
						continue;

					if (c - 1 < 39)
					{
						rect.x = x;
						rect.y = screenHeight - characterHeight;
						rect.w = characterWidth;
						rect.h = characterHeight;
						
						SDL_FillRect(screen, &rect, 255);
						
						x += characterWidth;

						drawCharacter(x, rect.y, 0, 0, 0, 0x01);

						SDL_UpdateRect(screen, rect.x, rect.y, 2 * characterWidth, characterHeight);
					}
					else
					{
						rect.x = 0;
						rect.y = screenHeight - characterHeight;
						rect.w = screenWidth - characterWidth;
						rect.h = characterHeight;

						SDL_FillRect(screen, &rect, 255);

						rect.w = characterWidth;

						for (i = 0; i < 39; i++)
						{
							rect.x = characterWidth * i;

							if (buffer[(c - 39) + i] == ' ')
								SDL_FillRect(screen, &rect, 255);
							else
								drawCharacter(rect.x, rect.y, 0, 0, 0, buffer[(c - 39) + i]);
						}

						SDL_UpdateRect(screen, 0, rect.y, screenWidth - characterWidth, characterHeight);
					}
				}
				else if (event.key.keysym.sym == SDLK_BACKSPACE && type != TYPE_CHOICE)
				{
					if (c > 0)
						buffer[--c] = '\0';
					else
						continue;

					if (x > 0 && c < 39)
					{
						x -= characterWidth;

						rect.x = x;
						rect.y = screenHeight - characterHeight;
						rect.w = 2 * characterWidth;
						rect.h = characterHeight;
						
						SDL_FillRect(screen, &rect, 255);
						
						drawCharacter(x, rect.y, 0, 0, 0, 0x01);

						SDL_UpdateRect(screen, rect.x, rect.y, rect.w, characterHeight);
					}
					else
					{
						rect.x = 0;
						rect.y = screenHeight - characterHeight;
						rect.w = screenWidth - characterWidth;
						rect.h = characterHeight;

						SDL_FillRect(screen, &rect, 255);

						rect.w = characterWidth;

						for (i = 0; i < 39; i++)
						{
							rect.x = characterWidth * i;

							if (buffer[(c - 39) + i] == ' ')
								SDL_FillRect(screen, &rect, 255);
							else
								drawCharacter(rect.x, rect.y, 0, 0, 0, buffer[(c - 39) + i]);
						}

						SDL_UpdateRect(screen, 0, rect.y, screenWidth - characterWidth, characterHeight);
					}
				}
				else if ((event.key.keysym.sym == SDLK_RETURN && c) || (type == TYPE_CHOICE && (event.key.keysym.sym == SDLK_1 || event.key.keysym.sym == SDLK_2)))
				{
					if (type == TYPE_CHOICE)
						choice = event.key.keysym.sym & 0x03;

					rect.x = 0;
					rect.y = screenHeight - (2 * characterHeight + pixelSize);
					rect.w = screenWidth;
					rect.h = 2 * characterHeight + pixelSize;

					SDL_FillRect(screen, &rect, 255);

					buffer[c] = '\0';

					if (!(*func)())
					{
						redrawScreen();
						return;
					}

					SDL_UpdateRect(screen, 0, rect.y, screenWidth, rect.h);

					c = x = 0;

					step++;
				}
				else if (!(event.key.keysym.unicode & 0xFF80) && event.key.keysym.unicode)
				{
					tmp = event.key.keysym.unicode & 0x7F;

					if (type == TYPE_HEXADECIMAL && tmp >= 0x61 && tmp <= 0x66)
						tmp &= 0x5F;

					if (c < max && ((type == TYPE_STRING && tmp >= 0x21 && tmp <= 0x7E) || (type == TYPE_DECIMAL && tmp >= 0x30 && tmp <= 0x39) || (type == TYPE_HEXADECIMAL && ((tmp >= 0x30 && tmp <= 0x39) || (tmp >= 0x41 && tmp <= 0x46)))))
							buffer[c++] = tmp;
					else
						continue;

					if (c - 1 < 39)
					{
						rect.x = x;
						rect.y = screenHeight - characterHeight;
						rect.w = characterWidth;
						rect.h = characterHeight;
						
						SDL_FillRect(screen, &rect, 255);
						
						drawCharacter(x, rect.y, 0, 0, 0, tmp);

						x += characterWidth;

						drawCharacter(x, rect.y, 0, 0, 0, 0x01);

						SDL_UpdateRect(screen, rect.x, rect.y, 2 * characterWidth, characterHeight);
					}
					else
					{
						rect.x = 0;
						rect.y = screenHeight - characterHeight;
						rect.w = screenWidth - characterWidth;
						rect.h = characterHeight;

						SDL_FillRect(screen, &rect, 255);

						rect.w = characterWidth;

						for (i = 0; i < 39; i++)
						{
							rect.x = 7 * pixelSize * i;

							if (buffer[(c - 39) + i] == ' ')
								SDL_FillRect(screen, &rect, 255);
							else
								drawCharacter(rect.x, rect.y, 0, 0, 0, buffer[(c - 39) + i]);
						}

						SDL_UpdateRect(screen, 0, rect.y, screenWidth - characterWidth, characterHeight);
					}
				}
			}
		}
	}
}

static int loadMemoryFunc(void)
{
	int i, length, size;
	unsigned int address, value;
	unsigned char *fbrut;

	if (step == 1)
	{
		type = TYPE_CHOICE;

		strcpy(filename, buffer);

		drawString("Choose file format:\nPress 1 for ASCII or 2 for Binary", 0, 192 * getPixelSize() - 16 * getPixelSize());
	}
	else if (step == 2)
	{
		if (choice == 1)
		{
			choice = 0;

			drawString("Do you want to simulate keyboard input?:\nPress 1 for yes or 2 for no", 0, 192 * getPixelSize() - 16 * getPixelSize());
		}
		else
		{
			type = TYPE_HEXADECIMAL;
			max = 4;
			
			drawString("Enter starting address:", 0, 192 * getPixelSize() - 16 * getPixelSize());

			return 1;
		}
	}
	else if (step == 3)
	{
		if (choice == 1 || choice == 2)
		{
			fp = fopen(filename, "r");

			if (!fp)
			{
				fprintf(stderr, "stderr: Could not open \"%s\" for read\n", filename);
				return 0;
			}
			
			if (choice == 1)
			{
				if (isInputFileOpen())
				{
					drawString("Do you want to abort the current read?:\nPress 1 for yes or 2 for no", 0, 192 * getPixelSize() - 16 * getPixelSize());
					return 1;
				}

				setInputFile(fp, filename);

				return 0;
			}

			while (!feof(fp))
			{
				if (!fgets(buffer, 1024, fp))
					continue;

				if (buffer[0] == '/')
					continue;

				if (buffer[0] != ':')
				{
					if (isHexDigit(buffer[0]))
					{
						if (buffer[1] == ':' || buffer[2] == '\0')
							i = 2;
						else if (isHexDigit(buffer[1]))
						{
							if (buffer[2] == ':' || buffer[3] == '\0')
								i = 3;
							else if (isHexDigit(buffer[2]))
							{
								if (buffer[3] == ':' || buffer[4] == '\0')
									i = 4;
								else if (isHexDigit(buffer[3]))
								{
									if (buffer[4] == ':' || buffer[5] == '\0')
										i = 5;
									else
										continue;
								}
								else
									continue;
							}
							else
								continue;
						}
						else
							continue;
					}
					else
						continue;

					sscanf(buffer, "%4X", &address);

					if (buffer[i] == ' ')
						i++;
					else if (buffer[i] == '\0')
						continue;
				}
				else if (buffer[1] == ' ')
					i = 2;
				else
					i = 1;
					
				length = strlen(buffer);
								
				for (; i < length; i += 3)
				{
					sscanf(&buffer[i], "%2X", &value);
					memWrite(address++, value);
				}
			}

			printf("stdout: Successfully loaded \"%s\"\n", filename);
		}
		else
		{
			sscanf(buffer, "%4X", &start);

			fp = fopen(filename, "rb");

			if (!fp)
			{
				fprintf(stderr, "stderr: Could not open \"%s\" for write\n", filename);
				return 0;
			}

			fseek(fp, 0, SEEK_END);
			size = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			if (size > 65536 || start + size - 1 > 65535)
				fprintf(stderr, "stderr: File size too large\n");
			else
			{
				fbrut = (unsigned char *)malloc(size);

				if (!fbrut)
					fprintf(stderr, "stderr: Could not allocate memory block\n");
				else
				{
					fread(fbrut, 1, size, fp);
					setMemory(fbrut, start, size);
					printf("stdout: Successfully loaded \"%s\"\n", filename);
				}
			}
		}

		fclose(fp);

		return 0;
	}
	else if (step == 4)
	{
		if (choice == 1)
		{
			closeInputFile();
			printf("stdout: Canceled loading \"%s\"\n", getInputFileName());
			setInputFile(fp, filename);
		}
		else
			fclose(fp);

		return 0;
	}

	return 1;
}

void loadMemory(void)
{
	type = TYPE_STRING;
	max = 1024;

	inputLoop("Enter file to load:", &loadMemoryFunc);
}

static int saveMemoryFunc(void)
{
	int i, j, k, length;
	unsigned int end, temp;
	unsigned char *fbrut;
	char *strFile;

	if (step == 1)
	{
		type = TYPE_CHOICE;

		strcpy(filename, buffer);

		drawString("Choose file format:\nPress 1 for ASCII or 2 for Binary", 0, 192 * getPixelSize() - 16 * getPixelSize());
	}
	else if (step == 2)
	{
		type = TYPE_HEXADECIMAL;
		max = 4;
		
		drawString("Enter starting address:", 0, 192 * getPixelSize() - 16 * getPixelSize());
	}
	else if (step == 3)
	{	
		sscanf(buffer, "%4X", &start);

		drawString("Enter ending address:", 0, 192 * getPixelSize() - 16 * getPixelSize());
	}
	else if (step == 4)
	{
		sscanf(buffer, "%4X", &end);

		if (start > end)
		{
			temp = start;
			start = end;
			end = temp;
		}

		if (choice == 1)
		{
			fp = fopen(filename, "w");

			if (!fp)
			{
				fprintf(stderr, "stderr: Could not open \"%s\" for write\n", filename);
				return 0;
			}

			fbrut = dumpMemory(start, end);

			if (fbrut)
			{
				strcpy(buffer, "// Pom1 Save - ");

				strFile = strrchr(filename, '/');
				
				if (!strFile)
				{
					strFile = strrchr(filename, '\\');
					
					if (!strFile)
						strFile = filename;
					else
						strFile++;
				}
				else
					strFile++;
				
				strcat(buffer, strFile);
				
				length = end - start + 1;
				
				for (i = 0, j = start; i < length; i++, k += 3)
				{
					if (i % 8 == 0)
					{
						fputs(buffer, fp);
						sprintf(buffer, "\n%04X: ", j);
						j += 8;
						k = 7;
					}
					
					sprintf(&buffer[k], "%02X ", fbrut[i]);
				}
				
				fputs(buffer, fp);

				printf("stdout: Successfully saved \"%s\"\n", filename);
			}
		}
		else
		{
			fp = fopen(filename, "wb");

			if (!fp)
			{
				fprintf(stderr, "stderr: Could not open \"%s\" for write\n", filename);
				return 0;
			}

			fbrut = dumpMemory(start, end);

			if (fbrut) {
				fwrite(fbrut, 1, end - start + 1, fp);
				printf("stdout: Successfully saved \"%s\"\n", filename);
			}
		}

		free(fbrut);

		fclose(fp);

		return 0;
	}

	return 1;
}

void saveMemory(void)
{
	type = TYPE_STRING;
	max = 1024;

	inputLoop("Enter file to save:", &saveMemoryFunc);
}

static int changePixelSizeFunc(void)
{
	setPixelSize(choice);
	printf("stdout: pixelSize=%d\n", getPixelSize());

	if (choice == 1)
		setScanlines(0);

	SDL_SetVideoMode(280 * getPixelSize(), 192 * getPixelSize(), 8, SDL_HWSURFACE | (getFullscreen() ? SDL_FULLSCREEN : 0));

	return 0;
}

void changePixelSize(void)
{
	type = TYPE_CHOICE;

	inputLoop("Choose pixel size:\nPress 1 for 1x or 2 for 2x", &changePixelSizeFunc);
}

static int changeTerminalSpeedFunc(void)
{
	int terminalSpeed = atoi(buffer);

	if (terminalSpeed < 1 || terminalSpeed > 120)
		fprintf(stderr, "stderr: Terminal speed out of range\n");
	else
	{
		setTerminalSpeed(terminalSpeed);
		printf("stdout: terminalSpeed=%d\n", getTerminalSpeed());
	}

	return 0;
}

void changeTerminalSpeed(void)
{
	type = TYPE_DECIMAL;
	max = 3;

	inputLoop("Enter terminal speed (Range: 1 - 120):", &changeTerminalSpeedFunc);
}

static int setIrqBrkVectorFunc(void)
{
	unsigned int brkVector;

	sscanf(buffer, "%4X", &brkVector);
	memWrite(0xFFFE, (unsigned char)brkVector);
	memWrite(0xFFFF, (unsigned char)(brkVector >> 8));
	printf("stdout: brkVector=%s\n", buffer);

	return 0;
}

void setIrqBrkVector(void)
{
	type = TYPE_HEXADECIMAL;
	max = 4;

	inputLoop("Enter IRQ/BRK vector:", &setIrqBrkVectorFunc);
}

void showAbout(void)
{
	SDL_Event event;
	SDL_Rect rect;
	SDL_Surface *screen = SDL_GetVideoSurface();
	int screenWidth = screen->w, screenHeight = screen->h;
	int pixelSize = getPixelSize();
	int characterWidth = 7 * pixelSize, characterHeight = 8 * pixelSize;

	rect.x = (screenWidth - (36 * characterWidth)) / 2;
	rect.y = (screenHeight - (10 * characterHeight)) / 2;
	rect.w = 36 * characterWidth;
	rect.h = 10 * characterHeight;

	SDL_FillRect(screen, &rect, 255);

	drawString(PACKAGE_NAME, (screenWidth - strlen(PACKAGE_NAME) * characterWidth) / 2, rect.y + characterHeight);
	drawString(PACKAGE_VERSION, (screenWidth - strlen(PACKAGE_VERSION) * characterWidth) / 2, rect.y + 3 * characterHeight);
	drawString("Copyright (C) 2000 Verhille Arnaud", (screenWidth - 34 * characterWidth) / 2, rect.y + 5 * characterHeight);
	drawString("Copyright (C) 2012 John D. Corrado", (screenWidth - 34 * characterWidth) / 2, rect.y + 6 * characterHeight);
	drawString("Press Esc to continue", (screenWidth - 21 * characterWidth) / 2, rect.y + 8 * characterHeight);

	SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);

	while (1)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.mod & KMOD_CTRL && event.key.keysym.sym == SDLK_q))
				exit(0);

			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
			{
				redrawScreen();
				return;
			}
		}
	}
}
