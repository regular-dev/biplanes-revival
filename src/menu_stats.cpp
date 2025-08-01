/*
  Biplanes Revival
  Copyright (C) 2019-2025 Regular-dev community
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
#include <include/constants.hpp>
#include <include/game_state.hpp>
#include <include/stats.hpp>
#include <include/variables.hpp>
#include <include/utility.hpp>

#include <cmath>


void
Menu::screen_stats_recent_page1()
{
  setRenderColor(constants::colors::background);
  SDL_RenderClear(gRenderer);


  const auto& recentStats = gameState().stats.recent;
  const auto& redStats = recentStats.at(PLANE_TYPE::RED);
  const auto& blueStats = recentStats.at(PLANE_TYPE::BLUE);

  char textbuf[40];

  draw_text(        "Last dogfight stats (blue / red)", 0.f, 0.100f );

  sprintf( textbuf, "  shots: %u / %u", blueStats.shots, redStats.shots );
  draw_text( textbuf, 0.025f, 0.200f );

  sprintf( textbuf, "  plane hits: %u / %u", blueStats.plane_hits, redStats.plane_hits );
  draw_text( textbuf, 0.025f, 0.250f );

  sprintf( textbuf, "  chute hits: %u / %u", blueStats.chute_hits, redStats.chute_hits );
  draw_text( textbuf, 0.025f, 0.300f );

  sprintf( textbuf, "  pilot hits: %u / %u", blueStats.pilot_hits, redStats.pilot_hits );
  draw_text( textbuf, 0.025f, 0.350f );

  sprintf( textbuf, "  misses: %u / %u", blueStats.misses, redStats.misses );
  draw_text( textbuf, 0.025f, 0.400f );

  sprintf( textbuf, "Accuracy: %.2f%% / %.2f%%", blueStats.accuracy, redStats.accuracy );
  draw_text( textbuf, 0, 0.450f );

  sprintf( textbuf, "Shots per kill: %.2f / %.2f", blueStats.shotsPerKill, redStats.shotsPerKill );
  draw_text( textbuf, 0, 0.500f );


  sprintf( textbuf, "  jumps: %u / %u", blueStats.jumps, redStats.jumps );
  draw_text( textbuf, 0.025f, 0.650f );

  sprintf( textbuf, "  crashes: %u / %u", blueStats.crashes, redStats.crashes );
  draw_text( textbuf, 0.025f, 0.700f );

  sprintf( textbuf, "  fall deaths: %u / %u", blueStats.falls, redStats.falls );
  draw_text( textbuf, 0.025f, 0.750f );

  sprintf( textbuf, "  barn respawns: %u / %u", blueStats.rescues, redStats.rescues );
  draw_text( textbuf, 0.025f, 0.800f );

  draw_text( "Self-preservation:", 0, 0.850f );

  sprintf( textbuf, "%.2f%% / %.2f%%", blueStats.selfPreservation, redStats.selfPreservation );
  draw_text( textbuf, 0.400f, 0.900f );
}

void
Menu::screen_stats_recent_page2()
{
  setRenderColor(constants::colors::background);
  SDL_RenderClear(gRenderer);


  const auto& recentStats = gameState().stats.recent;
  const auto& redStats = recentStats.at(PLANE_TYPE::RED);
  const auto& blueStats = recentStats.at(PLANE_TYPE::BLUE);

  char textbuf[40];

  draw_text(        "Last dogfight stats (blue / red)", 0.f, 0.100f );

  sprintf( textbuf, "  plane kills: %u / %u", blueStats.plane_kills, redStats.plane_kills );
  draw_text( textbuf, 0.025f, 0.200f );

  sprintf( textbuf, "  pilot kills: %u / %u", blueStats.pilot_hits, redStats.pilot_hits );
  draw_text( textbuf, 0.025f, 0.250f );

  sprintf( textbuf, "  plane deaths: %u / %u", blueStats.plane_deaths, redStats.plane_deaths );
  draw_text( textbuf, 0.025f, 0.300f );

  sprintf( textbuf, "  pilot deaths: %u / %u", blueStats.pilot_deaths, redStats.pilot_deaths );
  draw_text( textbuf, 0.025f, 0.350f );


  sprintf( textbuf, "  total kills: %u / %u", blueStats.totalKills, redStats.totalKills );
  draw_text( textbuf, 0.025f, 0.450f );

  sprintf( textbuf, "  total deaths: %u / %u", blueStats.totalDeaths, redStats.totalDeaths );
  draw_text( textbuf, 0.025f, 0.500f );

  sprintf( textbuf, "K-D ratio: %.2f / %.2f", blueStats.kdRatio, redStats.kdRatio );
  draw_text( textbuf, 0, 0.550f );

  draw_text( "Survivability:", 0, 0.600f );

  sprintf( textbuf, "%.2f%% / %.2f%%", blueStats.survivability, redStats.survivability );
  draw_text( textbuf, 0.400f, 0.650f );
}

void
Menu::screen_stats_total_page1()
{
  setRenderColor(constants::colors::background);
  SDL_RenderClear(gRenderer);

  const auto& stats = gameState().stats.total;

  char textbuf[40];

  draw_text(        "          Total stats:          ", 0.f, 0.100f );

  sprintf( textbuf, "  %d shots", stats.shots );
  draw_text( textbuf, 0.025f, 0.200f );

  sprintf( textbuf, "  %d plane hits", stats.plane_hits );
  draw_text( textbuf, 0.025f, 0.250f );

  sprintf( textbuf, "  %d chute hits", stats.chute_hits );
  draw_text( textbuf, 0.025f, 0.300f );

  sprintf( textbuf, "  %d pilot hits", stats.pilot_hits );
  draw_text( textbuf, 0.025f, 0.350f );

  sprintf( textbuf, "  %d misses", stats.misses );
  draw_text( textbuf, 0.025f, 0.400f );

  sprintf( textbuf, "Accuracy: %.2f%%", stats.accuracy );
  draw_text( textbuf, 0, 0.450f );

  sprintf( textbuf, "Shots per kill: %.2f", stats.shotsPerKill );
  draw_text( textbuf, 0, 0.500f );


  sprintf( textbuf, "  %d jumps", stats.jumps );
  draw_text( textbuf, 0.025f, 0.650f );

  sprintf( textbuf, "  %d crashes", stats.crashes );
  draw_text( textbuf, 0.025f, 0.700f );

  sprintf( textbuf, "  %d fall deaths", stats.falls );
  draw_text( textbuf, 0.025f, 0.750f );

  sprintf( textbuf, "  %d barn respawns", stats.rescues );
  draw_text( textbuf, 0.025f, 0.800f );

  sprintf( textbuf, "Self-preservation: %.2f%%", stats.selfPreservation );
  draw_text( textbuf, 0, 0.850f );
}

void
Menu::screen_stats_total_page2()
{
  setRenderColor(constants::colors::background);
  SDL_RenderClear(gRenderer);

  const auto& stats = gameState().stats.total;


  const float winLose =
    std::isnan( (float) stats.wins / stats.losses )
    ? 0.0f
    : stats.wins * 100.0f / stats.losses;

  char textbuf[40];

  draw_text(        "          Total stats:          ", 0.f, 0.100f );

  sprintf( textbuf, "  %d plane kills", stats.plane_kills );
  draw_text( textbuf, 0.025f, 0.200f );

  sprintf( textbuf, "  %d pilot kills", stats.pilot_hits );
  draw_text( textbuf, 0.025f, 0.250f );

  sprintf( textbuf, "  %d plane deaths", stats.plane_deaths );
  draw_text( textbuf, 0.025f, 0.300f );

  sprintf( textbuf, "  %d pilot deaths", stats.pilot_deaths );
  draw_text( textbuf, 0.025f, 0.350f );


  sprintf( textbuf, "  %d total kills", stats.totalKills );
  draw_text( textbuf, 0.025f, 0.450f );

  sprintf( textbuf, "  %d total deaths", stats.totalDeaths );
  draw_text( textbuf, 0.025f, 0.500f );

  sprintf( textbuf, "K-D ratio: %.2f", stats.kdRatio );
  draw_text( textbuf, 0, 0.550f );

  sprintf( textbuf, "Survivability: %.2f%%", stats.survivability );
  draw_text( textbuf, 0, 0.600f );

  sprintf( textbuf, "  %d wins", stats.wins );
  draw_text( textbuf, 0.025f, 0.700f );

  sprintf( textbuf, "  %d losses", stats.losses );
  draw_text( textbuf, 0.025f, 0.750f );

  sprintf( textbuf, "Win-Lose ratio: %.2f%%", winLose );
  draw_text( textbuf, 0, 0.800f );

  sprintf( textbuf, "  %d Developer victories", stats.wins_vs_developer );
  draw_text( textbuf, 0.025f, 0.850f );

  sprintf( textbuf, "  %d Insane victories", stats.wins_vs_insane );
  draw_text( textbuf, 0.025f, 0.900f );
}


void
calcDerivedStats(
  Statistics& stats )
{
  stats.totalHits =
    stats.plane_hits + stats.chute_hits + stats.pilot_hits;

  stats.misses =
    stats.shots - stats.totalHits;


  stats.suicides =
    stats.crashes + stats.falls;


  stats.totalKills =
    stats.plane_kills + stats.pilot_hits;

  stats.totalDeaths =
    stats.plane_deaths + stats.pilot_deaths + stats.suicides;


//  ACCURACY
  if ( stats.shots == 0 )
    stats.accuracy = 0.f;
  else
    stats.accuracy = stats.totalHits * 100.f / stats.shots;


//  SELF-PRESERVATION
  if ( stats.suicides + stats.rescues == 0 )
    stats.selfPreservation =
      100.f * (stats.totalKills + stats.totalDeaths > 0);

  else
    stats.selfPreservation =
      stats.rescues * 100.f / (stats.suicides + stats.rescues);


//  KD RATIO
  if ( stats.totalKills == 0 )
    stats.kdRatio = 0.f;

  else if ( stats.totalDeaths == 0 )
    stats.kdRatio = std::numeric_limits <float>::infinity();

  else
    stats.kdRatio = 1.f * stats.totalKills / stats.totalDeaths;


//  SURVIVABILITY
  if ( stats.totalKills + stats.totalDeaths == 0 )
    stats.survivability = 0.f;

  else if ( stats.totalKills > 0 &&
            stats.totalDeaths + stats.rescues == 0 )
    stats.survivability = 100.f;

  else
    stats.survivability =
      stats.rescues * 100.f / (stats.totalDeaths + stats.rescues);


//  SHOTS PER KILL
  if ( stats.totalKills == 0 )
    stats.shotsPerKill = 0.f;

  else
    stats.shotsPerKill =
      1.f * stats.shots / stats.totalKills;
}

void
updateRecentStats()
{
  for ( const auto& [planeType, plane] : planes )
    gameState().stats.recent[planeType] = plane.stats();
}

void
resetRecentStats()
{
  for ( auto& [planeType, stats] : gameState().stats.recent )
    stats = {};
}

void
updateTotalStats()
{
  auto& game = gameState();

  const auto& planeRed = planes.at(PLANE_TYPE::RED);
  const auto& planeBlue = planes.at(PLANE_TYPE::BLUE);

  const auto& playerPlane =
    planeRed.isBot() == false && planeRed.isLocal() == true
    ? planeRed : planeBlue;

  const auto& playerStats = game.stats.recent[playerPlane.type()];
  auto& totalStats = game.stats.total;

  totalStats.chute_hits   += playerStats.chute_hits;
  totalStats.crashes      += playerStats.crashes;
  totalStats.plane_deaths += playerStats.plane_deaths;
  totalStats.pilot_deaths += playerStats.pilot_deaths;
  totalStats.falls        += playerStats.falls;
  totalStats.jumps        += playerStats.jumps;
  totalStats.losses       += playerStats.losses;
  totalStats.pilot_hits   += playerStats.pilot_hits;
  totalStats.plane_hits   += playerStats.plane_hits;
  totalStats.plane_kills  += playerStats.plane_kills;
  totalStats.rescues      += playerStats.rescues;
  totalStats.shots        += playerStats.shots;
  totalStats.wins         += playerStats.wins;
}
