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

char unused;
char tunnelUser[64]; //The clients username
bool hide = false;

void tunnelMsg(char *msg) {
    char cmsg[64];
    toMCString(cmsg,msg);

    char p = 0x0d;
    char pid = 0;

    send(serverSock, &p, 1, 0);
    send(serverSock, &pid, 1, 0);
    send(serverSock, cmsg, 64, 0);
}

void *tunnelServer() { //This is the tunnel server that handles the client

	WSADATA wsa;
    WORD version = MAKEWORD(2, 1);

    WSAStartup(version, &wsa);

    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN networkShit;

    networkShit.sin_addr.s_addr = inet_addr("127.0.0.1");
    networkShit.sin_family = AF_INET;
    networkShit.sin_port = htons(80); //I use port 80 to avoid needing to port forward

    int shitSize = sizeof(networkShit);

    bind(sockfd, (SOCKADDR*)&networkShit, sizeof(networkShit));

    listen(sockfd, 1);

    unsigned char id;
    int retval = 0;

    bool clientConnected = false;    
    bool connected = false;
    attron(COLOR_PAIR(3));
    simplePrint("\nTunnel is ready!");
    attron(COLOR_PAIR(1));
    simplePrint("\n\nDirect connect to 127.0.0.1:80. (doesn't matter what you set username/mppass to)\n");

    while(!connected) { //Wait for client to connect...
        if (serverSock = accept(sockfd, (SOCKADDR*)&networkShit, &shitSize)) {
            closesocket(sockfd);
            connected = true;
        }
    }


    /*FILE *pack;
    pack = fopen("packets2.txt", "w+");
    fprintf(pack, "\n\n:BEGIN:\n");*/

    cuboid.zspeed = 50;

    char mado[10][64];

    bool fucked = false;

    while (connected) {

        retval = recvW(serverSock, &id, 1, 0);

        if (retval < 1) { //Disconnected
        	connected = false;
            closesocket(serverSock);
            closesocket(tunnelSock);
            WSACleanup();
            bError("Client has disconnected!");
            cuboid.halt = true;
        }

        	//printw("Client: %d.\n",id);
        	//refresh();

            //fprintf(pack, "ID: %d.\n", id);
        	if (id == 0x00 && fucked == false) { //Login
				fucked = true;
        		char pass[64];
            	char name[64];
            	char shit;

            	recvW(serverSock, &shit, 1, 0);
            	recvW(serverSock, name, 64, 0);
           	 	recvW(serverSock, pass, 64, 0);
            	recvW(serverSock, &unused, 1 , 0);

            	cleanString(tunnelUser, name);
        		
                //After the client logs in we create a thread for the 'client' part of the tunnel which acts
                //as a client and passes all the data from the server to the real client connected to the tunnel

        		printw("\n'%s' has connected to the tunnel.\n",tunnelUser);
        		refresh();
                clientConnected = true;

        		char mcMotd[64];
        		char mcServer[64];

                pthread_t tunnelT; //This will be the thread for the tunnel 'client' that handles the server
                if (pthread_create(&tunnelT,NULL,&mcConnect, 0) != 0) {
                    bError("Failed to create tunnel thread!");
                }
                pthread_t buildT; //The builder thread
                if (pthread_create(&buildT,NULL,&builder, 0) != 0) {
                    bError("Failed to create builder thread!");
                }
                cuboid.start = false;
        	}

            if (id == 0x05) { //Set block
                short x,y,z;
                unsigned char mode,blocktype;

                recvW(serverSock, (char*)&x, 2, 0);
                recvW(serverSock, (char*)&y, 2, 0);
                recvW(serverSock, (char*)&z, 2, 0);
                recvW(serverSock, &mode, 1, 0);
                recvW(serverSock, &blocktype, 1, 0);

                char buf[6];

                memcpy(&buf[0], &x, sizeof(short));
                memcpy(&buf[2], &y, sizeof(short));
                memcpy(&buf[4], &z, sizeof(short));

                //Don't send the packet if we're setting points
                if (zPlace) {
                	if (cuboid.place) {
	                    if (points == 0) {
	                        cuboid.x1 = ntohs(x);
	                        cuboid.y1 = ntohs(y);
	                        cuboid.z1 = ntohs(z);
	                        points = 1;
	                        tunnelMsg("&0[&4LavaTunnel&0] &fz: First point.");
	                    } else if (points == 1) {
	                        cuboid.x2 = ntohs(x);
	                        cuboid.y2 = ntohs(y);
	                        cuboid.z2 = ntohs(z);
	                        if (cuboid.getBlock) {
	                            cuboid.block = blocktype;
	                        }
	                        points = 0;
	                        cuboid.place = false;
	                        zPlace = false;
	                        cuboid.start = true;
	                        tunnelMsg("&0[&4LavaTunnel&0] &fz: Second point, starting cuboid!");
	                    }
	                }
                    if (paste.place) {
                            paste.x = ntohs(x);
                            paste.y = ntohs(y);
                            paste.z = ntohs(z);
                            char ptxt[64];
                            sprintf(ptxt, "&0[&4LavaTunnel&0] &fpaste: pasting %s",paste.chunkName);
                            tunnelMsg(ptxt);
                            paste.place = false;
                            zPlace = false;
                            paste.start = true;
                    }
	                if (copy.place) {
	                	if (points == 0) {
	                        copy.x1 = ntohs(x);
	                        copy.y1 = ntohs(y);
	                        copy.z1 = ntohs(z);
	                        points = 1;
	                        tunnelMsg("&0[&4LavaTunnel&0] &fcopy: First point.");
	                    } else if (points == 1) {
	                        copy.x2 = ntohs(x);
	                        copy.y2 = ntohs(y);
	                        copy.z2 = ntohs(z);
	                        points = 0;
	                        copy.place = false;
	                        zPlace = false;
	                        copy.start = true;
	                        tunnelMsg("&0[&4LavaTunnel&0] &fcopy: Second point, copying");
	                    }
	                }

	                //Change the pointer block back to its original block clientside, from serverside nothing gets placed/removed
	                printw("setting %d\n",bOffset(ntohs(x),ntohs(y),ntohs(z)));
	                refresh();
	                tunnelSetBlock(x, y, z, bOffset(ntohs(x),ntohs(y),ntohs(z)));
	                tunnelSetBlock(x, y, z, bOffset(ntohs(x),ntohs(y),ntohs(z)));
	                tunnelSetBlock(x, y, z, bOffset(ntohs(x),ntohs(y),ntohs(z)));
	               
                } else {
                    if (paint) {
                        blocktype = paintBid;
                        mode = 1;
                    } 

                    char p = 0x05;
                    sendW(tunnelSock, &p, 1, 0);
                    sendW(tunnelSock, &buf[0], 2, 0);
                    sendW(tunnelSock, &buf[2], 2, 0);
                    sendW(tunnelSock, &buf[4], 2, 0);
                    sendW(tunnelSock, &mode, 1, 0);
                    sendW(tunnelSock, &blocktype, 1, 0);
                }
            }

            if (id == 0x08) { //Pos update
                unsigned char pid,yaw,pitch;
                short x,y,z;

                recvW(serverSock, &pid, 1, 0);
                recvW(serverSock, (char*)&x, 2, 0);
                recvW(serverSock, (char*)&y, 2, 0);
                recvW(serverSock, (char*)&z, 2, 0);
                recvW(serverSock, &yaw, 1, 0);
                recvW(serverSock, &pitch, 1, 0);

                if (hide == false) { 
                    user._x = ntohs(x);
                    user._y = ntohs(y);
                    user._z = ntohs(z);
                    user._yaw = yaw;
                    user._pitch = pitch;

                    char buf[6];

                    memcpy(&buf[0], &x, sizeof(short));
                    memcpy(&buf[2], &y, sizeof(short));
                    memcpy(&buf[4], &z, sizeof(short));

                    char p = 0x08;
                    sendW(tunnelSock, &p, 1, 0);
                    sendW(tunnelSock, &pid, 1, 0);
                    sendW(tunnelSock, &buf[0], 2, 0);
                    sendW(tunnelSock, &buf[2], 2, 0);
                    sendW(tunnelSock, &buf[4], 2, 0);
                    sendW(tunnelSock, &yaw, 1, 0);
                    sendW(tunnelSock, &pitch, 1, 0);
                } else { //Don't send postion to server due to hide command being toggled
                    user._hx = ntohs(x); //Still keep track of where the client is, using different vars of course
                    user._hy = ntohs(y);
                    user._hz = ntohs(z);
                    user._hyaw = yaw;
                    user._hpitch = pitch;
                }

                //FUCK(yaw);
            }

            if (id == 0x0d) {//Chat message
                char message[64];
                char what;

                recvW(serverSock, &what, 1, 0);
                recvW(serverSock, message, 64, 0);

                //Command interpretation

                char msg[64];
                cleanString(msg, message);

                if ((msg[0] == '\'') && (strlen(msg) >= 2)) {
                    char command[64];
                    memcpy(command, msg+1, strlen(msg) - 1); //Remove the ' at the beginning of the string
                    memset(command+strlen(msg) - 1, '\0', 1);

                    printw("Command: %s.\n",command);
                    refresh();
                    if (commandFunc(command, false) == false) {
                        tunnelMsg("&0[&4LavaTunnel&0] &fUnknown command.");
                    }
                } else { //Not a command
                    if (gimped == true && message[0] != '/') {
                        if (what == 0) {
                            srand(time(NULL));
                            int randomnumber = rand() % (gimpN - 1) + 1;
                            char msg[64];
                            int size = strlen(gimp[randomnumber])+strlen(player[-1]._username);

                            if (size >= 64) { //Prevent gimp from being cutoff
                                int n = ceil((double)size / (double)64); //How many messages the gimp will be split into
                                char buf[n][64];
                                char buf2[n][64];

                                int i = 0;
                                int total = 0;
                                for (i = 0; i <= n; i++) {
                                    strncpy(buf[i], gimp[randomnumber]+total, 64);
                                    total += 64;
                                    toMCString(buf2[i], buf[i]);

                                    //Send i of n of the messages
                                    if (i < n) {
                                        sendMessage(1, buf2[i]);
                                    } else {
                                        sendMessage(0, buf2[i]);
                                    }
                                }
                            } else {
                                toMCString(msg, gimp[randomnumber]);
                                sendMessage(what, msg);
                            }
                        }

                    } else {
                        sendMessage(what, message);
                    }
                }
            }

            //CPE:

            if (id == 0x10) { //ExtInfo
                char app[64];
                short count;

                recvW(serverSock, app, 64, 0);
                recvW(serverSock, (char*)&count, 2, 0);

                count = ntohs(count);
                count = count-1;
                count = htons(count);

                char buf[1];

                memcpy(&buf[0], &count, sizeof(short));

                sendW(tunnelSock, &id, 1, 0);
                sendW(tunnelSock, app, 64, 0);
                sendW(tunnelSock, &buf[0], 2, 0);
            }

            if (id == 0x11) { //ExtEntry
                char extname[64];
                int version;

                recvW(serverSock, extname, 64, 0);
                recvW(serverSock, (char*)&version, 4, 0);

                char buf[4];
                int ev = ntohl(version);
                memcpy(&buf[0], &version, sizeof(int));

                char nextname[64];
                strncpy(nextname, extname, 64);
                nullTerm(nextname);
                /*printw("Name: %s.\nVersion: %d\n",nextname,ev);
                refresh();
                char cped[64];
                sprintf(cped, "\n%s\n",nextname);
                FILE *cpef = fopen("CPE.txt", "a+");
                fwrite(cped, strlen(cped), 1, cpef);
                fclose(cpef);*/
               //if (strstr(extname, "FullCP437") != NULL || strstr(extname, "LongerMessages") != NULL || strstr(extname, "Player") != NULL || strstr(extname, "Entity") != NULL || strstr(extname, "ClickDistance") != NULL || strstr(extname, "CustomBlocks") != NULL || strstr(extname, "HoldThis") != NULL || strstr(extname, "EmoteFix") != NULL || strstr(extname, "TextHotKey") != NULL || strstr(extname, "ExtPlayerList") != NULL || strstr(extname, "ExtAddEntity2") != NULL || strstr(extname, "ExtAddPlayerName") != NULL || strstr(extname, "ExtRemovePlayerName") != NULL || strstr(extname, "EnvSetColor") || strstr(extname, "SetBlockPermission") != NULL || strstr(extname, "ChangeModel") != NULL || strstr(extname, "EnvMapAppearence") != NULL || strstr(extname, "EnvSetMapAppearance v2") != NULL || strstr(extname, "EnvSetWeatherType") != NULL || strstr(extname, "HackControl") != NULL || strstr(extname, "MessageTypes") != NULL || strstr(extname, "PlayerClick") != NULL || strstr(extname, "LongerMessages") != NULL || strstr(extname, "BlockDefinitions") != NULL || strstr(extname, "BlockDefinitionsExt") != NULL || strstr(extname, "FullCP437") != NULL || strstr(extname, "BulkBlockUpdate") != NULL) {
                    //if (strcmp(nextname, "FastMap") == 0 || strcmp(nextname, "ExtEntityPositions") == 0 || strcmp(nextname, "ExtendedBlocks") == 0) {
                        /*sendW(tunnelSock, &id, 1, 0);
                        sendW(tunnelSock, extname, 64, 0);
                        sendW(tunnelSock, &buf[0], 4, 0);
                        cfastMap = true;*/
                    if (strcmp(nextname, "ExtendedBlocks") == 0) {

                    } else {
                        sendW(tunnelSock, &id, 1, 0);
                        sendW(tunnelSock, extname, 64, 0);
                        sendW(tunnelSock, &buf[0], 4, 0);
                    }
                    //memset(extname+64, '\0', 1);
            }

            if (id == 0x12) { //Click distance
                short distance;

                recvW(serverSock, (char*)&distance, 2, 0);

                char buf[2];

                memcpy(&buf[0], &distance, sizeof(short));

                sendW(tunnelSock, &id, 1, 0);
                sendW(tunnelSock, &buf[0], 2, 0);
            }

            if (id == 0x13) { //Custom blocks
                char level;
                
                recvW(serverSock, &level, 1, 0);

                sendW(tunnelSock, &id, 1, 0);
                sendW(tunnelSock, &level, 1, 0);
            }

            if (id == 0x14) { //Hold this
                char block,prevent;

                recvW(serverSock, &block, 1, 0);
                recvW(serverSock, &prevent, 1, 0);

                sendW(tunnelSock, &id, 1, 0);
                sendW(tunnelSock, &block, 1, 0);
                sendW(tunnelSock, &prevent, 1, 0);
            }

            if (id == 0x15) { //Text hot key
                char label[64];
                char action[64];
                int keycode;
                char keymods;

                recvW(serverSock, label, 64, 0);
                recvW(serverSock, action, 64, 0);
                recvW(serverSock, (char*)&keycode, 4, 0);
                recvW(serverSock, &keymods, 1, 0);

                char buf[3];
                memcpy(&buf[0], &keycode, sizeof(int));

                sendW(tunnelSock, &id, 1, 0);
                sendW(tunnelSock, label, 64, 0);
                sendW(tunnelSock, action, 64, 0);
                sendW(tunnelSock, &buf[0], 4, 0);
            }

            if (id == 0x16) { //ExtAddPlayerName
                short nameid;
                char playername[64];
                char listname[64];
                char groupname[64];
                char grouprank;

                recv(serverSock, (char*)&nameid, 2, 0);
                recvW(serverSock, playername, 64, 0);
                recvW(serverSock, listname, 64, 0);
                recvW(serverSock, groupname, 64, 0);
                recv(serverSock, &grouprank, 1, 0);

                char buf[1];
                memcpy(&buf[0], &nameid, sizeof(short));

                send(tunnelSock, &id, 1, 0);
                send(tunnelSock, &buf[0], 2, 0);
                sendW(tunnelSock, playername, 64, 0);
                sendW(tunnelSock, listname, 64, 0);
                sendW(tunnelSock, groupname, 64, 0);
                send(tunnelSock, &grouprank, 1, 0);
            }

            if (id == 0x21) { //ExtAddEntity2
                char entityid;
                char ingame[64];
                char skinname[64];
                short x,y,z;
                char pitch,yaw;

                recv(serverSock, &entityid, 1, 0);
                recvW(serverSock, ingame, 64, 0);
                recvW(serverSock, skinname, 64, 0);
                recvW(serverSock, (char*)&x, 2, 0);
                recvW(serverSock, (char*)&y, 2, 0);
                recvW(serverSock, (char*)&z, 2, 0);
                recv(serverSock, &yaw, 1, 0);
                recv(serverSock, &pitch, 1, 0);

                char buf[6];
                memcpy(&buf[0], &x, sizeof(short));
                memcpy(&buf[2], &y, sizeof(short));
                memcpy(&buf[4], &z, sizeof(short));

                send(tunnelSock, &id, 1, 0);
                sendW(tunnelSock, ingame, 64, 0);
                sendW(tunnelSock, skinname, 64, 0);
                sendW(tunnelSock, &buf[0], 2, 0);
                sendW(tunnelSock, &buf[2], 2, 0);
                sendW(tunnelSock, &buf[4], 2, 0);
                send(tunnelSock, &yaw, 1, 0);
                send(tunnelSock, &pitch, 1, 0);
            }

            if (id == 0x18) { //ExtRemovePlayerName
                short nameid;

                recv(serverSock, (char*)&nameid, 2, 0);

                char buf[1];
                memcpy(&buf[0], &nameid, sizeof(short));

                send(tunnelSock, &id, 1, 0);
                send(tunnelSock, &buf[0], 2, 0);
            }

            if (id == 0x19) { //EnvSetColor
                char var;
                short red,green,blue;

                recv(serverSock, &var, 1, 0);
                recv(serverSock, (char*)&red, 2, 0);
                recv(serverSock, (char*)&green, 2, 0);
                recv(serverSock, (char*)&blue, 2, 0);

                char buf[6];
                memcpy(&buf[0], &red, sizeof(short));
                memcpy(&buf[2], &green, sizeof(short));
                memcpy(&buf[4], &blue, sizeof(short));

                send(tunnelSock, &id, 1, 0);
                send(tunnelSock, &var, 1, 0);
                send(tunnelSock, &buf[0], 2, 0);
                send(tunnelSock, &buf[2], 2, 0);
                send(tunnelSock, &buf[4], 2, 0);
            }

            if (id == 0x1A) { //MakeSelection
                char selectionid;
                char label[64];
                short startx,starty,startz;
                short endx,endy,endz;
                short red,green,blue;
                short opacity;

                recv(serverSock, &selectionid, 1, 0);
                recv(serverSock, label, 64, 0);
                recv(serverSock, (char*)&startx, 2, 0);
                recv(serverSock, (char*)&starty, 2, 0);
                recv(serverSock, (char*)&startz, 2, 0);
                recv(serverSock, (char*)&endx, 2, 0);
                recv(serverSock, (char*)&endy, 2, 0);
                recv(serverSock, (char*)&endz, 2, 0);
                recv(serverSock, (char*)&red, 2, 0);
                recv(serverSock, (char*)&green, 2, 0);
                recv(serverSock, (char*)&blue, 2, 0);
                recv(serverSock, (char*)&opacity, 2, 0);

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

                send(tunnelSock, &id, 1, 0);
                send(tunnelSock, &selectionid, 1, 0);
                send(tunnelSock, label, 64, 0);
                send(tunnelSock, &buf1[0], 2, 0);
                send(tunnelSock, &buf1[2], 2, 0);
                send(tunnelSock, &buf1[4], 2, 0);

                send(tunnelSock, &buf2[0], 2, 0);
                send(tunnelSock, &buf2[2], 2, 0);
                send(tunnelSock, &buf2[4], 2, 0);

                send(tunnelSock, &buf3[0], 2, 0);
                send(tunnelSock, &buf3[2], 2, 0);
                send(tunnelSock, &buf3[4], 2, 0);

                send(tunnelSock, &buf4[0], 2, 0);

            }

            if (id == 0x1B) { //RemoveSelection
                char selection;

                recv(serverSock, &selection, 1, 0);

                send(tunnelSock, &id, 1, 0);
                send(tunnelSock, &selection, 1, 0);
            }

            if (id == 0x1C) { //SetBlockPermission
                char block,allowplace,allowdelete;

                recvW(serverSock, &block, 1, 0);
                recvW(serverSock, &allowplace, 1, 0);
                recvW(serverSock, &allowdelete, 1, 0);

                sendW(tunnelSock, &id, 1, 0);
                sendW(tunnelSock, &block, 1, 0);
                sendW(tunnelSock, &allowplace, 1, 0);
                sendW(tunnelSock, &allowdelete, 1, 0);
            }

            if (id == 0x1D) { //ChangeModel
                char entityid;
                char modelname[64];

                recv(serverSock, &entityid, 1, 0);
                recvW(serverSock, modelname, 64, 0);

                send(tunnelSock, &id, 1, 0);
                send(tunnelSock, &entityid, 1, 0);
                sendW(tunnelSock, modelname, 64, 0);
            }

            if (id == 0x1E) { //EnvMapAppearence
                char url[64];
                char sideblock,edgeblock;
                unsigned short sidelevel;

                recvW(serverSock, url, 64, 0);
                recv(serverSock, &sideblock, 1, 0);
                recv(serverSock, &edgeblock, 1, 0);
                recv(serverSock, (char*)&sidelevel, 2, 0);
                unsigned char buf[1];
                memcpy(&buf[0], &sidelevel, sizeof(short));

                send(tunnelSock, &id, 1, 0);
                sendW(tunnelSock, url, 64, 0);
                send(tunnelSock, &sideblock, 1, 0);
                send(tunnelSock, &edgeblock, 1, 0);
                send(tunnelSock, &buf[0], 2, 0);
            }

            if (id == 0x1F) { //EnvSetWeatherType
                char weathertype;

                recvW(serverSock, &weathertype, 1, 0);

                sendW(tunnelSock, &id, 1, 0);
                sendW(tunnelSock, &weathertype, 1, 0);
            }

            /*if (id == 0x20) { //HackCuntrol
                char flying,noclip,speeding,spawncontrol,thirdperson;
                short jumpheight;

                recv(serverSock, &flying, 1, 0);
                recv(serverSock, &noclip, 1, 0);
                recv(serverSock, &speeding, 1, 0);
                recv(serverSock, &spawncontrol, 1, 0);
                recv(serverSock, &thirdperson, 1, 0);
                recv(serverSock, (char*)&jumpheight, 2, 0);

                //Ignore server attempts to disable hax
                jumpheight = ntohs(jumpheight);
                jumpheight = -1;
                jumpheight = htons(jumpheight);

                flying = 1;
                noclip = 1;
                speeding = 1;
                spawncontrol = 1;
                thirdperson = 1;

                char buf[1];
                memcpy(&buf[0], &jumpheight, sizeof(short));

                send(tunnelSock, &id, 1, 0);
                send(tunnelSock, &flying, 1, 0);
                send(tunnelSock, &noclip, 1, 0);
                send(tunnelSock, &speeding, 1, 0);
                send(tunnelSock, &spawncontrol, 1, 0);
                send(tunnelSock, &thirdperson, 1, 0);
                send(tunnelSock, &buf[0], 2, 0);
            }*/

            if (id == 0x22) { //PlayerClicked
                unsigned char button,action,targetent,targetblock,buttondown;
                unsigned short yaw,pitch,targetx,targety,targetz;

                recvW(serverSock, &button, 1, 0);
                recvW(serverSock, &action, 1, 0);
                recvW(serverSock, &buttondown, 1, 0);
                recvW(serverSock, (char*)&yaw, 2, 0);
                recvW(serverSock, (char*)&pitch, 2, 0);
                recvW(serverSock, &targetent, 1, 0);
                recvW(serverSock, (char*)&targetx, 2, 0);
                recvW(serverSock, (char*)&targety, 2, 0);
                recvW(serverSock, (char*)&targetz, 2, 0);
                recvW(serverSock, &targetblock, 1, 0);

                unsigned char buf1[4];
                memcpy(&buf1[0], &yaw, sizeof(short));
                memcpy(&buf1[2], &pitch, sizeof(short));
                unsigned char buf2[6];
                memcpy(&buf2[0], &targetx, sizeof(short));
                memcpy(&buf2[2], &targety, sizeof(short));
                memcpy(&buf2[4], &targetz, sizeof(short));

                sendW(tunnelSock, &id, 1, 0);
                sendW(tunnelSock, &button, 1, 0);
                sendW(tunnelSock, &action, 1, 0);
                sendW(tunnelSock, &buttondown, 1, 0);
                sendW(tunnelSock, &buf1[0], 2, 0);
                sendW(tunnelSock, &buf1[2], 2, 0);
                sendW(tunnelSock, &targetent, 1, 0);
                sendW(tunnelSock, &buf2[0], 2, 0);
                sendW(tunnelSock, &buf2[2], 2, 0);
                sendW(tunnelSock, &buf2[4], 2, 0);
                sendW(tunnelSock, &targetblock, 1, 0);

            }

            /*if (id == 35) { //DefineBlock
            char bid,solid,movement,toptexture,sidetexture,bottomtexture,light,walksound;
            char fullbright,shape,blockdraw,fogdens,fogr,fogg,fogb;
            char name[64];

            recvW(serverSock, &bid, 1, 0);
            recvW(serverSock, name, 64, 0);
            recvW(serverSock, &solid, 1, 0);
            recvW(serverSock, &movement, 1, 0);
            recvW(serverSock, &toptexture, 1, 0);
            recvW(serverSock, &sidetexture, 1, 0);
            recvW(serverSock, &bottomtexture, 1, 0);
            recvW(serverSock, &light, 1, 0);
            recvW(serverSock, &walksound, 1, 0);
            recvW(serverSock, &fullbright, 1, 0);
            recvW(serverSock, &shape, 1, 0);
            recvW(serverSock, &blockdraw, 1, 0);
            recvW(serverSock, &fogdens, 1, 0);
            recvW(serverSock, &fogr, 1, 0);
            recvW(serverSock, &fogg, 1, 0);
            recvW(serverSock, &fogb, 1, 0);

            sendW(tunnelSock, &id, 1, 0);
            sendW(tunnelSock, &bid, 1, 0);
            sendW(tunnelSock, name, 64, 0);
            sendW(tunnelSock, &solid, 1, 0);
            sendW(tunnelSock, &movement, 1, 0);
            sendW(tunnelSock, &toptexture, 1, 0);
            sendW(tunnelSock, &sidetexture, 1, 0);
            sendW(tunnelSock, &bottomtexture, 1, 0);
            sendW(tunnelSock, &light, 1, 0);
            sendW(tunnelSock, &walksound, 1, 0);
            sendW(tunnelSock, &fullbright, 1, 0);
            sendW(tunnelSock, &shape, 1, 0);
            sendW(tunnelSock, &blockdraw, 1, 0);
            sendW(tunnelSock, &fogdens, 1, 0);
            sendW(tunnelSock, &fogr, 1, 0);
            sendW(tunnelSock, &fogg, 1, 0);
            sendW(tunnelSock, &fogb, 1, 0);
        }*/

        /*if (id == 36) { //Remove DefineBlock
            char bid;
            recvW(serverSock, &bid, 1, 0);

            sendW(tunnelSock, &id, 1, 0);
            sendW(tunnelSock, &bid, 1, 0);
        }*/
        
        //Sleep(1);

        if (id == 43) {
            char ping;
            char stc;
            short data;
            char buf[2];

            recvW(serverSock, &ping, 1, 0);
            //recvW(serverSock, &stc, 1, 0);
            recvW(serverSock, &buf[0], 2, 0);

            memcpy(&buf[0], &data, sizeof(short));
            sendW(tunnelSock, &id, 1, 0);
            sendW(tunnelSock, &ping, 1, 0);
            //sendW(tunnelSock, &stc, 1, 0);
            sendW(tunnelSock, &buf[0], 2, 0);
        }
    }
}