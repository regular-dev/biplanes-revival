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
#include <include/sdl.hpp>
#include <include/render.hpp>
#include <include/plane.hpp>
#include <include/game_state.hpp>
#include <include/stats.hpp>
#include <include/sizes.hpp>
#include <include/variables.hpp>
#include <include/utility.hpp>


void
Menu::screen_stats_recent()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear(gRenderer);


  const auto& recentStats = gameState().stats.recent;
  const auto& redStats = recentStats.at(PLANE_TYPE::RED);
  const auto& blueStats = recentStats.at(PLANE_TYPE::BLUE);


  draw_text(        "Last dogfight stats (blue / red)", 0, 0 );
  char textbuf[40];
  sprintf( textbuf, "  shots: %u / %u", blueStats.shots, redStats.shots );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.050 );
  sprintf( textbuf, "  plane hits: %u / %u", blueStats.plane_hits, redStats.plane_hits );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.100 );
  sprintf( textbuf, "  chute hits: %u / %u", blueStats.chute_hits, redStats.chute_hits );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.150 );
  sprintf( textbuf, "  pilot hits: %u / %u", blueStats.pilot_hits, redStats.pilot_hits );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.200 );
  sprintf( textbuf, "  misses: %u / %u", blueStats.misses, redStats.misses );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.250 );
  sprintf( textbuf, "Accuracy: %.2f%% / %.2f%%", blueStats.accuracy, redStats.accuracy );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.300 );
  sprintf( textbuf, "Average shots per kill: %.2f / %.2f", blueStats.avgBulletsPerKill, redStats.avgBulletsPerKill );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.350 );

  sprintf( textbuf, "  jumps: %u / %u", blueStats.jumps, redStats.jumps );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.450 );
  sprintf( textbuf, "  crashes: %u / %u", blueStats.crashes, redStats.crashes );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.500 );
  sprintf( textbuf, "  fall deaths: %u / %u", blueStats.falls, redStats.falls );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.550 );
  sprintf( textbuf, "  successful jumps: %u / %u", blueStats.rescues, redStats.rescues );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.600 );
  sprintf( textbuf, "Self-preservation: %.2f%% / %.2f%%", blueStats.selfPreservation, redStats.selfPreservation );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.650 );

  sprintf( textbuf, "  plane kills: %u / %u", blueStats.plane_kills, redStats.plane_kills );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.750 );
  sprintf( textbuf, "  total kills: %u / %u", blueStats.totalKills, redStats.totalKills );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.800 );
  sprintf( textbuf, "  total deaths: %u / %u", blueStats.deaths, redStats.deaths );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.850 );
  sprintf( textbuf, "K-D ratio: %.2f / %.2f", blueStats.kdRatio, redStats.kdRatio );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.900 );
  sprintf( textbuf, "Survivability: %.2f%% / %.2f%%", blueStats.survivability, redStats.survivability );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.950 );
}

void
Menu::screen_stats_total_page1()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear(gRenderer);

  const auto& stats = gameState().stats.total;

  draw_text(        "          Total stats:          ", 0, 0 );
  char textbuf[40];
  sprintf( textbuf, "  %d shots", stats.shots );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.050 );
  sprintf( textbuf, "  %d plane hits", stats.plane_hits );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.100 );
  sprintf( textbuf, "  %d chute hits", stats.chute_hits );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.150 );
  sprintf( textbuf, "  %d pilot hits", stats.pilot_hits );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.200 );
  sprintf( textbuf, "  %d misses", stats.misses );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.250 );
  sprintf( textbuf, "Accuracy: %.2f%%", stats.accuracy );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.300 );
  sprintf( textbuf, "Average shots per kill: %.2f", stats.avgBulletsPerKill );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.350 );

  sprintf( textbuf, "  %d jumps", stats.jumps );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.450 );
  sprintf( textbuf, "  %d crashes", stats.crashes );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.500 );
  sprintf( textbuf, "  %d fall deaths", stats.falls );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.550 );
  sprintf( textbuf, "  %d successful jumps", stats.rescues );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.600 );
  sprintf( textbuf, "Self-preservation: %.2f%%", stats.selfPreservation );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.650 );
}

void
Menu::screen_stats_total_page2()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear(gRenderer);

  const auto& stats = gameState().stats.total;


  const float winLose = std::isnan( (float) stats.wins / stats.losses )
                      ? 0.0f : stats.wins * 100.0f / stats.losses;

  draw_text(        "          Total stats:          ", 0, 0 );
  char textbuf[40];
  sprintf( textbuf, "  %d plane kills", stats.plane_kills );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.100 );
  sprintf( textbuf, "  %d pilot kills", stats.pilot_hits );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.150 );
  sprintf( textbuf, "  %d total kills", stats.totalKills );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.200 );
  sprintf( textbuf, "  %d total deaths", stats.deaths );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.250 );
  sprintf( textbuf, "K-D ratio: %.2f", stats.kdRatio );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.300 );
  sprintf( textbuf, "Survivability: %.2f%%", stats.survivability );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.350 );

  sprintf( textbuf, "  %d wins", stats.wins );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.450 );
  sprintf( textbuf, "  %d losses", stats.losses );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.500 );
  sprintf( textbuf, "Win/Lose: %.2f%%", winLose );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.550 );
}


void
calcDerivedStats(
  Statistics& stats )
{
  stats.totalHits         = stats.plane_hits + stats.chute_hits + stats.pilot_hits;
  stats.misses            = stats.shots - stats.totalHits;
  stats.accuracy          = std::isnan( (float) stats.totalHits / stats.shots)
                          ? 0.0f : stats.totalHits * 100.0f / stats.shots;

  stats.suicides          = stats.crashes + stats.falls;
  stats.selfPreservation  = std::isnan( (float) stats.rescues / (stats.suicides + stats.rescues) )
                          ? 0.0f : stats.rescues * 100.0f / (stats.suicides + stats.rescues);

  stats.totalKills        = stats.plane_kills + stats.pilot_hits;
  stats.kdRatio           = std::isnan( (float) stats.totalKills / (stats.suicides + stats.deaths) )
                          ? 0.0f : (float) stats.totalKills / ( stats.suicides + stats.deaths );

  stats.survivability     = std::isnan( (float) stats.rescues / (stats.deaths + stats.suicides + stats.rescues) )
                          ? 0.0f : stats.rescues * 100.0f / (stats.deaths + stats.suicides + stats.rescues);

  stats.avgBulletsPerKill = std::isnan( (float) stats.shots / stats.totalKills )
                          ? 0.0f : (float) stats.shots / stats.totalKills;

  if ( (stats.totalKills > 0 || stats.deaths > 0) &&
        stats.suicides + stats.rescues == 0 )
    stats.selfPreservation = 100.0f;

  if (  stats.totalKills > 0 &&
        stats.rescues + stats.deaths + stats.suicides == 0 )
    stats.survivability = 100.0f;
}

void
updateRecentStats()
{
  log_message("updateRecentStats()\n");

  for ( const auto& [planeType, plane] : planes )
    gameState().stats.recent[planeType] = plane.stats();
}

void
resetRecentStats()
{
  log_message("resetRecentStats()\n");

  for ( auto& [planeType, stats] : gameState().stats.recent )
    stats = {};
}

void
updateTotalStats()
{
  log_message("updateTotalStats()\n");

  auto& game = gameState();

  const auto& planeRed = planes.at(PLANE_TYPE::RED);
  const auto& planeBlue = planes.at(PLANE_TYPE::BLUE);

  const auto& playerPlane =
    planeRed.isBot() == false && planeRed.isLocal() == true
    ? planeRed : planeBlue;

  const auto& playerStats = game.stats.recent[playerPlane.type()];
  auto& totalStats = game.stats.total;

  totalStats.chute_hits  += playerStats.chute_hits;
  totalStats.crashes     += playerStats.crashes;
  totalStats.deaths      += playerStats.deaths;
  totalStats.falls       += playerStats.falls;
  totalStats.jumps       += playerStats.jumps;
  totalStats.losses      += playerStats.losses;
  totalStats.pilot_hits  += playerStats.pilot_hits;
  totalStats.plane_hits  += playerStats.plane_hits;
  totalStats.plane_kills += playerStats.plane_kills;
  totalStats.rescues     += playerStats.rescues;
  totalStats.shots       += playerStats.shots;
  totalStats.wins        += playerStats.wins;
}
