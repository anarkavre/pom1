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
#include "configuration.h"
#include "pia6820.h"

static unsigned char mem[65536];
static int ram8k = 0, writeInRom = 1;

static int loadMonitor(void)
{
	const char *romdir = getRomDirectory();
	char *filename;
	FILE *fp;

	filename = (char *)malloc(strlen(romdir) + 13);
	sprintf(filename, "%s/monitor.rom", romdir);

	fp = fopen(filename, "rb");

	free(filename);

	if (fp)
	{
		fread(&mem[0xFF00], 1, 256, fp);
		fclose(fp);
	}
	else
		return 0;

	return 1;
}

static int loadBasic(void)
{
	const char *romdir = getRomDirectory();
	char *filename;
	FILE *fp;

	filename = (char *)malloc(strlen(romdir) + 11);
	sprintf(filename, "%s/basic.rom", romdir);

	fp = fopen(filename, "rb");

	free(filename);

	if (fp)
	{
		fread(&mem[0xE000], 1, 4096, fp);
		fclose(fp);
	}
	else
		return 0;

	return 1;
}


void resetMemory(void)
{
	memset(mem, 0, 57344);
	
	if (!loadMonitor())
	{
		fprintf(stderr, "stderr: Could not load monitor\n");
		exit(1);
	}

	if (!loadBasic())
	{
		fprintf(stderr, "stderr: Could not load basic\n");
		exit(1);
	}
}

void setRam8k(int b)
{
	ram8k = b;
}

int getRam8k()
{
	return ram8k;
}

void setWriteInRom(int b)
{
	writeInRom = b;
}

int getWriteInRom(void)
{
	return writeInRom;
}

unsigned char memRead(unsigned short address)
{
	if (address == 0xD013)
		return readDspCr();
	if (address == 0xD012)
		return readDsp();
	if (address == 0xD011)
		return readKbdCr();
	if (address == 0xD010)
		return readKbd();

	return mem[address];
}

void memWrite(unsigned short address, unsigned char value)
{
	if (address == 0xD013)
	{
		writeDspCr(value);
		return;
	}
	if (address == 0xD012)
	{
		writeDsp((unsigned char)(value | 0x80));
		return;
	}
	if (address == 0xD011)
	{
		writeKbdCr(value);
		return;
	}
	if (address == 0xD010)
	{
		writeKbd(value);
		return;
	}

	if (address >= 0xFF00 && !writeInRom)
		return;
	if (ram8k && address >= 0x2000 && address < 0xFF00)
		return;
		
	mem[address] = value;
}

unsigned char *dumpMemory(unsigned short start, unsigned short end)
{
	unsigned char *fbrut = (unsigned char *)malloc(end - start + 1);

	if (!fbrut)
		fprintf(stderr, "stderr: Could not allocate memory block\n");
	else
		memcpy(fbrut, &mem[start], end - start + 1);

	return fbrut;
}

void setMemory(const unsigned char *data, unsigned short start, unsigned int size)
{
	memcpy(&mem[start], data, size);
}
