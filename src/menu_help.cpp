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
#include <include/constants.hpp>
#include <include/textures.hpp>
#include <include/variables.hpp>


void
Menu::screen_mp_dc_help()
{
  setRenderColor(constants::colors::background);
  SDL_RenderClear( gRenderer );

  draw_text(          "      Direct connect HELP:       ", 0, 0.100f );
  draw_text(          "           For CLIENT:           ", 0, 0.175f );
  draw_text(          "   Make sure your  anti-virus    ", 0, 0.250f );
  draw_text(          "  and firewall don't block this  ", 0, 0.300f );
  draw_text(          "  game's access to the Internet. ", 0, 0.350f );

  draw_text(          "            For HOST:            ", 0, 0.475f );
  draw_text(          "  Make sure your firewall and    ", 0, 0.550f );
  draw_text(          "  router don't block incoming    ", 0, 0.600f );

  const auto text =   "  connections at port   " + std::to_string(LOCAL_PORT);
  draw_text( text,                                         0, 0.650f );
  draw_text(          "  and your opponent is provided  ", 0, 0.700f );
  draw_text(          "  with your public IP address.   ", 0, 0.750f );
  draw_text(          "       Or you can use any        ", 0, 0.800f );
  draw_text(          "Virtual Private Network software ", 0, 0.850f );
  draw_text(          " (LogMeIn Hamachi, Tunngle, etc) ", 0, 0.900f );
}

void
Menu::screen_mp_help_page1()
{
  setRenderColor(constants::colors::background);
  SDL_RenderClear( gRenderer );

  draw_text(        "       Multiplayer  HELP:       ", 0, 0.175f );
  draw_text(        "   Make sure your  anti-virus   ", 0, 0.250f );
  draw_text(        " and firewall  don't block this ", 0, 0.300f );
  draw_text(        " game's access to the Internet. ", 0, 0.350f );

  draw_text(        "          Matchmaking:          ", 0, 0.475f );
  draw_text(        "  Use empty password  field to  ", 0, 0.550f );
  draw_text(        "  automatically find an opponent", 0, 0.600f );
  draw_text(        "     and start peer-to-peer     ", 0, 0.650f );
  draw_text(        "         game  with him         ", 0, 0.700f );
  draw_text(        "  If you want to play with your ", 0, 0.750f );
  draw_text(        "     friend you can specify     ", 0, 0.800f );
  draw_text(        "a password. The session  will be", 0, 0.850f );
  draw_text(        " established  between first two ", 0, 0.900f );
  draw_text(        "opponents having  same passwords", 0, 0.950f );
}

void
Menu::screen_mp_help_page2()
{
  setRenderColor(constants::colors::background);
  SDL_RenderClear( gRenderer );

  draw_text(        "  Possible matchmaking issues:  ", 0, 0.175f );
  draw_text(        "  Successful matchmaking isn't  ", 0, 0.250f );
  draw_text(        "  guaranteed if  your Internet  ", 0, 0.300f );
  draw_text(        "  provider uses  Symmetric NAT  ", 0, 0.350f );
  draw_text(        " or unpredictable port mapping. ", 0, 0.400f );
  draw_text(        " It's very likely that you fall ", 0, 0.450f );
  draw_text(        "  into these conditions if you  ", 0, 0.500f );
  draw_text(        "    access Internet through     ", 0, 0.550f );
  draw_text(        "         mobile network.        ", 0, 0.600f );
  draw_text(        " P2P connection timeout message ", 0, 0.650f );
  draw_text(        " means that you tried to connect", 0, 0.700f );
  draw_text(        " to found opponent  but failed. ", 0, 0.750f );
  draw_text(        "    If it happens every time -  ", 0, 0.800f );
  draw_text(        "unfortunately you  won't be able", 0, 0.850f );
  draw_text(        "   to use matchmaking feature   ", 0, 0.900f );
}

void
Menu::screen_mp_help_page3()
{
  setRenderColor(constants::colors::background);
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, 0.175f );
  draw_text(        "Connecting to matchmake server: ", 0, 0.250f );
  draw_text(        " Game tries to connect to our   ", 0, 0.300f );
  draw_text(        " matchmaking server.            ", 0, 0.350f );
  draw_text(        " Seeing this message longer than", 0, 0.400f );
  draw_text(        " 5-10 seconds means that our    ", 0, 0.450f );
  draw_text(        " matchmaking server is down for ", 0, 0.500f );
  draw_text(        " some reason.                   ", 0, 0.550f );
  draw_text(        " If the issue persists longer   ", 0, 0.600f );
  draw_text(        " than a day please contact us to", 0, 0.650f );
  draw_text(        " resolve it.                    ", 0, 0.700f );
  draw_text(        "                                ", 0, 0.750f );
  draw_text(        "                                ", 0, 0.800f );
  draw_text(        "                                ", 0, 0.850f );
  draw_text(        "                                ", 0, 0.900f );
}

void
Menu::screen_mp_help_page4()
{
  setRenderColor(constants::colors::background);
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, 0.175f );
  draw_text(        "Bad matchmake server reply:     ", 0, 0.250f );
  draw_text(        " It's almost impossible to get  ", 0, 0.300f );
  draw_text(        " this message, however if you do", 0, 0.350f );
  draw_text(        " - please let us know the exact ", 0, 0.400f );
  draw_text(        " time when you got this error   ", 0, 0.450f );
  draw_text(        " so we can check our logs and   ", 0, 0.500f );
  draw_text(        " find the issue.                ", 0, 0.550f );
  draw_text(        "                                ", 0, 0.600f );
  draw_text(        "Searching for opponent:         ", 0, 0.650f );
  draw_text(        " You received an echo reply from", 0, 0.700f );
  draw_text(        " matchmaking server and now you ", 0, 0.750f );
  draw_text(        " are waiting for an opponent.   ", 0, 0.800f );
  draw_text(        " If there aren't any opponents  ", 0, 0.850f );
  draw_text(        " the game won't start obviously.", 0, 0.900f );
}

void
Menu::screen_mp_help_page5()
{
  setRenderColor(constants::colors::background);
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, 0.175f );
  draw_text(        "Establishing p2p connection:    ", 0, 0.250f );
  draw_text(        " You found an opponent and now  ", 0, 0.300f );
  draw_text(        " you're trying to connect to him", 0, 0.350f );
  draw_text(        "                                ", 0, 0.400f );
  draw_text(        "Waiting for opponent answer:    ", 0, 0.450f );
  draw_text(        " You performed all required     ", 0, 0.500f );
  draw_text(        " actions and wait for opponent  ", 0, 0.550f );
  draw_text(        " to do the same. If the game    ", 0, 0.600f );
  draw_text(        " doesn't start in 10 seconds    ", 0, 0.650f );
  draw_text(        " you will get following error   ", 0, 0.700f );
  draw_text(        " described in the next page:    ", 0, 0.750f );
  draw_text(        "Peer-to-peer connection timeout!", 0, 0.800f );
}

void
Menu::screen_mp_help_page6()
{
  setRenderColor(constants::colors::background);
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, 0.175f );
  draw_text(        "Peer-to-peer connection timeout:", 0, 0.250f );
  draw_text(        " Something went wrong and       ", 0, 0.300f );
  draw_text(        " connection wasn't established. ", 0, 0.350f );
  draw_text(        " If you get this error every    ", 0, 0.400f );
  draw_text(        " time - unfortunately that means", 0, 0.450f );
  draw_text(        " matchmaking is not possible    ", 0, 0.500f );
  draw_text(        " with your current Internet     ", 0, 0.550f );
  draw_text(        " provider.                      ", 0, 0.600f );
  draw_text(        " You can still play using Direct", 0, 0.650f );
  draw_text(        " connect if you or your friend  ", 0, 0.700f );
  draw_text(        " manage to setup port forwarding", 0, 0.750f );
  draw_text(        " or Virtual Private Network     ", 0, 0.800f );
  draw_text(        " software.                      ", 0, 0.850f );
  draw_text(        "                                ", 0, 0.900f );
}

void
Menu::screen_mp_help_page7()
{
  setRenderColor(constants::colors::background);
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, 0.175f );
  draw_text(        "Connection failed:              ", 0, 0.250f );
  draw_text(        " In Direct Connect mode you can ", 0, 0.300f );
  draw_text(        " see this message if your       ", 0, 0.350f );
  draw_text(        " opponent stopped hosting or he ", 0, 0.400f );
  draw_text(        " is unreacheable (closed port or", 0, 0.450f );
  draw_text(        " private IP address probably)   ", 0, 0.500f );

  draw_text(        " In Matchmaking mode this       ", 0, 0.600f );
  draw_text(        " message appears only if your   ", 0, 0.650f );
  draw_text(        " opponent aborted the game      ", 0, 0.700f );
  draw_text(        " before you managed to connect. ", 0, 0.750f );
}

void
Menu::screen_mp_help_page8()
{
  setRenderColor(constants::colors::background);
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, 0.175f );
  draw_text(        "Connection timed out:           ", 0, 0.250f );
  draw_text(        " This message may appear if you ", 0, 0.300f );
  draw_text(        " didn't receive packets from    ", 0, 0.350f );
  draw_text(        " your opponent for a long time. ", 0, 0.400f );
  draw_text(        " This may occur if his game     ", 0, 0.450f );
  draw_text(        " suddenly crashed or he lost    ", 0, 0.500f );
  draw_text(        " Internet connection.           ", 0, 0.550f );
  draw_text(        "Server ceased connection or     ", 0, 0.600f );
  draw_text(        "Client disconnected:            ", 0, 0.650f );
  draw_text(        " Obviously your opponent just   ", 0, 0.700f );
  draw_text(        " exited game session.           ", 0, 0.750f );
  draw_text(        "All other messages may indicate ", 0, 0.850f );
  draw_text(        " problems with initializing     ", 0, 0.900f );
  draw_text(        " network systems.               ", 0, 0.950f );
}

void
Menu::screen_help()
{
  namespace plane = constants::plane;


  setRenderColor(constants::colors::background);
  SDL_RenderClear( gRenderer );


  const SDL_FRect helpRect
  {
    toWindowSpaceX(0.0f),
    toWindowSpaceY(0.0f),
    scaleToScreenX(1.0f),
    scaleToScreenY(1.0f),
  };

  SDL_RenderCopyF(
    gRenderer,
    textures.menu_help,
    nullptr,
    &helpRect );


  const SDL_FRect planeRect
  {
    toWindowSpaceX(0.5f - plane::sizeX * 0.5f),
//    TODO: move to constants
    toWindowSpaceY(0.2f - plane::sizeY * 0.5f),
    scaleToScreenX(plane::sizeX),
    scaleToScreenY(plane::sizeY),
  };

  SDL_RenderCopyExF(
    gRenderer,
    textures.texture_biplane_b,
    nullptr,
    &planeRect,
    337.5,
    nullptr,
    SDL_FLIP_NONE );


//  Accelerate
  draw_text( "Accelerate",        0.360f, 0.05f );
  char textbuf[30];
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(THROTTLE_UP) );
  draw_text( textbuf,             0.400f, 0.1f );


//  Turn left
  draw_text( "Turn Anti       ",  0, 0.100f );
  draw_text( "Clockwise       ",  0, 0.150f );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(TURN_LEFT) );
  draw_text( textbuf,             0, 0.200f );


//  Turn right
  draw_text( "Turn            ",  0.600f, 0.150f );
  draw_text( "Clockwise       ",  0.600f, 0.200f );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(TURN_RIGHT) );
  draw_text( textbuf,             0.600f, 0.250f );


//  Decelerate
  draw_text( "Decelerate      ",  0.200f, 0.250f );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(THROTTLE_DOWN) );
  draw_text( textbuf,             0.200f, 0.300f );


//  Fire
  draw_text( " Fire           ",  0.125f, 0.350f );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(FIRE) );
  draw_text( textbuf,             0.275f, 0.350f );


//  Jump
  draw_text( "Eject           ",  0.125f, 0.400f );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(JUMP) );
  draw_text( textbuf,             0.275f, 0.400f );


//  Move left
  draw_text( "Move            ",  0.025f, 0.650f );
  draw_text( "Left            ",  0.025f, 0.700f );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(TURN_LEFT) );
  draw_text( textbuf,             0.025f, 0.750f );


//  Move right
  draw_text( " Move           ",  0.325f, 0.650f );
  draw_text( "Right           ",  0.325f, 0.700f );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(TURN_RIGHT) );
  draw_text( textbuf,             0.325f, 0.600f );


//  Run into the barn!
  draw_text( "Run into        ",  0.025f, 0.850f );
  draw_text( "      the barn! ",  0.025f, 0.900f );


//  Fly left
  draw_text( "Move            ",  0.525f, 0.650f );
  draw_text( "Left            ",  0.525f, 0.700f );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(TURN_LEFT) );
  draw_text( textbuf,             0.525f, 0.750f );


//  Fly right
  draw_text( " Move           ",  0.825f, 0.650f );
  draw_text( "Right           ",  0.825f, 0.700f );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(TURN_RIGHT) );
  draw_text( textbuf,             0.750f, 0.600f );


//  Open Parachute
  draw_text( "Open Parachute  ",  0.525f, 0.850f );
  sprintf( textbuf, "[%s]",       SDL_GetKeyName(JUMP) );
  draw_text( textbuf,             0.525f, 0.900f );
}
