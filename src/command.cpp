/*
 * Kaolin - Exfoliate your face
 * Copyright (c) 2008 Devin Torres
 * Licensed under the MIT license.
 */

#include <iomanip>
#include <string>
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



namespace command
{

/*
 * Player command function declarations.
 */
CMD(cmd_commands)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: commands\r\n");
    return true;
  }

  std::ostringstream buf;

  buf << "Availiable Commands\r\n"
      << "-------------------\r\n";

  unsigned short int col = 0;

  for (std::map<std::string, fun_ptr>::iterator it = s->command_map.begin();
      it != s->command_map.end(); ++it)
  {
    ++col;

    if (col == 4)
    {
      col = 0;
      buf << (*it).first << "\r\n";
    }
    else
      buf << (*it).first << std::setw(20 - (*it).first.length() + 1) << '\0';
  }

  buf << "\r\n";

  p->send_to(buf);

  return true;
}



CMD(cmd_clear)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: clear\r\n");
    return true;
  }

  std::ostringstream buf;

  for (unsigned short int i = 0; i < p->get_screen_size() - 2; ++i)
    buf << "\r\n";

  p->send_to(buf);

  return true;
}



CMD(cmd_copyover)
{
  if (!p->get_immortal())
    return false;

  if (!arg.empty())
  {
    p->send_to("Syntax: copyover\r\n");
    return true;
  }

  for (std::list<player*>::iterator it = s->player_list.begin();
       it != s->player_list.end(); ++it)
  {
    (*it)->send_to("Please wait.  Copyover in progress...\r\n");
    (*it)->set_state(STATE_CONNECTING);
  }

  s->copyover = true;

  return true;
}



CMD(cmd_credits)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: credits\r\n");
    return true;
  }

  cmd_help(p, "credits");

  return true;
}



CMD(cmd_delete)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: delete\r\n");
    return true;
  }

  p->send_to(
    "Are you absolutely sure you wish to delete yourself? ");
  p->set_state(STATE_CONFIRMING_DELETE);

  return true;
}



CMD(cmd_down)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: down\r\n");
    return true;
  }

  for (std::list<room*>::iterator it = s->room_list.begin();
       it != s->room_list.end(); ++it)
  {
    if (p->get_current_room() == (*it)->get_id() && (*it)->exit_down())
    {
      p->send_to("You descend...\r\n\r\n");
      p->send_to_room(p->get_name() + " descends from above.\r\n");
      p->set_current_room((*it)->get_down());
      p->send_to_room(p->get_name() + " ascends from below.\r\n");
      cmd_look(p, "");

      return true;
    }
  }

  p->send_to("There is no exit below you.\r\n");

  return true;
}



CMD(cmd_east)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: east\r\n");
    return true;
  }

  for (std::list<room*>::iterator it = s->room_list.begin();
       it != s->room_list.end(); ++it)
  {
    if (p->get_current_room() == (*it)->get_id() && (*it)->exit_east())
    {
      p->send_to("You walk east...\r\n");
      p->send_to_room(p->get_name() + " walks east.\r\n");
      p->set_current_room((*it)->get_east());
      p->send_to_room(p->get_name() + " walks in from the west.\r\n");
      cmd_look(p, "");

      return true;
    }
  }

  p->send_to("There is no exit to the east.\r\n");

  return true;
}



CMD(cmd_echo)
{
  if (!p->get_immortal())
    return false;

  if (arg.empty())
  {
    p->send_to("Syntax: echo [msg]\r\n");
    return true;
  }

  std::ostringstream buf;

  buf << arg << "\033[0m" << "\r\n";

  s->send_to_all(buf);

  return true;
}



CMD(cmd_emote)
{
  if (arg.empty())
  {
    p->send_to("Syntax: emote [message]\r\n");
    return true;
  }

  if (arg.length() > 1000)
  {
    p->send_to(
      "Please keep your message less than a thousand characters.\r\n");
    return true;
  }

  for (std::list<player*>::iterator it = s->player_list.begin();
       it != s->player_list.end(); ++it)
  {
    if (p == *it)
      p->send_to("[You] " + arg + "\033[0m\r\n");
    else
      (*it)->send_to(p->get_name() + ' ' + arg + "\033[0m\r\n");
  }

  return true;
}



CMD(cmd_goto)
{
  if (!p->get_immortal())
    return false;

  if (arg.empty())
  {
    p->send_to("Syntax: goto [room #]\r\n");
    return true;
  }

  unsigned long destination = strtoul(arg.c_str(), NULL, 0);

  if (p->get_current_room() == destination)
  {
    p->send_to("You're already at room " + arg + ".\r\n");
    return true;
  }

  for (std::list<room*>::iterator it = s->room_list.begin();
       it != s->room_list.end(); ++it)
  {
    if (destination == (*it)->get_id())
    {
      p->set_current_room((*it)->get_id());
      p->send_to_room(p->get_name() + " suddenly appears out of thin air!\r\n");
      cmd_look(p, "");
      return true;
    }
  }

  p->send_to("Room " + arg + " doesn't exist.\r\n");

  return true;
}



CMD(cmd_immortalize)
{
  if (!p->get_immortal())
    return false;

  if (arg.empty())
  {
    p->send_to("Syntax: immortalize [name]\r\n");
    return true;
  }

  for (std::list<player*>::iterator it = s->player_list.begin();
       it != s->player_list.end(); ++it)
  {
    if ((*it)->get_name() == arg)
    {
      if (p == *it)
      {
        p->send_to("You\'re already immortal.\r\n");
        return true;
      }
      else if ((*it)->get_immortal())
      {
        p->send_to("That player is already immortal.\r\n");
        return true;
      }

      (*it)->set_immortal(true);
      (*it)->send_to("You've become immortal!\r\n");
      (*it)->save_player();
      (*it)->send_to_all_except(
        "The ground shakes and the skies thunder!  " + (*it)->get_name()
          + " has become immortal.\r\n");

      return true;
    }
  }

  p->send_to("That player is currently not playing.\r\n");

  return true;
}



CMD(cmd_help)
{
  if (arg.empty())
  {
    std::ostringstream buf;

    buf << "Kaolin version " << CEMENT_VERSION << "\r\n"
        << "\r\n"
        << "Type \"commands\" for a list of availiable commands.\r\n";

    p->send_to(buf);
    return true;
  }

  std::vector<std::string> keywords;

  for (std::list<help*>::iterator it = s->help_list.begin();
       it != s->help_list.end(); ++it)
  {
    for (unsigned short int i = 0; i < (*it)->get_keywords().size(); ++i)
    {
      if (util::str_prefix(arg, (*it)->get_keywords()[i]))
      {
        p->send_to((*it)->get_description());
        return true;
      }
    }
  }

  p->send_to("No help entry found for \"" + arg + "\".\r\n");

  return true;
}



CMD(cmd_look)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: look\r\n");
    return true;
  }

  std::ostringstream buf;

  for (std::list<room*>::iterator it = s->room_list.begin();
       it != s->room_list.end(); ++it)
  {
    if (p->get_current_room() == (*it)->get_id())
    {
      bool exit = false;

      buf << "{y" << util::center_str((*it)->get_name()) << "{x" <<
        (*it)->get_description() << "\r\nExits: ";

      if ((*it)->exit_north())
      {
        buf << "north ";
        exit = true;
      }

      if ((*it)->exit_northeast())
      {
        buf << "northeast ";
        exit = true;
      }

      if ((*it)->exit_northwest())
      {
        buf << "northwest ";
        exit = true;
      }

      if ((*it)->exit_south())
      {
        buf << "south ";
        exit = true;
      }

      if ((*it)->exit_southeast())
      {
        buf << "southeast ";
        exit = true;
      }

      if ((*it)->exit_southwest())
      {
        buf << "southwest ";
        exit = true;
      }

      if ((*it)->exit_east())
      {
        buf << "east ";
        exit = true;
      }

      if ((*it)->exit_west())
      {
        buf << "west ";
        exit = true;
      }

      if ((*it)->exit_up())
      {
        buf << "up ";
        exit = true;
      }

      if ((*it)->exit_down())
      {
        buf << "down ";
        exit = true;
      }

      if (!exit)
        buf << "none";

      buf << "\r\n";
    }
  }

  for (std::list<player*>::iterator it = s->player_list.begin();
       it != s->player_list.end(); ++it)
  {
    if (p->get_current_room() == (*it)->get_current_room()
      && p != *it && (*it)->get_state() == STATE_PLAYING)
        buf << (*it)->get_name() << " is here.\r\n";
  }

  p->send_to(buf);

  return true;
}



CMD(cmd_north)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: north\r\n");
    return true;
  }

  for (std::list<room*>::iterator it = s->room_list.begin();
       it != s->room_list.end(); ++it)
  {
    if (p->get_current_room() == (*it)->get_id()
        && (*it)->exit_north())
    {
      p->send_to("You walk north...\r\n\r\n");
      p->send_to_room(p->get_name() + " walks north.\r\n");
      p->set_current_room((*it)->get_north());
      p->send_to_room(p->get_name() + " walks in from the south.\r\n");
      cmd_look(p, "");

      return true;
    }
  }

  p->send_to("There is no exit to the north.\r\n");

  return true;
}



CMD(cmd_northeast)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: northeast\r\n");
    return true;
  }

  for (std::list<room*>::iterator it = s->room_list.begin();
       it != s->room_list.end(); ++it)
  {
    if (p->get_current_room() == (*it)->get_id() && (*it)->exit_northeast())
    {
      p->send_to("You walk northeast...\r\n\r\n");
      p->send_to_room(p->get_name() + " walks northeast.\r\n");
      p->set_current_room((*it)->get_northeast());
      p->send_to_room(p->get_name() + " walks in from the southwest.\r\n");
      cmd_look(p, "");

      return true;
    }
  }

  p->send_to("There is no exit to the northeast.\r\n");

  return true;
}



CMD(cmd_northwest)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: northwest\r\n");
    return true;
  }

  for (std::list<room*>::iterator it = s->room_list.begin();
       it != s->room_list.end(); ++it)
  {
    if (p->get_current_room() == (*it)->get_id() && (*it)->exit_northwest())
    {
      p->send_to("You walk northwest...\r\n\r\n");
      p->send_to_room(p->get_name() + " walks northwest.\r\n");
      p->set_current_room((*it)->get_northwest());
      p->send_to_room(p->get_name() + " walks in from the southeast.\r\n");
      cmd_look(p, "");

      return true;
    }
  }

  p->send_to("There is no exit to the northwest.\r\n");

  return true;
}



CMD(cmd_password)
{
  std::istringstream arg_line(arg);
  std::vector<std::string> arg_elements;
  std::string old_password;
  std::string new_password;

  while (arg_line >> old_password >> new_password)
  {
    arg_elements.push_back(old_password);
    arg_elements.push_back(new_password);
  }

  if (arg.empty() || arg_elements.size() != 2)
  {
    p->send_to("Syntax: password [old password] [new password]\r\n");
    return true;
  }

  if (old_password != p->get_password())
  {
    p->send_to("Incorrect password!\r\nTry again.\r\n");
    return true;
  }

  if (old_password == p->get_password())
  {
    p->send_to("That\'s your password already.\r\n");
    return true;
  }

  p->set_password(new_password);
  p->send_to("New password set.\r\n");
  cmd_save(p, "");

  return true;
}



CMD(cmd_repeat)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: repeat\r\n");
    return true;
  }

  p->send_to(p->get_last_outbuf());

  return true;
}



CMD(cmd_save)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: save\r\n");
    return true;
  }

  p->save_player();

  p->send_to("Your soul has been redeemed.\r\n");

  return true;
}



CMD(cmd_say)
{
  if (arg.empty())
  {
    p->send_to("Syntax: say [message]\r\n");
    return true;
  }

  if (arg.length() > 1000)
  {
    p->send_to(
      "Please keep your message less than a thousand characters.\r\n");
    return true;
  }

  for (std::list<player*>::iterator it = s->player_list.begin();
       it != s->player_list.end(); ++it)
  {
    if (p == *it)
      p->send_to("You say, \"" + arg + "{x\"\r\n");
    else if (p->get_current_room() == (*it)->get_current_room())
      (*it)->send_to(p->get_name() + " says, \"" + arg + "{x\"\r\n");
  }

  return true;
}



CMD(cmd_screensize)
{
  std::string new_password;

  if (!arg.empty())
  {
    p->send_to("Syntax: screensize");
    return true;
  }

  p->set_state(STATE_SHOWING_LINES2);
  p->handle_state();

  return true;
}



CMD(cmd_set)
{
  if (!p->get_immortal())
    return false;

  std::istringstream arg_line(arg);
  std::vector<std::string> arg_elements;
  std::string name = "";
  std::string sex  = "";

  while (arg_line >> name >> sex)
  {
    arg_elements.push_back(name);
    arg_elements.push_back(sex);
  }

  if (arg.empty() || arg_elements.size() != 2)
  {
    p->send_to("Syntax: set [name] [male | female]\r\n");
    return true;
  }

  for (std::list<player*>::iterator it = s->player_list.begin();
       it != s->player_list.end(); ++it)
  {
    if (util::str_prefix(name, (*it)->get_name()))
    {
      if (*it != p && (*it)->get_immortal())
      {
        p->send_to(
          "That player is immortal; they can change their own sex.\r\n");
        return true;
      }

      if ((*it)->get_sex() == SEX_MALE
          && util::str_prefix(sex, "male")
          || (*it)->get_sex() == SEX_FEMALE
          && util::str_prefix(sex, "female"))
      {
        if (p == *it)
          p->send_to("You\'re already that sex.\r\n");
        else
          p->send_to("That player is already that sex.\r\n");

        return true;
      }

      std::ostringstream buf;

      if (util::str_prefix(sex, "male"))
      {
        (*it)->set_sex(SEX_MALE);

        if (p == *it)
          buf << "You have changed yourself into a male.\r\n";
        else
          buf << (*it)->get_name() << " has been changed into a male.\r\n";
      }
      else if (util::str_prefix(sex, "female"))
      {
        (*it)->set_sex(SEX_FEMALE);

        if (p == *it)
          buf << "You have changed yourself into a female.\r\n";
        else
          buf << (*it)->get_name() << " has been changed into a female.\r\n";
      }
      else
      {
        p->send_to("That\'s not a valid sex.\r\n");
        break;
      }

      if (p == *it)
        p->send_to(buf);
      else
        (*it)->send_to("You feel changed in some way...\r\n" + buf.str());

      (*it)->save_player();

      return true;
    }
  }

  p->send_to("That player is currently not playing.\r\n");

  return true;
}



CMD(cmd_shutdown)
{
  if (!p->get_immortal())
    return false;

  if (!arg.empty())
  {
    p->send_to("Syntax: shutdown\r\n");
    return true;
  }

  s->log("Shutting down...\n");
  s->send_to_all("Shutdown by " + p->get_name() + ".\r\n");

  s->shutdown = true;

  return true;
}



CMD(cmd_south)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: south\r\n");
    return true;
  }

  for (std::list<room*>::iterator it = s->room_list.begin();
       it != s->room_list.end(); ++it)
  {
    if (p->get_current_room() == (*it)->get_id() && (*it)->exit_south())
    {
      p->send_to("You walk south...\r\n\r\n");
      p->send_to_room(p->get_name() + " walks south.\r\n");
      p->set_current_room((*it)->get_south());
      p->send_to_room(p->get_name() + " walks in from the north.\r\n");
      cmd_look(p, "");

      return true;
    }
  }

  p->send_to("There is no exit to the south.\r\n");

  return true;
}



CMD(cmd_southeast)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: southeast\r\n");
    return true;
  }

  for (std::list<room*>::iterator it = s->room_list.begin();
       it != s->room_list.end(); ++it)
  {
    if (p->get_current_room() == (*it)->get_id() && (*it)->exit_southeast())
    {
      p->send_to("You walk southeast...\r\n\r\n");
      p->send_to_room(p->get_name() + " walks southeast.\r\n");
      p->set_current_room((*it)->get_southeast());
      p->send_to_room(p->get_name() + " walks in from the northwest.\r\n");
      cmd_look(p, "");

      return true;
    }
  }

  p->send_to("There is no exit to the southeast.\r\n");

  return true;
}



CMD(cmd_southwest)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: southwest\r\n");
    return true;
  }

  for (std::list<room*>::iterator it = s->room_list.begin();
       it != s->room_list.end(); ++it)
  {
    if (p->get_current_room() == (*it)->get_id() && (*it)->exit_southwest())
    {
      p->send_to("You walk southwest...\r\n\r\n");
      p->send_to_room(p->get_name() + " walks southwest.\r\n");
      p->set_current_room((*it)->get_southwest());
      p->send_to_room(p->get_name() + " walks in from the northeast.\r\n");
      cmd_look(p, "");

      return true;
    }
  }

  p->send_to("There is no exit to the southwest.\r\n");

  return true;
}



CMD(cmd_qui)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: quit\r\n");
    return true;
  }

  p->send_to("Type \"quit\" to quit.\r\n");

  return true;
}



CMD(cmd_quit)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: quit\r\n");
    return true;
  }

  send(p->get_desc(), "Bye!\r\n", 6, 0);
  p->send_to_all_except(
    p->get_name() + " has quit the game.  His body slowly vanishes...\r\n");
  p->set_state(STATE_EXITTING);

  return true;
}



CMD(cmd_up)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: up\r\n");
    return true;
  }

  for (std::list<room*>::iterator it = s->room_list.begin();
       it != s->room_list.end(); ++it)
  {
    if (p->get_current_room() == (*it)->get_id()
        && (*it)->exit_up())
    {
      p->send_to("You ascend...\r\n\r\n");
      p->send_to_room(p->get_name() + " ascends from below.\r\n");
      p->set_current_room((*it)->get_up());
      p->send_to_room(p->get_name() + " descends from above.\r\n");
      cmd_look(p, "");

      return true;
    }
  }

  p->send_to("There is no exit above you.\r\n");

  return true;
}



CMD(cmd_west)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: west\r\n");
    return true;
  }

  for (std::list<room*>::iterator it = s->room_list.begin();
       it != s->room_list.end(); ++it)
  {
    if (p->get_current_room() == (*it)->get_id() && (*it)->exit_west())
    {
      p->send_to("You walk west...\r\n\r\n");
      p->send_to_room(p->get_name() + " walks west.\r\n");
      p->set_current_room((*it)->get_west());
      p->send_to_room(p->get_name() + " walks in from the east.\r\n");
      cmd_look(p, "");

      return true;
    }
  }

  p->send_to("There is no exit to the west.\r\n");

  return true;
}



CMD(cmd_who)
{
  if (!arg.empty())
  {
    p->send_to("Syntax: who\r\n");
    return true;
  }

  std::ostringstream buf;

  buf << "Players\r\n"
      << "-------\r\n";

  unsigned short int count = 0;

  for (std::list<player*>::iterator it = s->player_list.begin();
       it != s->player_list.end(); ++it)
  {
    if (!(*it)->get_immortal() && (*it)->get_state() == STATE_PLAYING)
    {
      buf << (*it)->get_name() << "\r\n";
      ++count;
    }
  }

  switch (count)
  {
  case 0:
    buf << "None\r\n"
        << "\r\n"
        << "There are currently no players on.\r\n";
    break;
  case 1:
    buf << "\r\n"
        << "There is currently one player on.\r\n";
    break;
  default:
    buf << "\r\n"
        << "There are currently " << util::numberize(count) << " players on.\r\n";
    break;
  }

  buf << "\r\n"
      << "\r\n"
      << "Immortals\r\n"
      << "---------\r\n";

  count = 0;

  for (std::list<player*>::iterator it = s->player_list.begin();
      it != s->player_list.end(); ++it)
  {
    if ((*it)->get_immortal() && (*it)->get_state() == STATE_PLAYING)
    {
      buf << (*it)->get_name() << "\r\n";
      ++count;
    }
  }

  switch (count)
  {
  case 0:
    buf << "None\r\n"
        << "\r\n"
        << "There are currently no immortals on.\r\nz";
    break;
  case 1:
    buf << "\r\n"
        << "There is currently one immortal on.\r\n";
    break;
  default:
    buf << "\r\n"
        << "There are currently " << util::numberize(count) << " immortals on.\r\n";
    break;
  }

  p->send_to(buf);

  return true;
}

} // command namespace
