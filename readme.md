Add "font on" session

"font on" allows you to temporarily use fonts in portable applications.
It temporarily registers all fonts in a folder.

TartetOS: Windows 2000 - 11/2025

Usage:
	fonton [options]

	"fonton": file name
		x86: fonton.exe
		x64: fonton64.exe
		ARM64: fonton64A.exe

	options:
		-r Unregisters fonts in the font folder.
		-s Does not display error messages.

	You can specify operation options by embedding them in the file name.
	Example) fonton-s-r.exe


When you run "fonton", it will temporarily register all fonts in the font folder in the folder where fonton folder. Nothing will be displayed if the operation is successful.

Temporarily registered fonts will be unregistered when you sign out, log off, shut down, or restart.

If the font folder contains fonts that cannot be registered or files other than fonts, an error will occur.
