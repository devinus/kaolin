/*
 * Kaolin - Exfoliate your face
 * Copyright (c) 2008 Devin Torres
 * Licensed under the MIT license.
 */

#include <sys/time.h>
#include <fcntl.h>
#include <cstdarg>
#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <map>
#include "config.h"
#include "main.h"
#include "server.h"
#include "player.h"
#include "command.h"
#include "room.h"
#include "help.h"
#include "util.h"



#if !defined(FNDELAY)
  #define FNDELAY O_NDELAY
#endif




/*
 * Mimic usleep() through select().
 *
 * Uncomment if you get errors about usleep().
 */
#if 0
namespace
{
  void usleep(const int useconds)
  {
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = useconds;
    select(1, 0, 0, 0, &tv);
  }
}
#endif



server::server()
  : host(0), shutdown(false), copyover(false)
{
#ifdef DEBUG
  s->log("server constructor...\n");
#endif
}



server::~server()
{
#ifdef DEBUG
  s->log("server destructor...\n");
#endif

  for (std::list<player*>::iterator it = player_list.begin();
       it != player_list.end(); ++it)
  {
    if (FD_ISSET((*it)->get_desc(), &fd_write))
    {
      if (!(*it)->process_output())
      {
        delete *it;
        --it;
        continue;
      }
    }
  }

  while (!room_list.empty())
    delete *room_list.begin();

  while (!help_list.empty())
    delete *help_list.begin();

  while (!player_list.empty())
    delete *player_list.begin();

  s->log("Shutdown successfull.\n");
}



void server::log(const char * const msg, ...)
{
  char buf[MAX_BUFFER] = {'\0'};
  char filename_str[12] = {'\0'};
  char time_str[12] = {'\0'};
  va_list arglist;

  va_start(arglist, msg);
  vsprintf(buf, msg, arglist);
  va_end(arglist);

  time_t dummy_time = static_cast<time_t>(s->current_time.tv_sec);
  strftime(filename_str, 12, "%b-%d-%Y", localtime(&dummy_time));
  strftime(time_str, 12, "%I:%M:%S %p", localtime(&dummy_time));

  flog.open((LOG_DIR + filename_str + ".log").c_str(),
    std::ios::out | std::ios::app);
  flog << '[' << time_str << "] " << buf;
  flog.close();
}



void server::error(const char * const msg, ...)
{
  char buf[MAX_BUFFER] = {'\0'};
  char filename_str[12] = {'\0'};
  char time_str[12] = {'\0'};
  va_list arglist;

  va_start(arglist, msg);
  vsprintf(buf, msg, arglist);
  va_end(arglist);

  time_t dummy_time = static_cast<time_t>(s->current_time.tv_sec);
  strftime(filename_str, sizeof(filename_str), "%b-%d-%Y", localtime(&dummy_time));
  strftime(time_str, sizeof(time_str), "%I:%M:%S %p", localtime(&dummy_time));

  ferr.open((LOG_DIR + filename_str + ".err").c_str(),
    std::ios::out | std::ios::app);
  ferr << '[' << time_str << "] " << buf;
  ferr.close();
}



void server::game_loop(sockaddr_in& addr)
{
  socklen_t high = host;
  unsigned short int sleep_time = 0;
  timeval last_time;

  gettimeofday(&last_time, NULL);

  do
  {
    gettimeofday(&current_time, NULL);

    sleep_time = 250000 - (current_time.tv_usec - last_time.tv_usec);
    last_time = current_time;

    usleep(sleep_time);

    FD_ZERO(&fd_read);
    FD_ZERO(&fd_write);
    FD_SET(host, &fd_read);

    for (std::list<player*>::const_iterator it = player_list.begin();
         it != player_list.end(); ++it)
    {
      if ((*it)->get_desc() > high)
        high = (*it)->get_desc();

      FD_SET((*it)->get_desc(), &fd_read);
      FD_SET((*it)->get_desc(), &fd_write);
    }

    if (select(high + 1, &fd_read, &fd_write, NULL, 0) < 0)
    {
      s->error("%s: %d: %s: select\n", __FILE__, __LINE__, __func__);
      break;
    }

    if (FD_ISSET(host, &fd_read))
      create_player(addr);

    for (std::list<player*>::iterator it = player_list.begin();
         it != player_list.end(); ++it)
    {
      if ((*it)->get_state() == STATE_EXITTING)
      {
        delete *it;
        --it;
        continue;
      }

      if (FD_ISSET((*it)->get_desc(), &fd_write))
      {
        if (!(*it)->process_output())
        {
          delete *it;
          --it;
          continue;
        }
      }

      if (FD_ISSET((*it)->get_desc(), &fd_read))
      {
        if (!(*it)->process_input())
        {
          delete *it;
          --it;
          continue;
        }
      }
    }

    if (copyover)
    {
      for (std::list<player*>::iterator it = player_list.begin();
           it != player_list.end(); ++it)
      {
        if (FD_ISSET((*it)->get_desc(), &fd_write))
        {
          if (!(*it)->process_output())
          {
            delete *it;
            --it;
            continue;
          }
        }
      }

      while (!room_list.empty())
        delete *room_list.begin();

      while (!help_list.empty())
        delete *help_list.begin();

      command_map.clear();
      load_world();

      copyover = false;

      for (std::list<player*>::iterator it = player_list.begin();
           it != player_list.end(); ++it)
      {
        (*it)->send_to("Copyover complete.\r\n\r\n");
        (*it)->set_state(STATE_PLAYING);
        command::cmd_look(*it, "");
      }
    }
  } while (!shutdown);
}



void server::load_world()
{
  load_commands();
  load_helps();
  load_zones();
}



void server::load_commands()
{
  s->log("Loading command map...\n");

  /*
   * Common commands.
   */
  command_map["repeat"]      = &command::cmd_repeat;

  /*
   * Movement commands.
   */
  command_map["n"]           = &command::cmd_north;
  command_map["north"]       = &command::cmd_north;
  command_map["ne"]          = &command::cmd_northeast;
  command_map["northeast"]   = &command::cmd_northeast;
  command_map["nw"]          = &command::cmd_northwest;
  command_map["northwest"]   = &command::cmd_northwest;
  command_map["s"]           = &command::cmd_south;
  command_map["south"]       = &command::cmd_south;
  command_map["se"]          = &command::cmd_southeast;
  command_map["southeast"]   = &command::cmd_southeast;
  command_map["sw"]          = &command::cmd_southwest;
  command_map["southwest"]   = &command::cmd_southwest;
  command_map["e"]           = &command::cmd_east;
  command_map["east"]        = &command::cmd_east;
  command_map["w"]           = &command::cmd_west;
  command_map["west"]        = &command::cmd_west;
  command_map["u"]           = &command::cmd_up;
  command_map["up"]          = &command::cmd_up;
  command_map["d"]           = &command::cmd_down;
  command_map["down"]        = &command::cmd_down;

  /*
   * Communication commands.
   */
  command_map[","]           = &command::cmd_emote;
  command_map["emote"]       = &command::cmd_emote;
  command_map["\'"]          = &command::cmd_say;
  command_map["say"]         = &command::cmd_say;

  /*
   * Informational commands.
   */
  command_map["commands"]    = &command::cmd_commands;
  command_map["credits"]     = &command::cmd_credits;
  command_map["help"]        = &command::cmd_help;
  command_map["look"]        = &command::cmd_look;
  command_map["who"]         = &command::cmd_who;

  /*
   * Miscellaneous commands.
   */
  command_map["clear"]       = &command::cmd_clear;
  command_map["delete"]      = &command::cmd_delete;
  command_map["save"]        = &command::cmd_save;
  command_map["qui"]         = &command::cmd_qui;
  command_map["quit"]        = &command::cmd_quit;

  /*
   * Configuration commands.
   */
  command_map["password"]    = &command::cmd_password;
  command_map["screensize"]  = &command::cmd_screensize;

  /*
   * Immortal commands.
   */
  command_map["copyover"]    = &command::cmd_copyover;
  command_map["."]           = &command::cmd_echo;
  command_map["echo"]        = &command::cmd_echo;
  command_map["goto"]        = &command::cmd_goto;
  command_map["immortalize"] = &command::cmd_immortalize;
  command_map["set"]         = &command::cmd_set;
  command_map["shutdown"]    = &command::cmd_shutdown;
}



void server::load_helps()
{
  s->log("Loading help files...\n");

  std::ifstream index((HELP_DIR + "index").c_str(), std::ios::in);
  std::list<std::string> help_entries;

  try
  {
    if (!index.is_open())
      throw HELP_DIR + "index: Help index not found";

    std::string help_entry = "";

    while (!index.eof())
    {
      index >> help_entry;

      if (*help_entry.begin() == '#')
        break;

      help_entries.push_back(help_entry);
    }

    index.close();

    std::string keywords_key = "";
    std::string keyword      = "";
    std::string description  = "";

    for (std::list<std::string>::const_iterator it = help_entries.begin();
         it != help_entries.end(); ++it)
    {
      s->log("Loading %s...\n", (*it).c_str());

      std::ifstream file((HELP_DIR + *it).c_str(), std::ios::in);

      if (!file.is_open())
        throw HELP_DIR + *it + ": File not found";

      while (file >> keywords_key)
      {
        if (keywords_key != "$keywords")
          throw HELP_DIR + *it + ": Keywords section corrupted";

        help* h = new help;

        while (file >> keyword)
        {
          if (keyword == "$description")
            break;

          h->add_keyword(keyword);
        }

        if (file.eof())
          throw HELP_DIR + *it + ": EOF before description";

        while (getline(file, description))
        {
          if (*description.begin() == '#')
            break;

          description += "\r\n";

          h->append_description(description);
        }

        help_list.push_back(h);

        if (file.eof())
        {
          file.close();
          break;
        }
      }
    }
  }
  catch (const std::string& str)
  {
    s->error("%s: %d: %s: %s\n", __FILE__, __LINE__, __func__, str.c_str());
    return;
  }
}



void server::load_zones()
{
  s->log("Loading zones...\n");

  std::ifstream index((ZONE_DIR + "index").c_str(), std::ios::in);
  std::list<std::string> zone_entries;

  try
  {
    if (!index.is_open())
      throw ZONE_DIR + "index: Zone index not found";

    std::string zone_entry = "";

    while (!index.eof())
    {
      index >> zone_entry;

      if (*zone_entry.begin() == '#')
        break;

      zone_entries.push_back(zone_entry);
    }

    index.close();

    std::string   id_key          = "";
    unsigned long id              = 0;
    std::string   name_key        = "";
    std::string   name            = "";
    std::string   description_key = "";
    std::string   description     = "";
    std::string   exit_key        = "";
    std::string   exit            = "";
    unsigned long exit_to         = 0;

    for (std::list<std::string>::const_iterator it = zone_entries.begin();
         it != zone_entries.end(); ++it)
    {
      s->log("Loading %s...\n", (*it).c_str());

      std::ifstream file((ZONE_DIR + *it).c_str(), std::ios::in);

      if (!file.is_open())
        throw ZONE_DIR + *it + ": File not found";

      while (file >> id_key >> id)
      {
        if (id_key != "$id")
          throw ZONE_DIR + *it + ": ID section corrupted";

        for (std::list<room*>::iterator it2 = s->room_list.begin();
             it2 != s->room_list.end(); ++it2)
          if (id == (*it2)->get_id())
            throw ZONE_DIR + *it + ": Duplicate room ID";

        room* r = new room;

        r->set_id(id);

        if (!(file >> name_key))
          throw ZONE_DIR + *it + ": EOF before name key";

        if (name_key != "$name")
          throw ZONE_DIR + *it + ": Name section corrupted";

        if (!getline(file >> std::ws, name))
          throw ZONE_DIR + *it + ": EOF before name";

        r->set_name(name);

        if (!(file >> description_key))
          throw ZONE_DIR + *it + ": EOF before description key";

        if (description_key != "$description")
          throw ZONE_DIR + *it + ": Description section corrupted";

        while (getline(file, description))
        {
          if (*description.begin() == '~')
            break;

          description += "\r\n";

          r->append_description(description);
        }

        while (file >> exit_key)
        {
          if (*exit_key.begin() == '#')
            break;
          else if (exit_key != "$exit")
            throw ZONE_DIR + *it + ": Exit section corrupted";

          file >> exit >> exit_to;

          if (exit_to == 0)
            throw ZONE_DIR + *it + ": Invalid exit";

          if (exit == "N" && !r->exit_north())
            r->set_north(exit_to);
          else if (exit == "NE" && !r->exit_northeast())
            r->set_northeast(exit_to);
          else if (exit == "NW" && !r->exit_northwest())
            r->set_northwest(exit_to);
          else if (exit == "S" && !r->exit_south())
            r->set_south(exit_to);
          else if (exit == "SE" && !r->exit_southeast())
            r->set_southeast(exit_to);
          else if (exit == "SW" && !r->exit_southwest())
            r->set_southwest(exit_to);
          else if (exit == "E" && !r->exit_east())
            r->set_east(exit_to);
          else if (exit == "W" && !r->exit_west())
            r->set_west(exit_to);
          else if (exit == "U" && !r->exit_up())
            r->set_up(exit_to);
          else if (exit == "D" && !r->exit_down())
            r->set_down(exit_to);
          else
            throw ZONE_DIR + *it + ": Invalid exit";
        }

        room_list.push_back(r);

        if (file.eof())
        {
          file.close();
          break;
        }
      }
    }
  }
  catch (const std::string& str)
  {
    s->error("%s: %d: %s: %s\n", __FILE__, __LINE__, __func__, str.c_str());
    return;
  }
}



void server::create_player(sockaddr_in& addr)
{
  socklen_t desc = sizeof(sockaddr_in);

  try
  {
    if ((desc = accept(host, reinterpret_cast<sockaddr*>(&addr), &desc)) ==  0)
        throw "accept";

    if (fcntl(desc, F_SETFL, FNDELAY) < 0)
      throw "fcntl";
  }
  catch (const std::string& str)
  {
    s->error("%s: %d: %s: %s\n", __FILE__, __LINE__, __func__, str.c_str());
    return;
  }

  player* p = new player;

  p->set_desc(desc);
  p->lookup_address(addr);
  p->set_state(STATE_GREETING);
  player_list.push_back(p);
  p->handle_state();
}



void server::send_to_all(const std::string& msg)
{
  for (std::list<player*>::iterator it = player_list.begin();
       it != player_list.end(); ++it)
  {
    if ((*it)->get_state() == STATE_PLAYING)
      (*it)->send_to(msg);
  }
}



void server::send_to_all(const std::ostringstream& msg)
{
  for (std::list<player*>::iterator it = player_list.begin();
       it != player_list.end(); ++it)
  {
    if ((*it)->get_state() == STATE_PLAYING)
      (*it)->send_to(msg.str());
  }
}
