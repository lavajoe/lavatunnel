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
#include <winsock2.h>

#ifndef _TUNNEL_H_
#define _TUNNEL_H_

#define FUCK(a) printw("FUCK: %d.\n",a); refresh();
#define bOffset(x,y,z) map[calculateOffset(x,y,z)]

//Loaded from config.cfg
const char *version; //The version of this program
const char *haxType;
const char *gimpF;
int hax;
int pdump; //1 = dump client OP codes, 2 = dump server OP codes, 3 = dump all
int useCPE; //If 0 force the client to pretend it doesnt support CPE

//Error handling and I/O wrappers
void bError(char *);
int fileExists(char *);
void simplePrint(char *msg);


//String manipulation
void toMCString(char *destination, char *source);
void strLower(char *destination, char *source);
void cleanString(char *destination, char *source);
void lavaCuck(char *destination, char *source);
bool strEqual(char *equals, char *str, bool caseSensitive);
void nullTerm(char *source);
bool commandFunc(char *msg, bool console);


//Threads
void *builder(); //Builder thread
void *tunnelServer(); //Tunnel thread that serves the client
void *mcConnect(); //Client thread


//Network
int recvW(int socket, char *buffer, int size, int flags);
int sendW(int socket, char *buffer, int size, int flags);
void tunnelMsg(char *msg);
void tunnelTeleport(short x, short y, short z, char yaw, char pitch);
void setBlock(short x, short y, short z, char bid, bool sendpos);
void tunnelSetBlock(short x, short y, short z, char bid);
void tunnelMove(short x, short y, short z, char yaw, char pitch);
void sendMessage(char pid, char *msg);


//Tunnel related stuff
SOCKET tunnelSock; //The tunnels socket that connects to the server
SOCKET serverSock; //The socket that the client will connect to
extern bool tunnelConnected; //This will remain true while the tunnel is connected to the server
extern bool clientConnected; //This will remain true while the player is connected to the tunnel


//Command related toggles
char plotBlock;
bool paint;
char paintBid;
extern bool hide;
bool gimped;
bool zPlace; //If true every block client places will become a point for a building command
int points; //The number of points


//Player data and connection info
char ipa[64];
int port;
char mppass[64];
char password[64];
char username[64];
char hash[64];
extern char motd[64];
extern char servername[64];
extern char tunnelUser[64];
char unused; //The 'unused' byte that is used for CPE in login/motd packets


//Buffers loaded from file
char ignores[64][64];
int ignoreN;
char gimp[6024][256];
int gimpN;


//Structs for holding data like coords
struct _player { //Stores data about players
    unsigned char _pid;
    char _username[64];
    unsigned short _x,_y,_z;
    unsigned char _yaw,_pitch;
    int mode; //0 = ignore, 1 = following, 2 = plotmotioning.
    bool valid; //is on the server
    bool ignore;
};
struct _player player[256];

struct _user { //Stores data about the user connected to the tunnel
    char _username[64];
    short _x,_y,_z; //Current pos known to server
    char _yaw,_pitch;
    short _hx,_hy,_hz; //Clients pos while hidden
    char _hyaw,_hpitch;
};
struct _user user;

struct _copy { //Stores info for copy
    unsigned short x1,y1,z1; //Point one
    unsigned short x2,y2,z2; //Point two
    int size;
    char chunkName[64];
    bool place;
    bool start;
    bool halt;
};
struct _copy copy;

struct _paste {
    unsigned short x,y,z;
    char chunkName[64];
    bool place;
    bool start;
    bool halt;
};
struct _paste paste;

struct _cuboid { //Stores info for cuboids
    unsigned short x1,y1,z1; //Point one
    unsigned short x2,y2,z2; //Point two
    char block, block2;
    bool place;
    bool start; //Start the cuboid
    bool halt; //Quit the cuboid
    int type; //0 = regular cuboid, 1 = replace
    bool zlimit;
    bool getBlock; //Get block from points placed or the commmand
    int zspeed; //Amount of ms the tunnel will wait between each block place
};
struct _cuboid cuboid;

//Map related stuff
extern char *mapC; //The compressed level data
extern char *map; //the level data uncompressed
extern int mapSize;
extern char *blocks;
extern unsigned short lvlx;
extern unsigned short lvly;
extern unsigned short lvlz;

int calculateOffset(short x, short y, short z);
char getBid(char *block);

#endif