lavatunnel middle man between client and server that lets you manipulate packets in Classicube, copyright (C) lavajoe 2018 for details regarding
the GNU GPL v2 lavatunnel is licensed under see the LICENSE file.

To obtain the latest sourcode of lavatunnel as well as the source for libconfig 1.5 and win32-pthreads v2.10.0-rc which I installed to compile
lavatunnel are at: https://github.com/lavajoe/lavatunnel/tree/master

The libraries you have to install to compile lavatunnel are:
	Curl
	Zlib
	pthread (sourcecode for version v2.10.0-rc that I installed to compile lavatunnel with availble in libs folder of lavatunnel source)
	pdcurses
	libconfig (sourecode for version 1.5 that I installed to compile lavatunnel with availble in libs folder of lavatunnel source)
	jsmn
	
After that you should be able to compile it with the Makefile included (mingw32-make is what I run in my case with mingw's gcc)

Upon running the .exe for the first time it should prompt you for the username/password/server URL, then once you connect to the IP/Port
it gives you type 'help in chat or in lavatunnel for a list of commands.

The license for jsmn (https://github.com/zserge/jsmn) is:

Copyright (c) 2010 Serge A. Zaitsev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

The license for curl is:

COPYRIGHT AND PERMISSION NOTICE

Copyright (c) 1996 - 2018, Daniel Stenberg, <daniel@haxx.se>, and many
contributors, see the THANKS file.

All rights reserved.

Permission to use, copy, modify, and distribute this software for any purpose
with or without fee is hereby granted, provided that the above copyright
notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN
NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of a copyright holder shall not
be used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization of the copyright holder.
