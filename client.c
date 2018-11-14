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
#include <math.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#include <pthread.h>
#include <curses.h>
#include <stdbool.h>
#include <zlib.h>
#include <curl/curl.h>

#include "tunnel.h"
#include "CPE.h"

char percent1;

char *mapC = NULL; //The compressed map
char *map = NULL;
char *blocks = NULL;
int mapSize = 0;

char *outputbuf = NULL;

//The size of the lvl:
unsigned short lvlx = 0;
unsigned short lvly = 0;
unsigned short lvlz = 0;

char motd[64];
char servername[64];

void levelFastFinalize(SOCKET sockfd) {
    unsigned short x,y,z;
    
    recvW(sockfd, (char *)&x, 2, 0);
    recvW(sockfd, (char *)&y, 2, 0);
    recvW(sockfd, (char *)&z, 2, 0);

    /*x = ntohs(x);
    y = ntohs(y);
    z = ntohs(z);
    x = htons(x);
    y = htons(y);
    z = htons(z);*/

    lvlx = ntohs(x);
    lvly = ntohs(y);
    lvlz = ntohs(z);

    unsigned char p = 0x04;
    sendW(serverSock, &p, 1, 0);
    unsigned char buf[6];
    memcpy(&buf[0], &x, sizeof(short));
    memcpy(&buf[2], &y, sizeof(short));
    memcpy(&buf[4], &z, sizeof(short));

    sendW(serverSock, &buf[0], 2, 0);
    sendW(serverSock, &buf[2], 2, 0);
    sendW(serverSock, &buf[4], 2, 0);

    printw("Finished downloading level data.\n");
    printw("Server level size: %hd, %hd, %hd\n",lvlx, lvly, lvlz);
    refresh();
    unsigned int realblocks = 0;

    int lx, ly, lz;

    uLong uncompressedsize = lvlx * lvly * lvlz;
    uLong four = 4;

    int i = 0;
    uLong more = uncompressedsize + 4;
    
    z_stream boners;

    int return1 = 0;

    outputbuf = malloc(uncompressedsize);

    boners.zalloc = (alloc_func)0;
    boners.zfree = (free_func)0;
    boners.opaque = (voidpf)0;
    boners.next_in = mapC;
    boners.avail_in = mapSize;
    boners.next_out = outputbuf;
    boners.avail_out = more;

    return1 = inflateInit2(&boners, -8);
    return1 = inflate(&boners, Z_FINISH);
    return1 = inflateEnd(&boners);

    if (return1 == Z_BUF_ERROR) {
        bError("ZLIB: dest buffer was not large enough.\n");
    }
    if (return1 == Z_MEM_ERROR) {
        bError("ZLIB: not enough memory\n");
    }
    if (return1 == Z_DATA_ERROR) {
        bError("ZLIB: the compressed data was corrupted.\n");
    }

    refresh();
    free(mapC); mapC = NULL; mapSize = 0;

    map = realloc(map, uncompressedsize);
    /*blocks = realloc(blocks, 4);
    memmove(blocks, outputbuf, 4);
    memmove(map, outputbuf + 4, uncompressedsize);
    free(outputbuf); outputbuf = NULL;*/
    //printw("Blocks: %d, map: %d.\n",blocks,map[20]);
    return;
}

void levelFinalize(SOCKET sockfd) {
    unsigned short x,y,z;
    
    recvW(sockfd, (char *)&x, 2, 0);
    recvW(sockfd, (char *)&y, 2, 0);
    recvW(sockfd, (char *)&z, 2, 0);

    /*x = ntohs(x);
    y = ntohs(y);
    z = ntohs(z);
    x = htons(x);
    y = htons(y);
    z = htons(z);*/

    lvlx = ntohs(x);
    lvly = ntohs(y);
    lvlz = ntohs(z);

    unsigned char p = 0x04;
    sendW(serverSock, &p, 1, 0);
    unsigned char buf[6];
    memcpy(&buf[0], &x, sizeof(short));
    memcpy(&buf[2], &y, sizeof(short));
    memcpy(&buf[4], &z, sizeof(short));

    sendW(serverSock, &buf[0], 2, 0);
    sendW(serverSock, &buf[2], 2, 0);
    sendW(serverSock, &buf[4], 2, 0);

    printw("Finished downloading level data.\n");
    printw("Server level size: %hd, %hd, %hd\n",lvlx, lvly, lvlz);
    refresh();
    unsigned int realblocks = 0;

    int lx, ly, lz;

    uLong uncompressedsize = lvlx * lvly * lvlz;
    uLong four = 4;

    int i = 0;
    uLong more = uncompressedsize + 4;
    
    z_stream boners;

    int return1 = 0;

    outputbuf = malloc(uncompressedsize+4);

    boners.zalloc = (alloc_func)0;
    boners.zfree = (free_func)0;
    boners.opaque = (voidpf)0;
    boners.next_in = mapC;
    boners.avail_in = mapSize;
    boners.next_out = outputbuf;
    boners.avail_out = more;

    return1 = inflateInit2(&boners, 31);
    return1 = inflate(&boners, Z_FINISH);
    return1 = inflateEnd(&boners);

    if (return1 == Z_BUF_ERROR) {
        bError("ZLIB: dest buffer was not large enough.\n");
    }
    if (return1 == Z_MEM_ERROR) {
        bError("ZLIB: not enough memory\n");
    }
    if (return1 == Z_DATA_ERROR) {
        bError("ZLIB: the compressed data was corrupted.\n");
    }

    refresh();
    free(mapC); mapC = NULL; mapSize = 0;

    map = realloc(map, uncompressedsize);
    blocks = realloc(blocks, 4);
    memmove(blocks, outputbuf, 4);
    memmove(map, outputbuf + 4, uncompressedsize);
    free(outputbuf); outputbuf = NULL;
    //printw("Blocks: %d, map: %d.\n",blocks,map[20]);
    return;
}

void levelData(SOCKET sockfd) {

    unsigned char percent2 = 0;
    short chunk_len;
    int recvd = 0;
    char garbageBuffer[1024];
    char *pass = NULL;
    recvW(sockfd, (char *)&chunk_len, 2, 0);
    short chunk_lenhost = ntohs(chunk_len);

    mapSize += chunk_lenhost;

    char data[chunk_lenhost];
    mapC = realloc(mapC, mapSize+chunk_lenhost);
    pass = malloc(chunk_lenhost);
    int ret = recvW(sockfd, pass, chunk_lenhost, 0);
 
    /*FILE *dick, *bong;
    dick = fopen("mapc.gzip", "wb");
    fwrite(mapC, sizeof(char), 1024, dick);
    fclose(dick);*/

    if (chunk_lenhost < 1024) {
        recvW(sockfd, garbageBuffer, 1024-chunk_lenhost, 0);
    }
    recvW(sockfd, &percent2, 1, 0);

    memmove(mapC+mapSize-chunk_lenhost, pass, chunk_lenhost);

    unsigned char p = 0x03;
    sendW(serverSock, &p, 1, 0);
    unsigned char buf[2];
    memcpy(&buf[0], &chunk_len, sizeof(short));
    sendW(serverSock, &buf[0], 2, 0);
    sendW(serverSock, pass, ret, 0);
    if (chunk_lenhost < 1024) {
        sendW(serverSock, garbageBuffer, 1024-ret, 0);
    }

    sendW(serverSock, &percent2, 1, 0);
    free(pass);
    pass = NULL;
}

int lfuck = 1;

void levelData2(SOCKET sockfd) {
    char percent;
    short chunkLength;
    char *pass = NULL;
    char shit[1024];

    pass = malloc(1024);

    //Recv from server

    recvW(sockfd, (char *)&chunkLength, 2, 0);
    short chunkHost = ntohs(chunkLength);
    mapSize += chunkHost;
    //mapC = realloc(mapC, mapSize);
    int ret = recvW(sockfd, pass, chunkHost, 0);
    if (chunkHost < 1024) {
        recvW(sockfd, shit, 1024-chunkHost, 0);
    }
    recv(sockfd, &percent, 1, 0);
	printw("Size: %d.\n",mapSize);
	refresh();
	
    unsigned char buf[1];
    memcpy(&buf[0], &chunkLength, sizeof(short));

    int penisShit = mapSize - 1024;
    //printw("%d",mapC);
    //refresh();
    //memcpy(pass, mapC, 1024);

    //Send to client
    char id = 0x03;
    sendW(serverSock, &id, 1, 0);
    sendW(serverSock, &buf[0], 2, 0);
    sendW(serverSock, pass, chunkHost, 0);
    sendW(serverSock, &percent, 1, 0);

    mapC = realloc(mapC, mapSize);
	memcpy(&mapC+mapSize, &pass, chunkHost);
    //Store the leveldata after its been sent to the client

    free(pass);
    pass = NULL;
}

void *mcConnect() {

	WSADATA wsa;
    WORD Version = MAKEWORD(2, 2);

    WSAStartup(Version, &wsa);

    SOCKADDR_IN networkshit;

    struct hostent *fuck;

    fuck = gethostbyname(ipa);

    networkshit.sin_addr.s_addr = *((unsigned long*)fuck->h_addr);
    networkshit.sin_family = AF_INET;
    networkshit.sin_port = htons(port);

    tunnelSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (tunnelSock == SOCKET_ERROR) {
		bError("Failed to create connection socket... You're really fucked now.");
	}

	printw("\n[LavaTunnel] Connecting...");
	refresh();
    bool tunnelConnected = false;
	if (connect(tunnelSock, (SOCKADDR*)&networkshit, sizeof(networkshit)) == SOCKET_ERROR) {
        bError("Failed to connect!");
    } else {
    	simplePrint("Connected!\n");
        tunnelConnected = true;
    }

    simplePrint("[LavaTunnel] Logging in to server...");

    char pType = 0x00;
    char version = 7;
    char fucker = 0;

    char mcMpass[64];
    char userName[64];

    if (useCPE == 0) {
        unused = 0; //Disables CPE
    }
    
    toMCString(mcMpass, mppass);
    toMCString(userName, username);
    sendW(tunnelSock, &pType, 1, 0);
    sendW(tunnelSock, &version, 1, 0);
    sendW(tunnelSock, userName, 64, 0);
    sendW(tunnelSock, mcMpass, 64, 0);
    sendW(tunnelSock, &unused, 1, 0);

    bool gotMotd = false;
    bool joined = false;
    int returnVal = 0;
    unsigned char id;

    /*FILE *pack;
    pack = fopen("packets.txt", "w+");
    fprintf(pack, "\n\n:BEGIN:\n");*/
    while (tunnelConnected == true) {
    	returnVal = recvW(tunnelSock, &id, 1, 0);
        //printw("Server: %d.\n",id);
        //refresh();
        if (returnVal < 1) {
            tunnelConnected = false;
            //Make sure the client connected to then tunnel gets kicked when the tunnel loses connection
            id = 0x0e;
            send(serverSock, &id, 1, 0);
            char msg[64];
            sprintf(msg, "[LavaTunnel] Disconnected!");
            toMCString(msg, msg);
            sendW(serverSock, msg, 64, 0);
            Sleep(10);
            closesocket(serverSock);
            closesocket(tunnelSock);
            WSACleanup();
            bError("Tunnel disconnected!");
            cuboid.halt = true;
        }

        //fprintf(pack, "ID: %d.\n", id);

        /*switch(id) {
            case 16:
                simplePrint("ExtInfo\n");
                break;
            case 17:
                simplePrint("ExtEntry\n");
                break;
            case 22:
                simplePrint("Extaddplayername\n");
                break;
            case 33:
                simplePrint("extaddentity2\n");
                break;
            case 24:
                simplePrint("extremoveplayername\n");
                break;
            case 30:
                simplePrint("envsetmap\n");
                break;
            case 32:
                simplePrint("hackcontrol\n");
                break;
            case 35:
                simplePrint("defineblock\n");
                break;
            case 36:
                simplePrint("removedefineblock\n");
                break;
        }*/

        /*if (id > 41) {
        	char cunt;
        	recv(tunnelSock, &cunt, 1, 0);
        }*/

        if (id == 0x00) { //MOTD
            char cmotd[64];
            char cservername[64];
            char shit;
            recvW(tunnelSock, &shit, 1, 0);
            recvW(tunnelSock, cservername, 64, 0);
            recvW(tunnelSock, cmotd, 64, 0);
            recv(tunnelSock, &fucker, 1 , 0);

            char haxMotd[64];
            char haxServername[64];

            //Remove bullshit like -hax from motd/servername so flying is possible
            if (hax == 1) {
                char buf1[64];
                char buf2[64];
                sprintf(buf1, haxType);
                toMCString(haxMotd, buf1);
                sprintf(buf2, haxType);
                toMCString(haxServername, buf2);
            } else {
                strncpy(haxServername, cservername, 64);
                strncpy(haxMotd, cmotd, 64);
            }

            shit = 0x00;
            sendW(serverSock, &shit, 1, 0);
            shit = 0x07;
            sendW(serverSock, &shit, 1, 0);
            sendW(serverSock, haxServername, 64, 0);
            sendW(serverSock, haxMotd, 64, 0);
            send(serverSock, &fucker, 1 , 0);

            strncpy(motd, cmotd, 64);
            strncpy(servername, cservername, 64);

            //printw("Name: %s.\n",servername);
            //refresh();
            //getchar();

            if (gotMotd == false) { /*theres alot of 'bullshit' that gets sent after we get the real motd
                                      so after we get it the first time we ignore all 'motd' packets.*/
                gotMotd = true;
            }
        }
        if (id == 0x01) { sendW(serverSock, &id, 1, 0); } //Pinged
        if (id == 0x02) { //Level int
            printw("fuck\n");
            refresh();
            /*if (sfastMap == true && cfastMap == true) {
                simplePrint("Using fastmap\n");
                char buf[4];
                int size;
                recvW(tunnelSock, (char*)&size, 4, 0);

                memcpy(&buf[0], &size, sizeof(int));

                sendW(serverSock, &id, 1, 0);
                sendW(serverSock, &buf[0], 4, 0);
            } else {*/
                sendW(serverSock, &id, 1, 0);
            //}
            //mapC = malloc(1024);
        } 
        if (id == 0x03) { //Level data
            levelData(tunnelSock);
        }
        if (id == 0x04) { //Level finalize
            levelFinalize(tunnelSock);
            if (joined == false) { 
                simplePrint("Sucess!");
                tunnelMsg("&0[&4LavaTunnel&0] &fType 'help for commands.");
                simplePrint("\n[LavaTunnel] Press t to type. (use 'help for commands)\n");
            }
            joined = true;
        }
        if (id == 0x06) { //Set block
            unsigned short x,y,z;
            char block;
            recvW(tunnelSock, (char *)&x, 2, 0);
            recvW(tunnelSock, (char *)&y, 2, 0);
            recvW(tunnelSock, (char *)&z, 2, 0);
            recvW(tunnelSock, &block, 1, 0);

            unsigned char buf[6];

            memcpy(&buf[0], &x, sizeof(short));
            memcpy(&buf[2], &y, sizeof(short));
            memcpy(&buf[4], &z, sizeof(short));

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &buf[0], 2, 0);
            sendW(serverSock, &buf[2], 2, 0);
            sendW(serverSock, &buf[4], 2, 0);
            sendW(serverSock, &block, 1, 0);

            int offset = calculateOffset(ntohs(x),ntohs(y),ntohs(z));

            //simplePrint("Updating map...\n");
            map[offset] = block;
            //printw("Done: %d.\n",map[offset]);
            //refresh();
        }
        if (id == 0x07) { //Spawn player
            char username[64];
            unsigned char pid;
            char yaw, pitch;
            unsigned short x,y,z;
            recvW(tunnelSock, &pid, 1, 0);
            recvW(tunnelSock, username, 64, 0);
            recvW(tunnelSock, (char *)&x, 2, 0);
            recvW(tunnelSock, (char *)&y, 2, 0);
            recvW(tunnelSock, (char *)&z, 2, 0);
            recvW(tunnelSock, &yaw, 1, 0);
            recvW(tunnelSock, &pitch, 1, 0);
            player[pid]._pid = pid;
            char cleanName[64];
            cleanString(player[pid]._username, username);
            //strncpy(player[pid]._username, cleanName, strlen(cleanName));
            player[pid]._x = ntohs(x);
            player[pid]._y = ntohs(y);
            player[pid]._z = ntohs(z);
            player[pid]._yaw = yaw;
            player[pid]._pitch = pitch;

            int i = 0;
            for (i = 0; i <= ignoreN; i++) {
                if (strEqual(player[pid]._username, ignores[i], true) == true) {
                    player[pid].ignore = true;
                }
            }
            player[pid].valid = true;

            //printw("spawned player %s with pid of %d.\n",player[pid]._username, player[pid]._pid);
            //refresh();

            unsigned char buf[6];

            memcpy(&buf[0], &x, sizeof(short));
            memcpy(&buf[2], &y, sizeof(short));
            memcpy(&buf[4], &z, sizeof(short));

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &pid, 1, 0);
            sendW(serverSock, username, 64, 0);
            sendW(serverSock, &buf[0], 2, 0);
            sendW(serverSock, &buf[2], 2, 0);
            sendW(serverSock, &buf[4], 2, 0);
            sendW(serverSock, &yaw, 1, 0);
            sendW(serverSock, &pitch, 1, 0);
        }
        if (id == 0x08) { //Pos and orientation (tunnelTeleport)
            unsigned char pid;
            char yaw, pitch;
            unsigned short x,y,z;
            recvW(tunnelSock, &pid, 1, 0);
            recvW(tunnelSock, (char *)&x, 2, 0);
            recvW(tunnelSock, (char *)&y, 2, 0);
            recvW(tunnelSock, (char *)&z, 2, 0);
            recvW(tunnelSock, &yaw, 1, 0);
            recvW(tunnelSock, &pitch, 1, 0);

            player[pid]._x = ntohs(x);
            player[pid]._y = ntohs(y);
            player[pid]._z = ntohs(z);
            player[pid]._yaw = yaw;
            player[pid]._pitch = pitch;

            unsigned char buf[6];

            memcpy(&buf[0], &x, sizeof(short));
            memcpy(&buf[2], &y, sizeof(short));
            memcpy(&buf[4], &z, sizeof(short));

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &pid, 1, 0);
            sendW(serverSock, &buf[0], 2, 0);
            sendW(serverSock, &buf[2], 2, 0);
            sendW(serverSock, &buf[4], 2, 0);
            sendW(serverSock, &yaw, 1, 0);
            sendW(serverSock, &pitch, 1, 0);


            if (player[pid].mode == 1) {
                tunnelTeleport(player[pid]._x,player[pid]._y,player[pid]._z, player[pid]._yaw, player[pid]._pitch);
            }
            if (player[pid].mode == 2) {
            	tunnelTeleport(player[pid]._x,player[pid]._y,player[pid]._z, player[pid]._yaw, player[pid]._pitch);
                setBlock(player[pid]._x/32,player[pid]._y/32,player[pid]._z/32, plotBlock, true);
            }
        }
        if (id == 0x09) {
            unsigned char pid;
            char x,y,z,yaw,pitch;
            recvW(tunnelSock, &pid, 1, 0);
            recvW(tunnelSock, &x, 1, 0);
            recvW(tunnelSock, &y, 1, 0);
            recvW(tunnelSock, &z, 1, 0);
            recvW(tunnelSock, &yaw, 1, 0);
            recvW(tunnelSock, &pitch, 1, 0);

            player[pid]._x = player[pid]._x + x;
            player[pid]._y = player[pid]._y + y;
            player[pid]._z = player[pid]._z + z;
            player[pid]._yaw = yaw;
            player[pid]._pitch = pitch;

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &pid, 1, 0);
            sendW(serverSock, &x, 1, 0);
            sendW(serverSock, &y, 1, 0);
            sendW(serverSock, &z, 1, 0);
            sendW(serverSock, &yaw, 1, 0);
            sendW(serverSock, &pitch, 1, 0);

            if (player[pid].mode == 1) {
                tunnelTeleport(player[pid]._x,player[pid]._y,player[pid]._z, player[pid]._yaw, player[pid]._pitch);
            }
            if (player[pid].mode == 2) {
            	tunnelTeleport(player[pid]._x,player[pid]._y,player[pid]._z, player[pid]._yaw, player[pid]._pitch);
                setBlock(player[pid]._x/32,player[pid]._y/32,player[pid]._z/32, plotBlock, true);
            }
        }
        if (id == 0x0a) {
            unsigned char pid;
            char x,y,z;
            recvW(tunnelSock, &pid, 1, 0);
            recvW(tunnelSock, &x, 1, 0);
            recvW(tunnelSock, &y, 1, 0);
            recvW(tunnelSock, &z, 1, 0);

            player[pid]._x = player[pid]._x + x;
            player[pid]._y = player[pid]._y + y;
            player[pid]._z = player[pid]._z + z;

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &pid, 1, 0);
            sendW(serverSock, &x, 1, 0);
            sendW(serverSock, &y, 1, 0);
            sendW(serverSock, &z, 1, 0);
            
            if (player[pid].mode == 1) {
                tunnelTeleport(player[pid]._x,player[pid]._y,player[pid]._z, player[pid]._yaw, player[pid]._pitch);
            }
            if (player[pid].mode == 2) {
            	tunnelTeleport(player[pid]._x,player[pid]._y,player[pid]._z, player[pid]._yaw, player[pid]._pitch);
                setBlock(player[pid]._x/32,player[pid]._y/32,player[pid]._z/32, plotBlock, true);
            }
        }
        if (id == 0x0b) {
            unsigned char pid;
            char yaw,pitch;
            recvW(tunnelSock, &pid, 1, 0);
            recvW(tunnelSock, &yaw, 1, 0);
            recvW(tunnelSock, &pitch, 1, 0);

            player[pid]._yaw = yaw;
            player[pid]._pitch = pitch;

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &pid, 1, 0);
            sendW(serverSock, &yaw, 1, 0);
            sendW(serverSock, &pitch, 1, 0);
        }
        if (id == 0x0c) { //Player disconnect
            unsigned char pid;
            recvW(tunnelSock, &pid, 1, 0);

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &pid, 1, 0);

            player[pid].valid = false;
            player[pid].mode = 0;
        }
        if (id == 0x0d) { //Chat message 
            unsigned char pid;
            unsigned char msg[64];
            recvW(tunnelSock, &pid, 1, 0);
            recvW(tunnelSock, msg, 64, 0);

            char cmsg[64];
            cleanString(cmsg, msg);

            //printw("%s\n", player[pid].ignore ? "true" : "false");
            //refresh();

            int i = 0;
            bool ignoremsg = false;

            

            for (i = 0; i < 256; i++) {
                if (strstr(cmsg, player[i]._username) != NULL) {
                    if (player[i].ignore == true) {
                        ignoremsg = true;
                    }
                }
            }

            if (!ignoremsg) {
                char p = 0x0d;
                sendW(serverSock, &p, 1, 0);
                sendW(serverSock, &pid, 1, 0);
                sendW(serverSock, msg, 64, 0);
            }
        }
        if (id == 0x0e) { //Kick
            unsigned char msg[64];
            recvW(tunnelSock, msg, 64, 0);
            cuboid.halt = true;
            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, msg, 64, 0);
            Sleep(20); //Make sure the client gets the kick packet before we cut the connection
            memset(msg+64, '\0', 1);
            tunnelConnected = false;
            closesocket(serverSock);
            closesocket(tunnelSock);
            WSACleanup();
            printw("\n\nKicked: %s\n\n",msg);
            printw("Press any key to the close program...");
            refresh();
            getch();
            endwin();
            exit(1);
        }
        if (id == 0x0f) { //Usertype
            char usertype;
            recvW(tunnelSock, &usertype, 1, 0);

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &usertype, 1, 0);
        }

        //CPE:

        if (id == 0x10) { //ExtInfo
            char app[64];
            short count;

            recvW(tunnelSock, app, 64, 0);
            recvW(tunnelSock, (char*)&count, 2, 0);
            char cApp[64];
            toMCString(cApp, app);
            //printw("\nServer software: %d\n",cApp);
           // refresh();
            count = ntohs(count);
            count=count;
            count = htons(count);
            char buf[1];

            memcpy(&buf[0], &count, sizeof(short));

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, app, 64, 0);
            sendW(serverSock, &buf[0], 2, 0);
        }

        if (id == 0x11) { //ExtEntry
            char extname[64];
            int version;

            recvW(tunnelSock, extname, 64, 0);
            recvW(tunnelSock, (char*)&version, 4, 0);

            char buf[4];

            memcpy(&buf[0], &version, sizeof(int));

            //if (strstr(extname, "EnvMapAppearence") != NULL) {
            //} //else {

            char nextname[64];
            strncpy(nextname, extname, 64);
            nullTerm(nextname);

            //if (strcmp(nextname, "FastMap") == 0 || strcmp(nextname, "ExtEntityPositions") == 0 || strcmp(nextname, "ExtendedBlocks") == 0) {
                //sfastMap = true;
            //} else {
                /*if (strcmp(nextname, "ExtendedBlocks") == 0) {
                    simplePrint("ignoring customblocks serverside");
                } else {*/
                sendW(serverSock, &id, 1, 0);
                sendW(serverSock, extname, 64, 0);
                sendW(serverSock, &buf[0], 4, 0);
            //}
            //}

                /*printw("ENTRY: %s.\n",extname);
                printw("version: %d.\n",ntohl(version));
                refresh();*/
                //simplePrint("\n\n!!!!!!Map!!!!!!!\n\n");
        }

        if (id == 0x12) { //Click distance
            short distance;

            recvW(tunnelSock, (char*)&distance, 2, 0);

            unsigned char buf[2];

            memcpy(&buf[0], &distance, sizeof(short));

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &buf[0], 2, 0);
        }

        if (id == 0x13) { //Custom blocks
            unsigned char level;
            recv(tunnelSock, &level, 1, 0);

            send(serverSock, &id, 1, 0);
            send(serverSock, &level, 1, 0);
        }

        if (id == 0x14) { //Hold this
            char block,prevent;

            recv(tunnelSock, &block, 1, 0);
            recv(tunnelSock, &prevent, 1, 0);

            //Ignore this retarded packet

            /*send(serverSock, &id, 1, 0);
            send(serverSock, &block, 1, 0);
            send(serverSock, &prevent, 1, 0);*/
        }

        if (id == 0x15) { //Text hot key
            char label[64];
            char action[64];
            int keycode;
            char keymods;

            recvW(tunnelSock, label, 64, 0);
            recvW(tunnelSock, action, 64, 0);
            recv(tunnelSock, (char*)&keycode, 4, 0);
            recv(tunnelSock, &keymods, 1, 0);

            char buf[3];
            memcpy(&buf[0], &keycode, sizeof(int));

            send(serverSock, &id, 1, 0);
            sendW(serverSock, label, 64, 0);
            sendW(serverSock, action, 64, 0);
            send(serverSock, &buf[0], 4, 0);
            send(serverSock, &keymods, 1, 0);
        }

        if (id == 0x16) { //ExtAddPlayerName
            short nameid;
            unsigned char playername[64];
            unsigned char listname[64];
            unsigned char groupname[64];
            unsigned char grouprank;

            recvW(tunnelSock, (char*)&nameid, 2, 0);
            recvW(tunnelSock, playername, 64, 0);
            recvW(tunnelSock, listname, 64, 0);
            recvW(tunnelSock, groupname, 64, 0);
            recvW(tunnelSock, &grouprank, 1, 0);

            short nameidH = ntohs(nameid);

            player[nameidH]._pid = (char)nameidH;
            char clean[64];
            cleanString(clean, playername);
            strcpy(player[nameidH]._username, clean);
            player[nameidH].valid = true;

            //printw("Added %s.\nPID: %hd.\n",player[nameidH]._username, nameidH);
            //refresh();
            unsigned char buf[2];
            memcpy(&buf[0], &nameid, sizeof(short));

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &buf[0], 2, 0);
            sendW(serverSock, playername, 64, 0);
            sendW(serverSock, listname, 64, 0);
            sendW(serverSock, groupname, 64, 0);
            sendW(serverSock, &grouprank, 1, 0);
        }

        if (id == 0x21) { //ExtAddEntity2
            char entityid;
            unsigned char ingame[64];
            unsigned char skinname[64];
            unsigned short x,y,z;
            char pitch,yaw;

            recvW(tunnelSock, &entityid, 1, 0);
            recvW(tunnelSock, ingame, 64, 0);
            recvW(tunnelSock, skinname, 64, 0);
            recvW(tunnelSock, (char*)&x, 2, 0);
            recvW(tunnelSock, (char*)&y, 2, 0);
            recvW(tunnelSock, (char*)&z, 2, 0);
            recvW(tunnelSock, &yaw, 1, 0);
            recvW(tunnelSock, &pitch, 1, 0);

            unsigned char buf[6];
            memcpy(&buf[0], &x, sizeof(short));
            memcpy(&buf[2], &y, sizeof(short));
            memcpy(&buf[4], &z, sizeof(short));

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &entityid, 1, 0);
            sendW(serverSock, ingame, 64, 0);
            sendW(serverSock, skinname, 64, 0);
            sendW(serverSock, &buf[0], 2, 0);
            sendW(serverSock, &buf[2], 2, 0);
            sendW(serverSock, &buf[4], 2, 0);
            sendW(serverSock, &yaw, 1, 0);
            sendW(serverSock, &pitch, 1, 0);

            char pid = entityid;

            player[pid]._pid = pid;
            player[pid]._x = ntohs(x);
            player[pid]._y = ntohs(y);
            player[pid]._z = ntohs(z);
            player[pid]._yaw = yaw;
            player[pid]._pitch = pitch;

            char mrclean[64];
            cleanString(mrclean, skinname);
            strcpy(player[pid]._username, mrclean);

            int i = 0;

            for (i = 0; i <= ignoreN; i++) {
                if (strEqual(player[pid]._username, ignores[i], true) == true) {
                    player[pid].ignore = true;
                }
            }

            player[pid].valid = true;

            //printw("adding: %d.\n",pid);
            //refresh();
            //printw("spawning %s.\nat %hd %hd %hd.\n",player[pid]._username,ntohs(x),ntohs(y),ntohs(z));
            //refresh();
        }

        if (id == 0x18) { //ExtRemovePlayerName
            short nameid;

            recvW(tunnelSock, (char*)&nameid, 2, 0);

            char buf[2];
            memcpy(&buf[0], &nameid, sizeof(short));

            send(serverSock, &id, 1, 0);
            send(serverSock, &buf[0], 2, 0);

            char pid = nameid;
            player[pid].valid = false;
        }

        if (id == 0x19) { //EnvSetColor
            char var;
            short red,green,blue;

            recv(tunnelSock, &var, 1, 0);
            recv(tunnelSock, (char*)&red, 2, 0);
            recv(tunnelSock, (char*)&green, 2, 0);
            recv(tunnelSock, (char*)&blue, 2, 0);

            char buf[6];
            memcpy(&buf[0], &red, sizeof(short));
            memcpy(&buf[2], &green, sizeof(short));
            memcpy(&buf[4], &blue, sizeof(short));

            send(serverSock, &id, 1, 0);
            send(serverSock, &var, 1, 0);
            send(serverSock, &buf[0], 2, 0);
            send(serverSock, &buf[2], 2, 0);
            send(serverSock, &buf[4], 2, 0);
        }

        if (id == 0x1A) { //MakeSelection
            char selectionid;
            char label[64];
            short startx,starty,startz;
            short endx,endy,endz;
            short red,green,blue;
            short opacity;

            recv(tunnelSock, &selectionid, 1, 0);
            recv(tunnelSock, label, 64, 0);
            recv(tunnelSock, (char*)&startx, 2, 0);
            recv(tunnelSock, (char*)&starty, 2, 0);
            recv(tunnelSock, (char*)&startz, 2, 0);
            recv(tunnelSock, (char*)&endx, 2, 0);
            recv(tunnelSock, (char*)&endy, 2, 0);
            recv(tunnelSock, (char*)&endz, 2, 0);
            recv(tunnelSock, (char*)&red, 2, 0);
            recv(tunnelSock, (char*)&green, 2, 0);
            recv(tunnelSock, (char*)&blue, 2, 0);
            recv(tunnelSock, (char*)&opacity, 2, 0);

            char buf1[6];
            char buf2[6];
            char buf3[6];
            char buf4[1];

            memcpy(&buf1[0], &startx, sizeof(short));
            memcpy(&buf1[2], &starty, sizeof(short));
            memcpy(&buf1[4], &startz, sizeof(short));

            memcpy(&buf2[0], &endx, sizeof(short));
            memcpy(&buf2[2], &endy, sizeof(short));
            memcpy(&buf2[4], &endz, sizeof(short));

            memcpy(&buf3[0], &red, sizeof(short));
            memcpy(&buf3[2], &green, sizeof(short));
            memcpy(&buf3[4], &blue, sizeof(short));

            memcpy(&buf4[0], &opacity, sizeof(short));

            send(serverSock, &id, 1, 0);
            send(serverSock, &selectionid, 1, 0);
            send(serverSock, label, 64, 0);
            send(serverSock, &buf1[0], 2, 0);
            send(serverSock, &buf1[2], 2, 0);
            send(serverSock, &buf1[4], 2, 0);

            send(serverSock, &buf2[0], 2, 0);
            send(serverSock, &buf2[2], 2, 0);
            send(serverSock, &buf2[4], 2, 0);

            send(serverSock, &buf3[0], 2, 0);
            send(serverSock, &buf3[2], 2, 0);
            send(serverSock, &buf3[4], 2, 0);

            send(serverSock, &buf4[0], 2, 0);

        }

        if (id == 0x1B) { //RemoveSelection
            char selection;

            recv(tunnelSock, &selection, 1, 0);

            send(serverSock, &id, 1, 0);
            send(serverSock, &selection, 1, 0);
        }

        if (id == 0x1C) { //SetBlockPermission
            char block,allowplace,allowdelete;

            recvW(tunnelSock, &block, 1, 0);
            recvW(tunnelSock, &allowplace, 1, 0);
            recvW(tunnelSock, &allowdelete, 1, 0);

            if (hax == 1) {
            	allowplace = 1;
            	allowdelete = 1;
            }

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &block, 1, 0);
            sendW(serverSock, &allowplace, 1, 0);
            sendW(serverSock, &allowdelete, 1, 0);
        }

        if (id == 0x1D) { //ChangeModel
            char entityid;
            char modelname[64];

            recv(tunnelSock, &entityid, 1, 0);
            recvW(tunnelSock, modelname, 64, 0);

            send(serverSock, &id, 1, 0);
            send(serverSock, &entityid, 1, 0);
            sendW(serverSock, modelname, 64, 0);
        }

        if (id == 30) { //EnvMapAppearence
            char url[64];
            char sideblock,edgeblock;
            short sidelevel,cloudlevel,view;
            recvW(tunnelSock, url, 64, 0);
            recvW(tunnelSock, &sideblock, 1, 0);
            recvW(tunnelSock, &edgeblock, 1, 0);
            recvW(tunnelSock, (char*)&sidelevel, 2, 0);
            recvW(tunnelSock, (char*)&cloudlevel, 2, 0);
            recvW(tunnelSock, (char*)&view, 2, 0);

            printw("URL: %s.\n",url);
            refresh();
            char buf[6];
            memcpy(&buf[0], &sidelevel, sizeof(short));
            memcpy(&buf[2], &cloudlevel, sizeof(short));
            memcpy(&buf[4], &view, sizeof(short));

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, url, 64, 0);
            sendW(serverSock, &sideblock, 1, 0);
            sendW(serverSock, &edgeblock, 1, 0);
            sendW(serverSock, &buf[0], 2, 0);
            sendW(serverSock, &buf[2], 2, 0);
            sendW(serverSock, &buf[4], 2, 0);
        }

        if (id == 0x1F) { //EnvSetWeatherType
            char weathertype;

            recvW(tunnelSock, &weathertype, 1, 0);

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &weathertype, 1, 0);
        }

        if (id == 0x20) { //HackCuntrol
            char flying,noclip,speeding,spawncontrol,thirdperson,walkspeed,runspeed,respawn,pov,superj;
            char jumpheight;

            recvW(tunnelSock, &flying, 1, 0);
            recvW(tunnelSock, &noclip, 1, 0);
            recvW(tunnelSock, &walkspeed, 1, 0);
            recvW(tunnelSock, &runspeed, 1, 0);
            recvW(tunnelSock, &respawn, 1, 0);
            recvW(tunnelSock, &pov, 1, 0);
            //recvW(tunnelSock, &speeding, 1, 0);
            recvW(tunnelSock, &jumpheight, 1, 0);
            recvW(tunnelSock, &superj, 1, 0);
            //recvW(tunnelSock, &thirdperson, 1, 0);
            //recvW(tunnelSock, (char*)&jumpheight, 2, 0);

            //Ignore server attempts to disable hax
            if (hax == 1) {
				pov = 3;
				respawn = 1;
				runspeed = 255;
				walkspeed = 255;
                flying = 255;
                noclip = 1;
                speeding = 1;
                spawncontrol = 1;
                thirdperson = 1;
                //jumpheight = ntohs(jumpheight);
                //jumpheight = -1;
//jumpheight = htons(jumpheight);
				jumpheight = 255;
				superj = 255;
				
            }
            
           // jumpheight = htons(jumpheight);
            char buf[2];
            memcpy(&buf[0], &jumpheight, sizeof(short));
            //id = 32;
            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &flying, 1, 0);
            sendW(serverSock, &noclip, 1, 0);
            sendW(serverSock, &walkspeed, 1, 0);
            sendW(serverSock, &runspeed, 1, 0);
            sendW(serverSock, &respawn, 1, 0);
            sendW(serverSock, &pov, 1, 0);
            sendW(serverSock, &jumpheight, 1, 0);
            sendW(serverSock, &superj, 1, 0);
            //sendW(serverSock, &speeding, 1, 0);
            //sendW(serverSock, &spawncontrol, 1, 0);
            //sendW(serverSock, &thirdperson, 1, 0);
            //sendW(serverSock, &buf[0], 2, 0);
           
        }

        if (id == 34) {
        	char button,action,targetid,targetblockface;
        	short yaw,pitch,targetblockx,targetblocky,targetblockz;

        	recv(tunnelSock, &button, 1, 0);
        	recv(tunnelSock, &action, 1, 0);
        	recv(tunnelSock, (char*)&yaw, 2, 0);
        	recv(tunnelSock, (char*)&pitch, 2, 0);
        	recv(tunnelSock, &targetid, 1, 0);
        	recv(tunnelSock, (char*)&targetblockx, 2, 0);
        	recv(tunnelSock, (char*)&targetblocky, 2, 0);
        	recv(tunnelSock, (char*)&targetblockz, 2, 0);
        	recv(tunnelSock, &targetblockface, 1, 0);

        	char buf1[4];

        	memcpy(&buf1[0], &yaw, sizeof(short));
        	memcpy(&buf1[2], &pitch, sizeof(short));

        	char buf2[6];

        	memcpy(&buf2[0], &targetblockx, sizeof(short));
        	memcpy(&buf2[2], &targetblocky, sizeof(short));
        	memcpy(&buf2[4], &targetblockz, sizeof(short));

			send(serverSock, &id, 1, 0);
			send(serverSock, &button, 1, 0);
        	send(serverSock, &action, 1, 0);
        	send(serverSock, &buf1[0], 2, 0);
        	send(serverSock, &buf1[2], 2, 0);
        	send(serverSock, &targetid, 1, 0);
        	send(serverSock, &buf2[0], 2, 0);
        	send(serverSock, &buf2[2], 2, 0);
        	send(serverSock, &buf2[4], 2, 0);
        	send(serverSock, &targetblockface, 1, 0);        	

        }

        if (id == 35) { //DefineBlock
            unsigned char bid,solid,movement,toptexture,sidetexture,bottomtexture,light,walksound,left,right,top,bottom;
            unsigned char fullbright,shape,blockdraw,fogdens,fogr,fogg,fogb;
            char name[64];

            recvW(tunnelSock, &bid, 1, 0);
            recvW(tunnelSock, name, 64, 0);
            recvW(tunnelSock, &solid, 1, 0);
            recvW(tunnelSock, &movement, 1, 0);
            recvW(tunnelSock, &toptexture, 1, 0);
            recvW(tunnelSock, &sidetexture, 1, 0);
            //recvW(tunnelSock, &left, 1, 0);
           // recvW(tunnelSock, &right, 1, 0);
            //recvW(tunnelSock, &top, 1, 0);
            //recvW(tunnelSock, &bottom, 1, 0);
            recvW(tunnelSock, &bottomtexture, 1, 0);
            recvW(tunnelSock, &light, 1, 0);
            recvW(tunnelSock, &walksound, 1, 0);
            recvW(tunnelSock, &fullbright, 1, 0);
            recvW(tunnelSock, &shape, 1, 0);
            recvW(tunnelSock, &blockdraw, 1, 0);
            recvW(tunnelSock, &fogdens, 1, 0);
            recvW(tunnelSock, &fogr, 1, 0);
            recvW(tunnelSock, &fogg, 1, 0);
            recvW(tunnelSock, &fogb, 1, 0);

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &bid, 1, 0);
            sendW(serverSock, name, 64, 0);
            sendW(serverSock, &solid, 1, 0);
            sendW(serverSock, &movement, 1, 0);
            sendW(serverSock, &toptexture, 1, 0);
            send(serverSock, &sidetexture, 1, 0);
            //sendW(serverSock, &left, 1, 0);
            //sendW(serverSock, &right, 1, 0);
            //sendW(serverSock, &top, 1, 0);
            //sendW(serverSock, &bottom, 1, 0);
            sendW(serverSock, &bottomtexture, 1, 0);
            sendW(serverSock, &light, 1, 0);
            sendW(serverSock, &walksound, 1, 0);
            sendW(serverSock, &fullbright, 1, 0);
            sendW(serverSock, &shape, 1, 0);
            sendW(serverSock, &blockdraw, 1, 0);
            sendW(serverSock, &fogdens, 1, 0);
            sendW(serverSock, &fogr, 1, 0);
            sendW(serverSock, &fogg, 1, 0);
            sendW(serverSock, &fogb, 1, 0);
        }

        if (id == 36) { //Remove DefineBlock
            char bid;
            recvW(tunnelSock, &bid, 1, 0);

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &bid, 1, 0);
        }
        //Neo defineblock: 

        if (id == 37) {
            unsigned char leftText,rightText,frontText,backText;
            char minx,miny,minz;
            char maxx,maxy,maxz;
            char bottomTex,trans,walk,bright;
            unsigned char bid,solid,movement,toptexture;
            unsigned char blockdraw,fog,fogr,fogg,fogb;
            char name[64];

            recvW(tunnelSock, &bid, 1, 0);
            recvW(tunnelSock, name, 64, 0);
            recvW(tunnelSock, &solid, 1, 0);
            recvW(tunnelSock, &movement, 1, 0);
            recvW(tunnelSock, &toptexture, 1, 0);
            recvW(tunnelSock, &leftText, 1, 0);
            recvW(tunnelSock, &rightText, 1, 0);
            recvW(tunnelSock, &frontText, 1, 0);
            recvW(tunnelSock, &backText, 1, 0);

            recvW(tunnelSock, &bottomTex, 1, 0);
            recvW(tunnelSock, &trans, 1, 0);
            recvW(tunnelSock, &walk, 1, 0);
            recvW(tunnelSock, &bright, 1, 0);

            recvW(tunnelSock, &minx, 1, 0);
            recvW(tunnelSock, &miny, 1, 0);
            recvW(tunnelSock, &minz, 1, 0);

            recvW(tunnelSock, &maxx, 1, 0);
            recvW(tunnelSock, &maxy, 1, 0);
            recvW(tunnelSock, &maxz, 1, 0);

            recvW(tunnelSock, &blockdraw, 1, 0);
            recvW(tunnelSock, &fog, 1, 0);
            recvW(tunnelSock, &fogr, 1, 0);
            recvW(tunnelSock, &fogg, 1, 0);
            recvW(tunnelSock, &fogb, 1, 0);

            //send

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &bid, 1, 0);
            sendW(serverSock, name, 64, 0);
            sendW(serverSock, &solid, 1, 0);
            sendW(serverSock, &movement, 1, 0);
            sendW(serverSock, &toptexture, 1, 0);
            sendW(serverSock, &leftText, 1, 0);
            sendW(serverSock, &rightText, 1, 0);
            sendW(serverSock, &frontText, 1, 0);
            sendW(serverSock, &backText, 1, 0);

            sendW(serverSock, &bottomTex, 1, 0);
            sendW(serverSock, &trans, 1, 0);
            sendW(serverSock, &walk, 1, 0);
            sendW(serverSock, &bright, 1, 0);

            sendW(serverSock, &minx, 1, 0);
            sendW(serverSock, &miny, 1, 0);
            sendW(serverSock, &minz, 1, 0);

            sendW(serverSock, &maxx, 1, 0);
            sendW(serverSock, &maxy, 1, 0);
            sendW(serverSock, &maxz, 1, 0);

            sendW(serverSock, &blockdraw, 1, 0);
            sendW(serverSock, &fog, 1, 0);
            sendW(serverSock, &fogr, 1, 0);
            sendW(serverSock, &fogg, 1, 0);
            sendW(serverSock, &fogb, 1, 0);

        }


        //Old as fuck defineblock

        /*if (id == 37) { //DefineBlockext
            char bid,solid,movement,toptexture,sidetexture,bottomtexture,light,walksound,left,right,top,bottom;
            char fullbright,shape,blockdraw,fogdens,fogr,fogg,fogb;
            char name[64];
            char minx,miny,minz;
            char maxx,maxy,maxz;

            recv(tunnelSock, &bid, 1, 0);
            recvW(tunnelSock, name, 64, 0);
            recv(tunnelSock, &solid, 1, 0);
            recv(tunnelSock, &movement, 1, 0);
            recv(tunnelSock, &toptexture, 1, 0);
            //recv(tunnelSock, &sidetexture, 1, 0);
            recv(tunnelSock, &left, 1, 0);
            recv(tunnelSock, &right, 1, 0);
            recv(tunnelSock, &top, 1, 0);
            recv(tunnelSock, &bottom, 1, 0);
            recv(tunnelSock, &bottomtexture, 1, 0);
            recv(tunnelSock, &light, 1, 0);
            recv(tunnelSock, &walksound, 1, 0);
            recv(tunnelSock, &fullbright, 1, 0);
            recv(tunnelSock, &minx, 1, 0);
            recv(tunnelSock, &miny, 1, 0);
            recv(tunnelSock, &minz, 1, 0);
            recv(tunnelSock, &maxx, 1, 0);
            recv(tunnelSock, &maxy, 1, 0);
            recv(tunnelSock, &maxz, 1, 0);
            recv(tunnelSock, &blockdraw, 1, 0);
            recv(tunnelSock, &fogdens, 1, 0);
            recv(tunnelSock, &fogr, 1, 0);
            recv(tunnelSock, &fogg, 1, 0);
            recv(tunnelSock, &fogb, 1, 0);

            send(serverSock, &id, 1, 0);
            send(serverSock, &bid, 1, 0);
            sendW(serverSock, name, 64, 0);
            send(serverSock, &solid, 1, 0);
            send(serverSock, &movement, 1, 0);
            send(serverSock, &toptexture, 1, 0);
            //send(serverSock, &sidetexture, 1, 0);
            send(serverSock, &left, 1, 0);
            send(serverSock, &right, 1, 0);
            send(serverSock, &top, 1, 0);
            send(serverSock, &bottom, 1, 0);
            send(serverSock, &bottomtexture, 1, 0);
            send(serverSock, &light, 1, 0);
            send(serverSock, &walksound, 1, 0);
            send(serverSock, &fullbright, 1, 0);

            send(serverSock, &minx, 1, 0);
            send(serverSock, &miny, 1, 0);
            send(serverSock, &minz, 1, 0);
            send(serverSock, &maxx, 1, 0);
            send(serverSock, &maxy, 1, 0);
            send(serverSock, &maxz, 1, 0);


            send(serverSock, &blockdraw, 1, 0);
            send(serverSock, &fogdens, 1, 0);
            send(serverSock, &fogr, 1, 0);
            send(serverSock, &fogg, 1, 0);
            send(serverSock, &fogb, 1, 0);

        }*/

        /*if (id == 37) {
            char minx,miny,minz;
            char maxx,maxy,maxz;

            recv(tunnelSock, &minx, 1, 0);
            recv(tunnelSock, &miny, 1, 0);
            recv(tunnelSock, &minz, 1, 0);
            recv(tunnelSock, &maxx, 1, 0);
            recv(tunnelSock, &maxy, 1, 0);
            recv(tunnelSock, &maxz, 1, 0);

            send(serverSock, &id, 1, 0);
            send(serverSock, &minx, 1, 0);
            send(serverSock, &miny, 1, 0);
            send(serverSock, &minz, 1, 0);
            send(serverSock, &maxx, 1, 0);
            send(serverSock, &maxy, 1, 0);
            send(serverSock, &maxz, 1, 0);
        }*/

        if (id == 39) {
            unsigned char red,green,blue,alpha,code;

            recv(tunnelSock, &red, 1, 0);
            recv(tunnelSock, &green, 1, 0);
            recv(tunnelSock, &blue, 1, 0);
            recv(tunnelSock, &alpha, 1, 0);
            recv(tunnelSock, &code, 1, 0);

            send(serverSock, &id, 1, 0);
            send(serverSock, &red, 1, 0);
            send(serverSock, &green, 1, 0);
            send(serverSock, &blue, 1, 0);
            send(serverSock, &alpha, 1, 0);
            send(serverSock, &code, 1, 0);
        }

        if (id == 38) {
            unsigned char count;
            char indices[1024];
            char blocks[256];
            simplePrint("Bulked'd\n");
            recvW(tunnelSock, &count, 1, 0);
            recvW(tunnelSock, (char*)&indices, 1024, 0);
            recvW(tunnelSock, (char*)&blocks, 256, 0);

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &count, 1, 0);
            sendW(serverSock, (char*)&indices, 1024, 0);
            sendW(serverSock, (char*)&blocks, 256, 0);
            simplePrint("Bulked'd2\n");
        }

        if (id == 40) { //EnvUrl
        	char url[64];
        	recvW(tunnelSock, url, 64, 0);


        	sendW(serverSock, &id, 1, 0);
        	sendW(serverSock, url, 64, 0);
         
        }

        if (id == 41) { //EnvProperty
        	char prop;
        	int buf;
        	char whore[4];

        	/*char side,edge,edgelevel,cloudlevel,fog,cspeed,wspeed;

        	recv(tunnelSock, &side, 1, 0);
            recv(tunnelSock, &edge, 1, 0);
            recv(tunnelSock, &edgelevel, 1, 0);
            recv(tunnelSock, &cloudlevel, 1, 0);
            recv(tunnelSock, &fog, 1, 0);
            recv(tunnelSock, &cspeed, 1, 0);
            recv(tunnelSock, &wspeed, 1, 0);*/

        	recvW(tunnelSock, &prop, 1, 0);
        	recvW(tunnelSock, (char*)&buf, 4, 0);

        	memcpy(&whore[0], &buf, sizeof(int));

        	sendW(serverSock, &id, 1, 0);

        	/*send(serverSock, &side, 1, 0);
            send(serverSock, &edge, 1, 0);
            send(serverSock, &edgelevel, 1, 0);
            send(serverSock, &cloudlevel, 1, 0);
            send(serverSock, &fog, 1, 0);
            send(serverSock, &cspeed, 1, 0);
            send(serverSock, &wspeed, 1, 0);*/

        	sendW(serverSock, &prop, 1, 0);
        	sendW(serverSock, &whore[0], 4, 0);

        }
		
		if (id == 42) { //entity property
			char sid,type;
			int value;
			char cunt[4];
			
			recvW(tunnelSock, &sid, 1, 0);
			recvW(tunnelSock, &type, 1, 0);
			recvW(tunnelSock, (char*)&value, 4, 0);
			
			memcpy(&cunt[0], &value, sizeof(int));
			
			sendW(serverSock, &id, 1, 0);
			sendW(serverSock, &sid, 1, 0);
			sendW(serverSock, &type, 1, 0);
			sendW(serverSock, &cunt[0], 4, 0);
		}

        if (id == 43) {
            char ping;
            char stc;
            short data;
            char buf[2];

            recvW(tunnelSock, &ping, 1, 0);
            //recvW(tunnelSock, &stc, 1, 0);
            recvW(tunnelSock, (char*)&data, 2, 0);

            memcpy(&buf[0], &data, sizeof(short));

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &ping, 1, 0);
            //sendW(serverSock, &stc, 1, 0);
            sendW(serverSock, &buf[0], 2, 0);
        }

        if (id == 44) {
            unsigned char block, order;

            recvW(tunnelSock, &block, 1, 0);
            recvW(tunnelSock, &order, 1, 0);

            sendW(serverSock, &id, 1, 0);
            sendW(serverSock, &block, 1, 0);
            sendW(serverSock, &order, 1, 0);
        }
        //Sleep(1);
    }
}