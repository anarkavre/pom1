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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "screen.h"
#include "memory.h"

static char *_romdir;

void freeRomDirectory(void)
{
	free(_romdir);
}

void setRomDirectory(const char *romdir)
{
	if (_romdir)
		free(_romdir);

	_romdir = strdup(romdir);
}

const char *getRomDirectory(void)
{
	return _romdir;
}

void loadConfiguration(void)
{
	FILE *fp;
	char *configdir, *filename, buffer[256], *value;

#if !defined(_WIN32)
	char *homedir = getenv("HOME");

	if (homedir)
	{
		configdir = (char *)malloc(strlen(homedir) + 8);
		sprintf(configdir, "%s/.pom1/", homedir);
	}
	else
#endif
	configdir = strdup("");

	filename = (char *)malloc(strlen(configdir) + 9);
	sprintf(filename, "%spom1.cfg", configdir);

	free(configdir);
	
	fp = fopen(filename, "r");

	free(filename);

	if (fp)
	{
		while (!feof(fp))
		{
			if (!fgets(buffer, 256, fp))
				continue;

			if (buffer[0] == '/' || buffer[0] == '\n')
				continue;

			*(strrchr(buffer, '\n')) = '\0';
			value = strrchr(buffer, '=') + 1;
			*(strrchr(buffer, '=')) = '\0';

			if (!strcmp(buffer, "pixelSize"))
				setPixelSize(value[0] & 0x03);
			else if (!strcmp(buffer, "scanlines"))
				setScanlines(value[0] & 0x01);
			else if (!strcmp(buffer, "terminalSpeed"))
				setTerminalSpeed(atoi(value));
			else if (!strcmp(buffer, "ram8k"))
				setRam8k(value[0] & 0x01);
			else if (!strcmp(buffer, "writeInRom"))
				setWriteInRom(value[0] & 0x01);
			else if (!strcmp(buffer, "fullscreen"))
				setFullscreen(value[0] & 0x01);
			else if (!strcmp(buffer, "blinkCursor"))
				setBlinkCursor(value[0] & 0x01);
			else if (!strcmp(buffer, "blockCursor"))
				setBlockCursor(value[0] & 0x01);
			else if (!strcmp(buffer, "krusaderRom"))
			  setKrusaderRom((value[0] & 0x01));
		}

		fclose(fp);
	}
}

void saveConfiguration(void)
{
	FILE *fp;
	char *configdir, *filename, buffer[256];

#if !defined(_WIN32)
	char *homedir = getenv("HOME");

	if (homedir)
	{
		configdir = (char *)malloc(strlen(homedir) + 8);
		sprintf(configdir, "%s/.pom1/", homedir);
		mkdir(configdir, 0755);
	}
	else
#endif
	configdir = strdup("");

	filename = (char *)malloc(strlen(configdir) + 9);
	sprintf(filename, "%spom1.cfg", configdir);

	free(configdir);
	
	fp = fopen(filename, "w");

	free(filename);

	if (fp)
	{
		fputs("// Pom1 Configuration\n", fp);

		strcpy(buffer, "pixelSize=");
		buffer[10] = getPixelSize() | 0x30;
		buffer[11] = '\n';
		buffer[12] = '\0';
		fputs(buffer, fp);

		strcpy(buffer, "scanlines=");
		buffer[10] = getScanlines() | 0x30;
		buffer[11] = '\n';
		buffer[12] = '\0';
		fputs(buffer, fp);

		strcpy(buffer, "terminalSpeed=");
		sprintf(&buffer[14], "%d\n", getTerminalSpeed());
		fputs(buffer, fp);

		strcpy(buffer, "ram8k=");
		buffer[6] = getRam8k() | 0x30;
		buffer[7] = '\n';
		buffer[8] = '\0';
		fputs(buffer, fp);

		strcpy(buffer, "writeInRom=");
		buffer[11] = getWriteInRom() | 0x30;
		buffer[12] = '\n';
		buffer[13] = '\0';
		fputs(buffer, fp);

		strcpy(buffer, "fullscreen=");
		buffer[11] = getFullscreen() | 0x30;
		buffer[12] = '\n';
		buffer[13] = '\0';
		fputs(buffer, fp);

		strcpy(buffer, "blinkCursor=");
		buffer[12] = getBlinkCursor() | 0x30;
		buffer[13] = '\n';
		buffer[14] = '\0';
		fputs(buffer, fp);

		strcpy(buffer, "blockCursor=");
		buffer[12] = getBlockCursor() | 0x30;
		buffer[13] = '\n';
		buffer[14] = '\0';
		fputs(buffer, fp);

		strcpy(buffer, "krusaderRom=");
		buffer[12] = getKrusaderRom() | 0x30;
		buffer[13] = '\n';
		buffer[14] = '\0';
		fputs(buffer, fp);

		fclose(fp);
	}
}
