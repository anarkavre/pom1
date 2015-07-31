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

static unsigned char _dspCr = 0, _dsp = 0, _kbdCr = 0, _kbd = 0x80;

void resetPia6820(void)
{
	_kbdCr = _dspCr = _dsp = 0;
	_kbd = 0x80;
}

void writeDspCr(unsigned char dspCr)
{
	_dspCr = dspCr;
}

void writeDsp(unsigned char dsp)
{
	if (!(_dspCr & 0x04))
		return;

	_dsp = dsp;
}

void writeKbdCr(unsigned char kbdCr)
{
	if (!_kbdCr)
		kbdCr = 0x27;

	_kbdCr = kbdCr;
}

void writeKbd(unsigned char kbd)
{
	_kbd = kbd;
}

unsigned char readDspCr(void)
{
	return _dspCr;
}

unsigned char readDsp(void)
{
	return _dsp;
}

unsigned char readKbdCr(void)
{
	return _kbdCr;
}

unsigned char readKbd(void)
{
	_kbdCr = 0x27;
	return _kbd;
}
