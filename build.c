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

#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#include <pthread.h>
#include <curses.h>
#include <stdbool.h>

#include "tunnel.h"

#define X2 cuboid.x2
#define Y2 cuboid.y2
#define Z2 cuboid.z2
#define X1 cuboid.x1
#define Y1 cuboid.y1
#define Z1 cuboid.z1

//One thread constantly checking if its respective bool's are true then it will build using info from
//_cuboid struct.

void *builder() {
	while (1) {
		if (paste.start) { //Paste
			int bufLoop = 6;
			char pastebuf[9000];
			//pastebuf = malloc(5000);
			FILE *chunkf = fopen(paste.chunkName, "rb");

			/*while(!feof(chunkf)) {
				fgets(pastebuf, sizeof(pastebuf), chunkf);
			}*/

			fread(pastebuf, sizeof(pastebuf), 1, chunkf);

			fclose(chunkf);

			short px = NULL;
			short py = NULL;
			short pz = NULL;

			bool hidden = false; //Stay hidden if 'hide is already toggled
				if (hide) {
					hidden = true;
				} else {
					hide = true;
				}

			memmove(&px, pastebuf, 2);
			memmove(&py, pastebuf+2, 2);
			memmove(&pz, pastebuf+4, 2);

    		px = paste.x + px - 1;
    		py = paste.y + py - 1;
    		pz = paste.z + pz - 1;

    		if (paste.x > px) { 
				short steppedOnALego = paste.x;
				paste.x = px;
				px = steppedOnALego;
			}
			if (paste.y > py) {
				short justinAndrews = paste.y;
				paste.y = py;
				py = justinAndrews;
			}
			if (paste.z > pz) {
				short cinnamon = paste.z;
				paste.z = pz;
				pz = cinnamon;
			}

			short vx = paste.x;
			short vy = paste.y;
			short vz = paste.z;
			int offset = 0;

			printw("Coords 1: %hd, %hd, %hd\n",px,py,pz);
			printw("Coords 2: %hd, %hd, %hd\n",vx,vy,vz);
    		refresh();

			while (vy <= py) {
				vx = paste.x;
				while (vx <= px) {
					vz = paste.z;
					while (vz <= pz) {
						if (paste.halt) { //Kill the motherfucker
							paste.start = false;
							break;
						}
						printw("Placing %d.\n",pastebuf[bufLoop]);
						refresh();
						if (pastebuf[bufLoop] != 0) {
							if (cuboid.zlimit == true) {
								setBlock(vx, vy, vz, pastebuf[bufLoop], true);
							    Sleep(cuboid.zspeed);
							} else {
								setBlock(vx, vy, vz, pastebuf[bufLoop], true);
							}
						}
						bufLoop++;
						vz++;
					}
					vx++;
				}
				vy++;
			}

			paste.start = false;
			if (!hidden) { //Don't unhide if already hidden before cuboid
				hide = false;
			}
			//free(pastebuf);
			//pastebuf = NULL;
		}
		if (copy.start) { //Copy
			if (copy.x1 > copy.x2) { 
				short steppedOnALego = copy.x1;
				copy.x1 = copy.x2;
				copy.x2 = steppedOnALego;
			}
			if (copy.y1 > copy.y2) {
				short justinAndrews = copy.y1;
				copy.y1 = copy.y2;
				copy.y2 = justinAndrews;
			}
			if (copy.z1 > copy.z2) {
				short cinnamon = copy.z1;
				copy.z1 = copy.z2;
				copy.z2 = cinnamon;
			}

			//Figure out how big the chunk is gonna be
			int compare = 0;
			int i = 0;
			for (i = copy.x1; i <= copy.x2; i++) {
				compare++;
			}
			short ux = compare;
			compare = 0;
			for (i = copy.y1; i <= copy.y2; i++) {
				compare++;
			}
			short uy = compare;
			compare = 0;
			for (i = copy.z1; i <= copy.z2; i++) {
				compare++;
			}
			short uz = compare;
			compare = 0;

			copy.size = ux * uy * uz + 6;

			char tMsg[64];
			char tMsg2[64];
			int mb = (copy.size/1024)/1024;
			sprintf(tMsg, "&0[&4LavaTunnel&0] &fcopy: size of area: %d Bytes, %d KB,",copy.size-6, (copy.size-6)/1024);
			sprintf(tMsg2, "%d MB",mb);
			tunnelMsg(tMsg);
			tunnelMsg(tMsg2);

			//Create a buffer with 6 bytes extra for choords which will be used to
			//determine the second point when the chunk is pasted
			char *copyBuf = malloc(copy.size);

			printw("coords: %hd, %hd, %hd\n",ux,uy,uz);
			refresh();
			memmove(copyBuf, &ux, 2);
			memmove(copyBuf+2, &uy, 2);
			memmove(copyBuf+4, &uz, 2);

			int bufLoop = 6;

			short vx = copy.x1;
			short vy = copy.y1;
			short vz = copy.z1;
			int offset = 0;

			while (vy <= copy.y2) {
				vx = copy.x1;
				while (vx <= copy.x2) {
					vz = copy.z1;
					while (vz <= copy.z2) {
						offset = calculateOffset(vx,vy,vz);
						copyBuf[bufLoop] = map[offset];
						bufLoop++;
						vz++;
					}
					vx++;
				}
				vy++;
			}

			tunnelMsg("&0[&4LavaTunnel&0] &fcopy: writing to file...");
			//char ctxt[64];
			//sprintf(ctxt, "chunks\\%s",copy.chunkName);
			FILE *chunk = fopen(copy.chunkName, "wb");
			fwrite(copyBuf, copy.size, 1, chunk);
			fclose(chunk);
			free(copyBuf);
			copy.size = 0;
			copyBuf = NULL;
			copy.start = false;
			tunnelMsg("&0[&4LavaTunnel&0] &fcopy: done!");

		}
		if (cuboid.start) {
			if (cuboid.type == 0) { //Regular cuboid
				if (cuboid.x1 > cuboid.x2) { 
					short steppedOnALego = cuboid.x1;
					cuboid.x1 = cuboid.x2;
					cuboid.x2 = steppedOnALego;
				}
				if (cuboid.y1 > cuboid.y2) {
					short justinAndrews = cuboid.y1;
					cuboid.y1 = cuboid.y2;
					cuboid.y2 = justinAndrews;
				}
				if (cuboid.z1 > cuboid.z2) {
					short cinnamon = cuboid.z1;
					cuboid.z1 = cuboid.z2;
					cuboid.z2 = cinnamon;
				}

				short vx = X1;
				short vy = Y1;
				short vz = Z1;

				//Hide is true because sending the position constantly while so many place/move packets are sent tends to cause
				//Malformed packets being sent and getting the tunnel kicked
				//^ bullshit
				bool hidden = false; //Stay hidden if 'hide is already toggled
				if (hide) {
					hidden = true;
				} else {
					hide = true;
				}
				while (vy <= Y2) {
					vx = X1;
					if (cuboid.halt) {
						cuboid.start = false;
						break;
					}
					while (vx <= X2) {
						vz = Z1;
						if (cuboid.halt) {
							cuboid.start = false;
							break;
						}
						while (vz <= Z2) {
							if (cuboid.halt) {
								cuboid.start = false;
								break;
							}
							if (map[calculateOffset(vx,vy,vz)] != cuboid.block) {
							//FUCK(map[calculateOffset(vx,vy,vz)])
								if (cuboid.zlimit == true) {
									//tunnelTeleport(vx*32,vy*32,vz*32, 0, 0);
									setBlock(vx, vy, vz, cuboid.block, false);
									Sleep(cuboid.zspeed);
								} else {
									//tunnelTeleport(vx*32,vy*32,vz*32, 0, 0);
									setBlock(vx, vy, vz, cuboid.block, true);
								}
								//FUCK(69);
							}
						vz++;
					}
					vx++;
				}
				vy++;
			}
			cuboid.start = false;
			if (!hidden) { //Don't unhide if already hidden before cuboid
				hide = false;
			}
			tunnelMsg("&0[&4LavaTunnel&0] &fz: Finished!");
			}
		}
		Sleep(2);
	}
}

char getBid(char *block) { //Take a blockname and return its block ID, if a block ID is given it will return the same BID.  Returns -1 if unknown block.
	char bid = 0;

	if (strcmp(block, "stone") == 0 || strcmp(block, "1") == 0) {
		bid = 1;
	}
	if (strcmp(block, "grass") == 0 || strcmp(block, "2") == 0) {
		bid = 2;
	}	
	if (strcmp(block, "dirt") == 0 || strcmp(block, "3") == 0) {
		bid = 3;
	}
	if (strcmp(block, "cobblestone") == 0 || strcmp(block, "4") == 0) {
		bid = 4;
	}
	if (strcmp(block, "wood") == 0 || strcmp(block, "5") == 0) {
		bid = 5;
	}
	if (strcmp(block, "tree") == 0 || strcmp(block, "6") == 0) {
		bid = 6;
	}
	if (strcmp(block, "sand") == 0 || strcmp(block, "12") == 0) {
		bid = 12;
	}
	if (strcmp(block, "gravel") == 0) {
		bid = 13;
	}

	if (strcmp(block, "goldore") == 0) {

		bid = 14;

	}

	if (strcmp(block, "ironore") == 0) {

		bid = 15;

	}

	if (strcmp(block, "coal") == 0) {

		bid = 16;

	}

	if (strcmp(block, "trunk") == 0) {

		bid = 17;

	}

	if (strcmp(block, "leaves") == 0) {

		bid = 18;

	}

	if (strcmp(block, "sponge") == 0) {

		bid = 19;

	}

	if (strcmp(block, "glass") == 0) {

		bid = 20;

	}

	if (strcmp(block, "red") == 0) {

		bid = 21;

	}

	if (strcmp(block, "orange") == 0) {

		bid = 22;

	}

	if (strcmp(block, "yellow") == 0) {

		bid = 23;

	}

	if (strcmp(block, "lightgreen") == 0) {

		bid = 24;

	}

	if (strcmp(block, "green") == 0) {

		bid = 25;

	}

	if (strcmp(block, "aqua") == 0) {

		bid = 26;

	}

	if (strcmp(block, "cyan") == 0) {

		bid = 27;

	}

	if (strcmp(block, "blue") == 0) {

		bid = 28;

	}

	if (strcmp(block, "purple") == 0) {

		bid = 29;

	}

	if (strcmp(block, "indigo") == 0) {

		bid = 30;

	}

	if (strcmp(block, "violet") == 0) {

		bid = 31;

	}

	if (strcmp(block, "magenta") == 0) {

		bid = 32;

	}

	if (strcmp(block, "pink") == 0) {

		bid = 33;

	}

	if (strcmp(block, "black") == 0) {

		bid = 34;

	}

	if (strcmp(block, "grey") == 0) {

		bid = 35;

	}

	if (strcmp(block, "white") == 0) {

		bid = 36;

	}

	if (strcmp(block, "rose") == 0) {

		bid = 38;

	}

	if (strcmp(block, "redmushroom") == 0) {

		bid = 39;

	}

	if (strcmp(block, "brownmushroom") == 0) {

		bid = 40;

	}

	if (strcmp(block, "gold") == 0) {

		bid = 41;

	}

	if (strcmp(block, "iron") == 0) {

		bid = 42;

	}

	if (strcmp(block, "stair") == 0) {

		bid = 44;

	}

	if (strcmp(block, "brick") == 0) {

		bid = 45;

	}

	if (strcmp(block, "tnt") == 0) {

		bid = 46;

	}

	if (strcmp(block, "mossycobblestone") == 0) {

		bid = 48;

	}

	if (strcmp(block, "obsidian") == 0) {

		bid = 49;

	}

	//below are blocks only supported by classicube

	if (strcmp(block, "cobblestoneslab") == 0) {
		bid = 50;
	}

	if (strcmp(block, "rope") == 0) {
		bid = 51;
	}

	if (strcmp(block, "sandstone") == 0) {
		bid = 52;
	}

	if (strcmp(block, "snow") == 0) {
		bid = 53;
	}

	if (strcmp(block, "fire") == 0) {
		bid = 54;
	}

	if (strcmp(block, "lightpink") == 0) {
		bid = 55;
	}

	if (strcmp(block, "forestgreen") == 0) {
		bid = 56;
	}

	if (strcmp(block, "brown") == 0) {
		bid = 57;
	}

	if (strcmp(block, "deepblue") == 0) {
		bid = 58;
	}

	if (strcmp(block, "turquoise") == 0) {
		bid = 59;
	}

	if (strcmp(block, "ice") == 0) {
		bid = 60;
	}

	if (strcmp(block, "ceramictile") == 0) {
		bid = 61;
	}

	if (strcmp(block, "magma") == 0) {
		bid = 62;
	}

	if (strcmp(block, "pillar") == 0) {
		bid = 63;
	}

	if (strcmp(block, "crate") == 0) {
		bid = 64;
	}

	if (strcmp(block, "stonebrick") == 0) {
		bid = 65;
	}

	if (strcmp(block, "air") == 0 || strcmp(block, "0") == 0) {
		bid = 0;
	}

	return bid;
}