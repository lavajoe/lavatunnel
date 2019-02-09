/*
	Copyright (C) 2018  lavajoe <joe89gunnar@gmail.com)

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdbool.h>

//#ifndef _CPE_H_
//  #define _CPE_H_

/*
	These are the bools responsible for finding out which cpe extentions are supported, there are client bools (prefixed by 'c')
	which tell us if the client supports an extension and a server bool (prefixed by 's') for if the server supports them.
	if both bools are true then we can expect to use that extension where if only the client bool is true
	but the server bool is false then we don't use the exentsion/change the way the protocol works.
*/

typedef struct {
	bool blockDefs;
	bool blockDefsExt;
	bool envMapAspect;
	bool entProperty;
	bool extEntityPos; //use ints instead of shorts for addent, entteleport and extaddentity2
	bool fastMap;
	bool extText;
	bool extBlocks; //inf id!!!!!

} CPE;

extern CPE cpeClient; //What the server tells the tunnel
extern CPE cpeServer; //What the client tell the tunnel