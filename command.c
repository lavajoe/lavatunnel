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

#include <curses.h>
#include <stdbool.h>

#include "tunnel.h"

bool commandFunc(char *command, bool console) { //returns false if command is unknown else returns true
	//Commands that take no args
    if (strcmp(command, "hide") == 0) {
       if (hide == false) {
           tunnelMsg("&0[&4LavaTunnel&0] &fStopped sending position to server.");
           hide = true;
        } else {
            tunnelMsg("&0[&4LavaTunnel&0] &fNow sending position to server.");
            hide = false;
        }
        return true;
    }
    if (strcmp(command, "back") == 0) {
       tunnelTeleport(user._x,user._y,user._z,user._yaw,user._pitch);
       return true;
    }
    if (strcmp(command, "ptp") == 0) {
        short y2 = user._y;
        int pitch = user._pitch;
        if (pitch > 0) { //Reverse negative with postive ect (minecraft coords are retarded)
            pitch = abs(pitch) * (-1);
        } else {
            pitch = abs(pitch);
        }

        if (user._pitch != 0) {  //Prevent meme division (dividing by zero)
           y2 += pitch;
        } else {
            y2 = user._y;
        }

        short x2 = user._x;
        short z2 = user._z;

        

        //y2 = cos(user._pitch);
        /*short z3 = cos(user._yaw)*sin(user._pitch);
        short x3 = sin(user._yaw)*sin(user._pitch);
        x2+=x3*32;
        z2+=z3*32;*/

        //int step = 64 / pitch;

        int vx = 0;
        int vy = user._y/32;

        for (vx = x2/32; vx < x2/32+10; vx++) {
            vy += pitch;
            FUCK(vy);
            setBlock(vx, vy, user._z/32, 2, true);
        }
        

       FUCK(pitch);
      //FUCK(step);
      // FUCK(x3*32);
       FUCK(y2);
       //FUCK(z3*32);
       //setBlock(user._x/32,y2,(user._z/32)+10, 41, false);
       tunnelTeleport(x2, y2, z2, user._yaw, user._pitch);
       return true;
    }
    if(strcmp(command, "roll") == 0) {
        //srand(time(NULL));
        int randomnumber = rand() % (99999 - 10000) + 1;
        char msg[64];
        sprintf(msg, "I rolled a %d.",randomnumber);
        sendMessage(0, msg);
        return true;
    }
    if (strcmp(command, "gimp") == 0) {
        if (gimpN < 1) {
            tunnelMsg("&0[&4LavaTunnel&0] &fGimp file is empty or doesnt exist.");
            return true;
        }
       if (gimped == false) {
           tunnelMsg("&0[&4LavaTunnel&0] &fYou are now gimped.");
           gimped = true;
           } else {
               tunnelMsg("&0[&4LavaTunnel&0] &fUngimped.");
               gimped = false;
           }
        return true;
    }
    if (strcmp(command, "dump") == 0) {
        int ll = 0;
        for (ll = 0; ll < 10000; ll++) {
            printw(":%d:",map[ll]);
            refresh();
            Sleep(1);
        }
    }
    if (strcmp(command, "cancel") == 0) {
    	if (hide) {
            tunnelTeleport(user._x,user._y,user._z,user._yaw,user._pitch);
            hide = false;
        }
    	gimped = false;
        cuboid.halt = true;
        paste.halt = true;
        paint = false;
        zPlace = false;
        copy.place = false;
        paste.place = false;

    	int i = 0;
    	for (i = 0; i < 256; i++) { //Search if we're following/plotting any players
    		if (player[i].mode != 0) {
    			player[i].mode = 0;
    		}
    	}

    	tunnelMsg("&0[&4LavaTunnel&0] &fCancled all toggles/actions.");

    	return true;
    }

    //After we check for commands that take no args we start splitting the string
    char *arg[16];

    arg[0] = strtok(command, " ");
    if (arg[0] == NULL) {
    } else {
    	if (strcmp(arg[0], "tp") == 0) { //Tits
    		arg[1] = strtok(NULL, " ");
    		if (arg[1] == NULL) {
    			tunnelMsg("&0[&4LavaTunnel&0] &ftp: missing argument.");
    			return true;
    		} else {
    			int i = 0;
    			for (i = 0; i < 256; i++) {
    				if (strstr(player[i]._username, arg[1]) != NULL) {
    					tunnelTeleport(player[i]._x, player[i]._y, player[i]._z, player[i]._yaw, player[i]._pitch);
    					return true;
    				}
    			}
    			tunnelMsg("&0[&4LavaTunnel&0] &fCan't find player.");
    		}

    		return true;
    	}
        if (strcmp(arg[0], "help") == 0) {
            arg[1] = strtok(NULL, " ");
            if (arg[1] == NULL) {
                tunnelMsg("&0[&4LavaTunnel&0] &fUsage: help <option>");
                tunnelMsg("&0[&4LavaTunnel&0] &f'help player - player related cmds like 'tp");
                tunnelMsg("&0[&4LavaTunnel&0] &f'help build - building cmds like 'z");
                return true;
            } else {
                if (strcmp(arg[1], "player") == 0) {
                    tunnelMsg("&0================");
                    tunnelMsg("&4Player related commands:");
                    tunnelMsg("'help <player,build,packets> - prints help for x cmds.");
                    tunnelMsg("'hide - stop sending your position to the server");
                    tunnelMsg("'tp <player> - teleports you to <player>");
                    tunnelMsg("'follow <player> - follow <player>");
                    tunnelMsg("'specate <player> - follow <player> while invisible");
                    tunnelMsg("'cancel - cancel all toggles and actions e.g. follow");
                    tunnelMsg("'ignore <player> - ignore all chat from <player> (see ignore.txt)");
                    tunnelMsg("'gimp - gimp/joker yourself");
                    tunnelMsg("'find <player> - gives the coords of <player>");
                    tunnelMsg("'back - go back to where the server thinks you are");
                    tunnelMsg("&0================");
                }
                if (strcmp(arg[1], "build") == 0) {
                    tunnelMsg("&0===============");
                    tunnelMsg("&4Build related commands:");
                    tunnelMsg("'zspeed <speed> - time the tunnel will wait between");
                    tunnelMsg("each block placed in ms, use 'none' for no limit");
                    tunnelMsg("'z block - place two blocks to define a cuboid");
                    tunnelMsg("leave <mode> blank for regular cuboid");
                    tunnelMsg("'paint <block> - same as /paint from MCLawl servers");
                    tunnelMsg("&0===============");
                }
            }

            return true;
        }
        if (strcmp(arg[0], "plotmotion") == 0) {
            arg[1] = strtok(NULL, " ");
            if (arg[1] == NULL) {
                tunnelMsg("&0[&4LavaTunnel&0] &fplotmotion: Missing playername");
                return true;
            } else {
                arg[2] = strtok(NULL, " ");
                if (arg[2] == NULL) {
                    tunnelMsg("&0[&4LavaTunnel&0] &fplotmotion: Missing block");
                    return true;
                } else {
                    if (getBid(arg[2]) != -1) {
                        plotBlock = getBid(arg[2]);
                        int i = 0;
                        for (i = 0; i < 256; i++) { 
                            if (strEqual(arg[1], player[i]._username, false) == true) {
                                player[i].mode = 2;
                                tunnelTeleport(player[i]._x, player[i]._y, player[i]._z, player[i]._yaw, player[i]._pitch);
                                char plotMsg[64];
                                sprintf(plotMsg, "&0[&4LavaTunnel&0] &fplotmotion: Plotmotioning '%s' with '%s'",arg[1],arg[2]);
                                tunnelMsg(plotMsg);
                                return true;
                            }
                        }
                    } else {
                        tunnelMsg("&0[&4LavaTunnel&0] &fplotmotion: unknown block");
                    }
                }
            }
            return true;
        }
        if (strcmp(arg[0], "paint") == 0) {
            arg[1] = strtok(NULL, " ");
            if (arg[1] == NULL) {
                tunnelMsg("&0[&4LavaTunnel&0] &fpaint: Missing argument.");
                return true;
            } else {
                if (getBid(arg[1]) != -1) {
                    paintBid = getBid(arg[1]);
                    paint = true;
                    char paintMsg[64];
                    sprintf(paintMsg, "&0[&4LavaTunnel&0] &fPainting with '%s'.",arg[1]);
                    tunnelMsg(paintMsg);
                } else {
                    tunnelMsg("&0[&4LavaTunnel&0] &fz: Unknown block!");
                    return true;
                }
            }
            return true;
        }
        if (strcmp(arg[0], "paste") == 0) {
            arg[1] = strtok(NULL, " ");
            if (arg[1] == NULL) {
                tunnelMsg("&0[&4LavaTunnel&0] &fpaste: missing chunkname");
                return true;
            } else {
                sprintf(paste.chunkName, "chunks\\%s.chunk\0",arg[1]);
                zPlace = true;
                paste.place = true;
                return true;
            }
        }
        if (strcmp(arg[0], "copy") == 0) {
            arg[1] = strtok(NULL, " ");
            if (arg[1] == NULL) {
                tunnelMsg("&0[&4LavaTunnel&0] &fcopy: missing chunkname");
                return true;
            } else {
                sprintf(copy.chunkName, "chunks\\%s.chunk\0",arg[1]);
                copy.halt = false;
                points = 0;
                zPlace = true;
                copy.place = true;
                return true;
            }
        }
        if (strcmp(arg[0], "z") == 0) {
            arg[1] = strtok(NULL, " ");
            if (arg[1] == NULL) { //Get block from the points
                tunnelMsg("&0[&4LavaTunnel&0] &fz: Place two points.");
                cuboid.getBlock = true;
            } else {
               if (getBid(arg[1]) != -1) {
                    cuboid.block = getBid(arg[1]);
                    cuboid.getBlock = false;
                    tunnelMsg("&0[&4LavaTunnel&0] &fz: Place two points.");
               } else {
                    tunnelMsg("&0[&4LavaTunnel&0] &fz: Unknown block!");
               }
            }
            points = 0;
            cuboid.halt = false;
            zPlace = true;
            cuboid.place = true;
            cuboid.type = 0;

            return true;
        }
        if (strcmp(arg[0], "zspeed") == 0) {
            arg[1] = strtok(NULL, " ");
            if (arg[1] == NULL) {
                tunnelMsg("&0[&4LavaTunnel&0] &fzspeed: missing argument.");
                return true;
            } else {
               if (strcmp(arg[1], "none") == 0) {
                    cuboid.zlimit = false;
                    tunnelMsg("&0[&4LavaTunnel&0] &fzspeed: no build speed delay huge cuboids may lag");
               } else {
                    cuboid.zspeed = atoi(arg[1]);
                    cuboid.zlimit = true;
               }
               char buf[64];
               sprintf(buf, "&0[&4LavaTunnel&0] &fzspeed: speed set to %dms.",cuboid.zspeed);
               tunnelMsg(buf);
            }

            return true;
        }
    	if (strcmp(arg[0], "find") == 0) {
    		arg[1] = strtok(NULL, " ");
    		if (arg[1] == NULL) {
    			tunnelMsg("&0[&4LavaTunnel&0] &ffind: missing argument.");
    			return true;
    		} else {
    			int i = 0;
    			for (i = -1; i < 256; i++) {
    				//if (strcmp(arg[1], player[i]._username) == 0) {
                    char cunt[64];
                    strcpy(cunt, arg[1]);
                    if (strEqual(cunt, player[i]._username, false) == true) {
    					char find[64];
    					sprintf(find, "&0[&4LavaTunnel&0] &f%s found at %hd, %hd, %hd (%d, %d)",player[i]._username, player[i]._x/32, player[i]._y/32, player[i]._z/32, player[i]._yaw, player[i]._pitch);
    					tunnelMsg(find);
    					return true;
    				}
    			}
    			tunnelMsg("&0[&4LavaTunnel&0] &fCan't find player.");
    		}

    		return true;
    	}
    	if (strcmp(arg[0], "ignore") == 0) {
    		arg[1] = strtok(NULL, " ");
    		if (arg[1] == NULL) {
    			tunnelMsg("&0[&4LavaTunnel&0] &fignore: missing argument.");
    			return true;
    		} else {
    			int i = 0;
    			for (i = 0; i < 256; i++) { 
    				if (strEqual(arg[1], player[i]._username, false) == true) {
    					if (player[i].ignore == true) {
    						char tmsg[64];
    						sprintf(tmsg, "&0[&4LavaTunnel&0] &fStopped ignoring %s.",player[i]._username);
    						tunnelMsg(tmsg);
    						player[i].ignore = false;
    					} else {
    						char tmsg[64];
    						sprintf(tmsg, "&0[&4LavaTunnel&0] &fIgnoring %s.",player[i]._username);
    						tunnelMsg(tmsg);
    						player[i].ignore = true;
    					}
    					return true;
    				}
    			}
    			tunnelMsg("&0[&4LavaTunnel&0] &fCan't find player.");
    		}

    		return true;
    	}
    	if (strcmp(arg[0], "follow") == 0) { //Whore
    		arg[1] = strtok(NULL, " ");
    		if (arg[1] == NULL) {
    			tunnelMsg("&0[&4LavaTunnel&0] &ffollow: missing argument.");
    			return true;
    		} else {
    			int i = 0;
    			bool found;
    			for (i = 0; i < 256; i++) { 
    				if (strstr(player[i]._username, arg[1]) != NULL) {
    					player[i].mode = 1;
    					tunnelTeleport(player[i]._x, player[i]._y, player[i]._z, player[i]._yaw, player[i]._pitch);
    					found = true;
    				}

    			}

    			if (!found) {
    				tunnelMsg("&0[&4LavaTunnel&0] &ffollow: couldn't find player.");
    			}
    			return true;
    		}
    		return true;
    	}
    	if (strcmp(arg[0], "spectate") == 0) {
    		arg[1] = strtok(NULL, " ");
    		if (arg[1] == NULL) {
    			tunnelMsg("&0[&4LavaTunnel&0] &fspectate: missing argument.");
    			return true;
    		} else {
    			int i = 0;
    			for (i = 0; i < 256; i++) { 
    				if (strEqual(arg[1], player[i]._username, false) == true) {
    					player[i].mode = 1;
    					hide = true;
    					tunnelTeleport(player[i]._x, player[i]._y, player[i]._z, player[i]._yaw, player[i]._pitch);
    					return true;
    				}
    			}
    		}
    		return true;
    	}
    }

    return false;
}