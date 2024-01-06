Pom1 is an Apple 1 emulator ported to C from the original Java
version. It uses the SDL1 library and works on most platforms.

Options
===

Pom1 has many options to configure and utilize the emulator. They are
accessed by Ctrl+<letter>. Some options also have corresponding
command line parameters.

| Option              | Letter | Parameter          | Description                                    |
|---------------------|--------|--------------------|------------------------------------------------|
| Load Memory         | L      |                    | Load memory from a binary or ascii file.       |
| Save Memory         | S      |                    | Save memory to a binary or ascii file.         |
| Quit                | Q      |                    | Quit the emulator.                             |
| Reset               | R      |                    | Soft reset the emulator.                       |
| Hard Reset          | H      |                    | Hard reset the emulator.                       |
| Pixel Size          | P      | -pixelsize <n>     | Set the pixel size (1 or 2).                   |
| Scanlines           | N      | -scanlines         | Turn scanlines on or off (pixel size 2 only).  |
| Terminal Speed      | T      | -terminalspeed <n> | Set the terminal speed (Range: 1 - 120).       |
| RAM 8K              | E      | -ram8k             | Use only 8KB of RAM or entire 64KB of RAM.     |
| Write In ROM        | W      | -writeinrom        | Allow writing data in ROM or not.              |
| IRQ/BRK Vector      | V      |                    | Set address of interrupt vector.               |
| Fullscreen          | F      | -fullscreen        | Switch to fullscreen or window.                |
| Blink Cursor        | B      | -blinkcursor       | Set the cursor to blink or not.                |
| Cursor Block        | C      | -blockcursor       | Set the cursor to block or @.                  |
| Show About          | A      |                    | Show version and copyright information.        |
| Toggle Krusader ROM | K      | -krusaderRom       | Toggle between Krusader and BASIC/Monitor ROMs |

Before exiting, Pom1 writes its configuration options to
`$HOME/.pom1/pom1.cfg`; you can modify options directly in this file as well.

Building
===

Generate the `configure` script:

	$ libtoolize --force
	$ aclocal
	$ autoheader
	$ automake --force-missing --add-missing
	$ autoconf

After that's it's just the usual:

	$ ./configure
	$ make
	$ make install

Other information
===

You can find more information about the project at the Pom1 website: http://pom1.sourceforge.net/
