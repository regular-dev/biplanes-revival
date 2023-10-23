/*
  Biplanes Revival
  Copyright (C) 2019-2023 Regular-dev community
  https://regular-dev.org
  regular.dev.org@gmail.com

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <include/menu.hpp>
#include <include/render.hpp>
#include <include/sdl.hpp>
#include <include/controls.hpp>
#include <include/sizes.hpp>
#include <include/textures.hpp>
#include <include/variables.hpp>


void
Menu::screen_mp_dc_help()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  char textbuf[30];
  draw_text(          "      Direct connect HELP:       ", 0, sizes.screen_height * 0.100 );
  draw_text(          "           For CLIENT:           ", 0, sizes.screen_height * 0.175 );
  draw_text(          "   Make sure your  anti-virus    ", 0, sizes.screen_height * 0.250 );
  draw_text(          "  and firewall don't block this  ", 0, sizes.screen_height * 0.300 );
  draw_text(          "  game's access to the Internet. ", 0, sizes.screen_height * 0.350 );

  draw_text(          "            For HOST:            ", 0, sizes.screen_height * 0.475 );
  draw_text(          "  Make sure your firewall and    ", 0, sizes.screen_height * 0.550 );
  draw_text(          "  router don't block incoming    ", 0, sizes.screen_height * 0.600 );

  sprintf( textbuf,   "  connections at port   %d", LOCAL_PORT );
  draw_text( textbuf,                                      0, sizes.screen_height * 0.650 );
  draw_text(          "  and your opponent is provided  ", 0, sizes.screen_height * 0.700 );
  draw_text(          "  with your public IP address.   ", 0, sizes.screen_height * 0.750 );
  draw_text(          "       Or you can use any        ", 0, sizes.screen_height * 0.800 );
  draw_text(          "Virtual Private Network software ", 0, sizes.screen_height * 0.850 );
  draw_text(          " (LogMeIn Hamachi, Tunngle, etc) ", 0, sizes.screen_height * 0.900 );
}

void
Menu::screen_mp_help_page1()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_text(        "       Multiplayer  HELP:       ", 0, sizes.screen_height * 0.175 );
  draw_text(        "   Make sure your  anti-virus   ", 0, sizes.screen_height * 0.250 );
  draw_text(        " and firewall  don't block this ", 0, sizes.screen_height * 0.300 );
  draw_text(        " game's access to the Internet. ", 0, sizes.screen_height * 0.350 );

  draw_text(        "          Matchmaking:          ", 0, sizes.screen_height * 0.475 );
  draw_text(        "  Use empty password  field to  ", 0, sizes.screen_height * 0.550 );
  draw_text(        "  automatically find an opponent", 0, sizes.screen_height * 0.600 );
  draw_text(        "     and start peer-to-peer     ", 0, sizes.screen_height * 0.650 );
  draw_text(        "         game  with him         ", 0, sizes.screen_height * 0.700 );
  draw_text(        "  If you want to play with your ", 0, sizes.screen_height * 0.750 );
  draw_text(        "     friend you can specify     ", 0, sizes.screen_height * 0.800 );
  draw_text(        "a password. The session  will be", 0, sizes.screen_height * 0.850 );
  draw_text(        " established  between first two ", 0, sizes.screen_height * 0.900 );
  draw_text(        "opponents having  same passwords", 0, sizes.screen_height * 0.950 );
}

void
Menu::screen_mp_help_page2()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_text(        "  Possible matchmaking issues:  ", 0, sizes.screen_height * 0.175 );
  draw_text(        "  Successful matchmaking isn't  ", 0, sizes.screen_height * 0.250 );
  draw_text(        "  guaranteed if  your Internet  ", 0, sizes.screen_height * 0.300 );
  draw_text(        "  provider uses  Symmetric NAT  ", 0, sizes.screen_height * 0.350 );
  draw_text(        " or unpredictable port mapping. ", 0, sizes.screen_height * 0.400 );
  draw_text(        " It's very likely that you fall ", 0, sizes.screen_height * 0.450 );
  draw_text(        "  into these conditions if you  ", 0, sizes.screen_height * 0.500 );
  draw_text(        "    access Internet through     ", 0, sizes.screen_height * 0.550 );
  draw_text(        "         mobile network.        ", 0, sizes.screen_height * 0.600 );
  draw_text(        " P2P connection timeout message ", 0, sizes.screen_height * 0.650 );
  draw_text(        " means that you tried to connect", 0, sizes.screen_height * 0.700 );
  draw_text(        " to found opponent  but failed. ", 0, sizes.screen_height * 0.750 );
  draw_text(        "    If it happens every time -  ", 0, sizes.screen_height * 0.800 );
  draw_text(        "unfortunately you  won't be able", 0, sizes.screen_height * 0.850 );
  draw_text(        "   to use matchmaking feature   ", 0, sizes.screen_height * 0.900 );
}

void
Menu::screen_mp_help_page3()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, sizes.screen_height * 0.175 );
  draw_text(        "Connecting to matchmake server: ", 0, sizes.screen_height * 0.250 );
  draw_text(        " Game tries to connect to our   ", 0, sizes.screen_height * 0.300 );
  draw_text(        " matchmaking server.            ", 0, sizes.screen_height * 0.350 );
  draw_text(        " Seeing this message longer than", 0, sizes.screen_height * 0.400 );
  draw_text(        " 5-10 seconds means that our    ", 0, sizes.screen_height * 0.450 );
  draw_text(        " matchmaking server is down for ", 0, sizes.screen_height * 0.500 );
  draw_text(        " some reason.                   ", 0, sizes.screen_height * 0.550 );
  draw_text(        " If the issue persists longer   ", 0, sizes.screen_height * 0.600 );
  draw_text(        " than a day please contact us to", 0, sizes.screen_height * 0.650 );
  draw_text(        " resolve it.                    ", 0, sizes.screen_height * 0.700 );
  draw_text(        "                                ", 0, sizes.screen_height * 0.750 );
  draw_text(        "                                ", 0, sizes.screen_height * 0.800 );
  draw_text(        "                                ", 0, sizes.screen_height * 0.850 );
  draw_text(        "                                ", 0, sizes.screen_height * 0.900 );
}

void
Menu::screen_mp_help_page4()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, sizes.screen_height * 0.175 );
  draw_text(        "Bad matchmake server reply:     ", 0, sizes.screen_height * 0.250 );
  draw_text(        " It's almost impossible to get  ", 0, sizes.screen_height * 0.300 );
  draw_text(        " this message, however if you do", 0, sizes.screen_height * 0.350 );
  draw_text(        " - please let us know the exact ", 0, sizes.screen_height * 0.400 );
  draw_text(        " time when you got this error   ", 0, sizes.screen_height * 0.450 );
  draw_text(        " so we can check our logs and   ", 0, sizes.screen_height * 0.500 );
  draw_text(        " find the issue.                ", 0, sizes.screen_height * 0.550 );
  draw_text(        "                                ", 0, sizes.screen_height * 0.600 );
  draw_text(        "Searching for opponent:         ", 0, sizes.screen_height * 0.650 );
  draw_text(        " You received an echo reply from", 0, sizes.screen_height * 0.700 );
  draw_text(        " matchmaking server and now you ", 0, sizes.screen_height * 0.750 );
  draw_text(        " are waiting for an opponent.   ", 0, sizes.screen_height * 0.800 );
  draw_text(        " If there aren't any opponents  ", 0, sizes.screen_height * 0.850 );
  draw_text(        " the game won't start obviously.", 0, sizes.screen_height * 0.900 );
}

void
Menu::screen_mp_help_page5()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, sizes.screen_height * 0.175 );
  draw_text(        "Establishing p2p connection:    ", 0, sizes.screen_height * 0.250 );
  draw_text(        " You found an opponent and now  ", 0, sizes.screen_height * 0.300 );
  draw_text(        " you're trying to connect to him", 0, sizes.screen_height * 0.350 );
  draw_text(        "                                ", 0, sizes.screen_height * 0.400 );
  draw_text(        "Waiting for opponent answer:    ", 0, sizes.screen_height * 0.450 );
  draw_text(        " You performed all required     ", 0, sizes.screen_height * 0.500 );
  draw_text(        " actions and wait for opponent  ", 0, sizes.screen_height * 0.550 );
  draw_text(        " to do the same. If the game    ", 0, sizes.screen_height * 0.600 );
  draw_text(        " doesn't start in 10 seconds    ", 0, sizes.screen_height * 0.650 );
  draw_text(        " you will get following error   ", 0, sizes.screen_height * 0.700 );
  draw_text(        " described in the next page:    ", 0, sizes.screen_height * 0.750 );
  draw_text(        "Peer-to-peer connection timeout!", 0, sizes.screen_height * 0.800 );
}

void
Menu::screen_mp_help_page6()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, sizes.screen_height * 0.175 );
  draw_text(        "Peer-to-peer connection timeout:", 0, sizes.screen_height * 0.250 );
  draw_text(        " Something went wrong and       ", 0, sizes.screen_height * 0.300 );
  draw_text(        " connection wasn't established. ", 0, sizes.screen_height * 0.350 );
  draw_text(        " If you get this error every    ", 0, sizes.screen_height * 0.400 );
  draw_text(        " time - unfortunately that means", 0, sizes.screen_height * 0.450 );
  draw_text(        " matchmaking is not possible    ", 0, sizes.screen_height * 0.500 );
  draw_text(        " with your current Internet     ", 0, sizes.screen_height * 0.550 );
  draw_text(        " provider.                      ", 0, sizes.screen_height * 0.600 );
  draw_text(        " You can still play using Direct", 0, sizes.screen_height * 0.650 );
  draw_text(        " connect if you or your friend  ", 0, sizes.screen_height * 0.700 );
  draw_text(        " manage to setup port forwarding", 0, sizes.screen_height * 0.750 );
  draw_text(        " or Virtual Private Network     ", 0, sizes.screen_height * 0.800 );
  draw_text(        " software.                      ", 0, sizes.screen_height * 0.850 );
  draw_text(        "                                ", 0, sizes.screen_height * 0.900 );
}

void
Menu::screen_mp_help_page7()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, sizes.screen_height * 0.175 );
  draw_text(        "Connection failed:              ", 0, sizes.screen_height * 0.250 );
  draw_text(        " In Direct Connect mode you can ", 0, sizes.screen_height * 0.300 );
  draw_text(        " see this message if your       ", 0, sizes.screen_height * 0.350 );
  draw_text(        " opponent stopped hosting or he ", 0, sizes.screen_height * 0.400 );
  draw_text(        " is unreacheable (closed port or", 0, sizes.screen_height * 0.450 );
  draw_text(        " private IP address probably)   ", 0, sizes.screen_height * 0.500 );

  draw_text(        " In Matchmaking mode this       ", 0, sizes.screen_height * 0.600 );
  draw_text(        " message appears only if your   ", 0, sizes.screen_height * 0.650 );
  draw_text(        " opponent aborted the game      ", 0, sizes.screen_height * 0.700 );
  draw_text(        " before you managed to connect. ", 0, sizes.screen_height * 0.750 );
}

void
Menu::screen_mp_help_page8()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, sizes.screen_height * 0.175 );
  draw_text(        "Connection timed out:           ", 0, sizes.screen_height * 0.250 );
  draw_text(        " This message may appear if you ", 0, sizes.screen_height * 0.300 );
  draw_text(        " didn't receive packets from    ", 0, sizes.screen_height * 0.350 );
  draw_text(        " your opponent for a long time. ", 0, sizes.screen_height * 0.400 );
  draw_text(        " This may occur if his game     ", 0, sizes.screen_height * 0.450 );
  draw_text(        " suddenly crashed or he lost    ", 0, sizes.screen_height * 0.500 );
  draw_text(        " Internet connection.           ", 0, sizes.screen_height * 0.550 );
  draw_text(        "Server ceased connection or     ", 0, sizes.screen_height * 0.600 );
  draw_text(        "Client disconnected:            ", 0, sizes.screen_height * 0.650 );
  draw_text(        " Obviously your opponent just   ", 0, sizes.screen_height * 0.700 );
  draw_text(        " exited game session.           ", 0, sizes.screen_height * 0.750 );
  draw_text(        "All other messages may indicate ", 0, sizes.screen_height * 0.850 );
  draw_text(        " problems with initializing     ", 0, sizes.screen_height * 0.900 );
  draw_text(        " network systems.               ", 0, sizes.screen_height * 0.950 );
}

void
Menu::screen_help()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );


  const SDL_Rect helpRect
  {
    0, 0,
    sizes.screen_width,
    sizes.screen_height,
  };

  SDL_RenderCopy(
    gRenderer,
    textures.menu_help,
    nullptr,
    &helpRect );


  const SDL_Rect planeRect
  {
    sizes.screen_width * 0.5 - sizes.plane_sizex * 0.5,
    sizes.screen_height * 0.2 - sizes.plane_sizey * 0.5,
    sizes.plane_sizex,
    sizes.plane_sizey,
  };

  SDL_RenderCopyEx(
    gRenderer,
    textures.texture_biplane_b,
    nullptr,
    &planeRect,
    337.5,
    nullptr,
    SDL_FLIP_NONE );


//  Accelerate
  draw_text( "Accelerate",        sizes.screen_width * 0.360, sizes.screen_height * 0.05 );
  char textbuf[30];
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(THROTTLE_UP) );
  draw_text( textbuf,             sizes.screen_width * 0.400, sizes.screen_height * 0.1 );


//  Turn left
  draw_text( "Turn Anti       ",  0,                          sizes.screen_height * 0.100 );
  draw_text( "Clockwise       ",  0,                          sizes.screen_height * 0.150 );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(TURN_LEFT) );
  draw_text( textbuf,             0,                          sizes.screen_height * 0.200 );


//  Turn right
  draw_text( "Turn            ",  sizes.screen_width * 0.600, sizes.screen_height * 0.150 );
  draw_text( "Clockwise       ",  sizes.screen_width * 0.600, sizes.screen_height * 0.200 );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(TURN_RIGHT) );
  draw_text( textbuf,             sizes.screen_width * 0.600, sizes.screen_height * 0.250 );


//  Decelerate
  draw_text( "Decelerate      ",  sizes.screen_width * 0.200, sizes.screen_height * 0.250 );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(THROTTLE_DOWN) );
  draw_text( textbuf,             sizes.screen_width * 0.200, sizes.screen_height * 0.300 );


//  Fire
  draw_text( " Fire           ",  sizes.screen_width * 0.125, sizes.screen_height * 0.350 );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(FIRE) );
  draw_text( textbuf,             sizes.screen_width * 0.275, sizes.screen_height * 0.350 );


//  Jump
  draw_text( "Eject           ",  sizes.screen_width * 0.125, sizes.screen_height * 0.400 );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(JUMP) );
  draw_text( textbuf,             sizes.screen_width * 0.275, sizes.screen_height * 0.400 );


//  Move left
  draw_text( "Move            ",  sizes.screen_width * 0.025, sizes.screen_height * 0.650 );
  draw_text( "Left            ",  sizes.screen_width * 0.025, sizes.screen_height * 0.700 );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(TURN_LEFT) );
  draw_text( textbuf,             sizes.screen_width * 0.025, sizes.screen_height * 0.750 );


//  Move right
  draw_text( " Move           ",  sizes.screen_width * 0.325, sizes.screen_height * 0.650 );
  draw_text( "Right           ",  sizes.screen_width * 0.325, sizes.screen_height * 0.700 );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(TURN_RIGHT) );
  draw_text( textbuf,             sizes.screen_width * 0.325, sizes.screen_height * 0.600 );


//  Run into the barn!
  draw_text( "Run into        ",  sizes.screen_width * 0.025, sizes.screen_height * 0.850 );
  draw_text( "      the barn! ",  sizes.screen_width * 0.025, sizes.screen_height * 0.900 );


//  Fly left
  draw_text( "Move            ",  sizes.screen_width * 0.525, sizes.screen_height * 0.650 );
  draw_text( "Left            ",  sizes.screen_width * 0.525, sizes.screen_height * 0.700 );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(TURN_LEFT) );
  draw_text( textbuf,             sizes.screen_width * 0.525, sizes.screen_height * 0.750 );


//  Fly right
  draw_text( " Move           ",  sizes.screen_width * 0.825, sizes.screen_height * 0.650 );
  draw_text( "Right           ",  sizes.screen_width * 0.825, sizes.screen_height * 0.700 );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(TURN_RIGHT) );
  draw_text( textbuf,             sizes.screen_width * 0.750, sizes.screen_height * 0.600 );


//  Open Parachute
  draw_text( "Open Parachute  ",  sizes.screen_width * 0.525, sizes.screen_height * 0.850 );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(JUMP) );
  draw_text( textbuf,             sizes.screen_width * 0.525, sizes.screen_height * 0.900 );
}
