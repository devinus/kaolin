/*
 * Kaolin - Exfoliate your face
 * Copyright (c) 2008 Devin Torres
 * Licensed under the MIT license.
 */
 
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <list>
#include "config.h"
#include "main.h"
#include "server.h"
#include "player.h"
#include "command.h"
#include "util.h"



player::player()
  : desc(0), state(STATE_CONNECTING), immortal(false), address(""),
      name("Guest"), password(""), inbuf(""), last_inbuf(""), outbuf(""),
        last_outbuf(""), screen_size(0), sex(SEX_NONE), current_room(1)
{
#ifdef DEBUG
  s->log("player constructor...\n");
#endif
}



player::~player()
{
#ifdef DEBUG
  s->log("player destructor...\n");
#endif
  close(desc);
  FD_CLR(desc, &s->fd_read);
  FD_CLR(desc, &s->fd_write);
  s->log("%s closed connection.\n", name.c_str());
  s->player_list.remove(this);
}



void player::set_desc(socklen_t& new_desc)
{
  desc = new_desc;
}



socklen_t player::get_desc() const
{
  return desc;
}



void player::set_state(states new_state)
{
  state = new_state;
}



states player::get_state() const
{
  return state;
}



void player::set_immortal(bool new_status)
{
  immortal = new_status;
}



bool player::get_immortal() const
{
  return immortal;
}



void player::set_address(const std::string& new_address)
{
  address = new_address;
}



std::string player::get_address() const
{
  return address;
}



void player::set_name(const std::string& new_name)
{
   name = new_name;
}



std::string player::get_name() const
{
  return name;
}



void player::set_password(const std::string& new_password)
{
  password = new_password;
}



std::string player::get_password() const
{
  return password;
}



void player::append_inbuf(const std::string& extra_inbuf)
{
  inbuf += extra_inbuf;
}



void player::set_inbuf(const std::string& new_inbuf)
{
  inbuf = new_inbuf;
}



std::string player::get_inbuf() const
{
  return inbuf;
}



void player::set_last_inbuf(const std::string& new_last_inbuf)
{
  last_inbuf = new_last_inbuf;
}



std::string player::get_last_inbuf() const
{
  return last_inbuf;
}



void player::send_to(const std::string& msg)
{
  outbuf += msg;
}



void player::send_to(const std::ostringstream& msg)
{
  outbuf += msg.str();
}



void player::send_to_room(const std::string& msg)
{
  for (std::list<player*>::iterator it = s->player_list.begin();
       it != s->player_list.end(); ++it)
  {
    if (get_current_room() == (*it)->get_current_room()
        && this != *it)
      (*it)->send_to(msg);
  }
}



void player::send_to_room(const std::ostringstream& msg)
{
  for (std::list<player*>::iterator it = s->player_list.begin();
       it != s->player_list.end(); ++it)
  {
    if (get_current_room() == (*it)->get_current_room()
        && this != *it)
      (*it)->send_to(msg.str());
  }
}



void player::send_to_all_except(const std::string& msg)
{
  for (std::list<player*>::iterator it = s->player_list.begin();
       it != s->player_list.end(); ++it)
  {
    if (*it != this && (*it)->get_state() == STATE_PLAYING)
      (*it)->send_to(msg);
  }
}



void player::send_to_all_except(const std::ostringstream& msg)
{
  for (std::list<player*>::iterator it = s->player_list.begin();
       it != s->player_list.end(); ++it)
  {
    if (*it != this && (*it)->get_state() == STATE_PLAYING)
      (*it)->send_to(msg);
  }
}



void player::set_outbuf(const std::string& new_outbuf)
{
  outbuf = new_outbuf;
}



std::string player::get_outbuf() const
{
  return outbuf;
}



void player::set_last_outbuf(const std::string& new_last_outbuf)
{
  last_outbuf = new_last_outbuf;
}



std::string player::get_last_outbuf() const
{
  return last_outbuf;
}



void player::set_screen_size(const unsigned short int new_screen_size)
{
  screen_size = new_screen_size;
}



unsigned short int player::get_screen_size() const
{
  return screen_size;
}



void player::set_sex(const sexes new_sex)
{
  sex = new_sex;
}



sexes player::get_sex() const
{
  return sex;
}



void player::set_current_room(const unsigned long new_room)
{
  current_room = new_room;
}



unsigned long player::get_current_room() const
{
  return current_room;
}



void player::lookup_address(sockaddr_in& addr)
{
  std::string buf;
  socklen_t size = sizeof(sockaddr_in);

  if (getpeername(desc, reinterpret_cast<sockaddr*>(&addr),
      &size) < 0)
  {
    s->error("%s: %d: %s: getpeername\n", __FILE__, __LINE__, __func__);
    address = "unknown";
  }
  else
  {
    int ip_addr = ntohl(addr.sin_addr.s_addr);

    buf += static_cast<int>((ip_addr >> 24) & 0xFF) + '.' +
           static_cast<int>((ip_addr >> 16) & 0xFF) + '.' +
           static_cast<int>((ip_addr >> 8)  & 0xFF) + '.' +
           static_cast<int>((ip_addr)       & 0xFF);

    hostent* from = gethostbyaddr(reinterpret_cast<char*>(&addr.sin_addr),
      sizeof(addr.sin_addr), AF_INET);

    if (from)
      address = from->h_name;
    else
      address = buf;
  }

  buf = "Connection established from " + address + ".\n";
  s->log(buf.c_str());
}



bool player::process_input()
{
  char tempbuf[MAX_BUFFER] = {'\0'};

  if (recv(desc, tempbuf, MAX_BUFFER - 1, 0) <= 0)
    return false;

  bool read_line = false;

  for (unsigned short int i = 0; i < strlen(tempbuf); ++i)
  {
    if (tempbuf[i] == '\r' || tempbuf[i] == '\n')
    {
      tempbuf[i] = '\0';
      inbuf += tempbuf;
      read_line = true;
      break;
    }
  }

  if (!read_line)
  {
    inbuf += tempbuf;
    return true;
  }

  std::string new_inbuf = inbuf;

  for (std::string::iterator it = new_inbuf.begin(); it != new_inbuf.end(); ++it)
  {
    if (*it == '\b')
    {
      if (it != new_inbuf.begin())
      {
        new_inbuf.erase(it--);
        new_inbuf.erase(it--);
      }
      else
        new_inbuf.erase(it--);
    }
  }

  if (new_inbuf.empty())
  {
    outbuf += "{x"; // Trigger process_output().
    return true;
  }
  else if (new_inbuf == "!")
    inbuf = last_inbuf;
  else
    inbuf = new_inbuf;

#ifdef DEBUG
  s->log("%s entered: %s\n", name.c_str(), new_inbuf.c_str());
#endif

  handle_state();

  return true;
}



bool player::process_output()
{
  if (outbuf.empty())
    return true;

  std::string new_outbuf;

  for(std::string::iterator it = outbuf.begin(); it != outbuf.end(); ++it)
  {
    switch (*it)
    {
    case '{':
      switch (*++it)
      {
      case 'x':
        new_outbuf += "\033[0m";
        break;
      case 'u':
        new_outbuf += "\033[4m";
        break;
      case 'i':
        new_outbuf += "\033[5m";
        break;
      case 'r':
        new_outbuf += "\033[31m";
        break;
      case 'g':
        new_outbuf += "\033[32m";
        break;
      case 'y':
        new_outbuf += "\033[33m";
        break;
      case 'b':
        new_outbuf += "\033[34m";
        break;
      case 'm':
        new_outbuf += "\033[35m";
        break;
      case 'c':
        new_outbuf += "\033[36m";
        break;
      case 'w':
        new_outbuf += "\033[37m";
        break;
      case 'R':
        new_outbuf += "\033[1m\033[31m";
        break;
      case 'G':
        new_outbuf += "\033[1m\033[32m";
        break;
      case 'Y':
        new_outbuf += "\033[1m\033[33m";
        break;
      case 'B':
        new_outbuf += "\033[1m\033[34m";
        break;
      case 'M':
        new_outbuf += "\033[1m\033[35m";
        break;
      case 'C':
        new_outbuf += "\033[1m\033[36m";
        break;
      case 'W':
        new_outbuf += "\033[1m\033[37m";
        break;
      case 'n':
        new_outbuf += "\r\n";
        break;
      case '{':
        new_outbuf += '{';
        break;
      default:
        break;
      }

      break;
    default:
      new_outbuf += *it;
      break;
    }
  }

  if (state == STATE_PLAYING && !s->shutdown)
    new_outbuf += show_prompt();

  if (send(desc, new_outbuf.c_str(), new_outbuf.length(), 0) < 0)
    return false;

  last_outbuf = outbuf;
  outbuf.erase();

  return true;
}



bool player::valid_name()
{
  if (*name.begin() == '\'' || *(name.end() - 1) == '\'')
      return false;

  for (std::string::const_iterator it = name.begin(); it != name.end(); ++it)
  {
    if (isalpha(tolower(*it)) || *it == '\'')
      continue;
    else
      return false;
  }

  if (*name.begin() != toupper(*name.begin()))
  {
    std::string cap_name = name;

    cap_name[0] = toupper(*name.begin());
    name = cap_name;
  }

  return true;
}



void player::handle_state()
{
  switch (state)
  {
  case STATE_CONNECTING:
    break;
  case STATE_GREETING:
    command::cmd_help(this, "greeting");
    state = STATE_GETTING_NAME;

    break;
  case STATE_GETTING_NAME:
  {
    name = inbuf;

    if (name.length() <= 2)
    {
      send_to(
        "\r\n"
        "Please keep the length of your name greater than two characters.\r\n"
        "Your name? ");

      break;
    }

    if (name.length() > 20)
    {
      send_to(
        "\r\n"
        "Please keep the length of your name less than twenty characters.\r\n"
        "Your name? ");
    }

    if (!valid_name())
    {
      send_to(
        "\r\n"
        "Illegal name.\r\n"
        "\r\n"
        "Your name? ");

      break;
    }

    bool found = false;

    for (std::list<player*>::const_iterator it = s->player_list.begin();
         it != s->player_list.end(); ++it)
    {
      if (name == (*it)->get_name() && this != *it)
      {
        send_to(
          "That player is already playing.\r\n"
          "\r\n"
          "Your name? ");
        found = true;

        break;
      }
    }

    if (found)
      break;

    std::ifstream file((PLAYER_DIR + name).c_str(), std::ios::in);

    if (file.is_open())
    {
      send_to("\r\nPassword: ");
      state = STATE_GETTING_PASSWORD;
      break;
    }
    else
    {
      send_to(
        "\r\n"
        "Creating new player...\r\n"
        "Choose a password for your player: ");
      state = STATE_GETTING_PASSWORD;
    }

    break;
  }
  case STATE_GETTING_PASSWORD:
  {
    password = inbuf;

    if (read_player())
      break;

    state = STATE_PLAYING;

    break;
  }
  case STATE_SHOWING_LINES:
  {
    std::ostringstream buf;

    for (unsigned short int i = 100; i >= 3; --i)
      buf << i << "\r\n";

    buf << "\r\nWhat is the highest number you can see without scrolling up? ";

    send_to(buf);
    state = STATE_GETTING_SCREEN_SIZE;

    break;
  }
  case STATE_SHOWING_LINES2:
  {
    std::ostringstream buf;

    for (unsigned short int i = 100; i >= 3; --i)
      buf << i << "\r\n";

    buf << "\r\nWhat is the highest number you can see without scrolling up? ";

    send_to(buf);
    state = STATE_GETTING_SCREEN_SIZE2;

    break;
  }
  case STATE_GETTING_SCREEN_SIZE:
  {
    unsigned short int new_screen_size =
      static_cast<unsigned short int>(atoi(inbuf.c_str()));

    if (new_screen_size < 4 || new_screen_size > 100)
    {
      send_to(
        "Invalid screen size.\r\n"
        "Calculating screen size again...\r\n");
      state = STATE_SHOWING_LINES;
      handle_state();

      break;
    }

    screen_size = new_screen_size;

    std::ostringstream buf;

    buf << "Screen size set to " << screen_size << ".\r\n";

    send_to(buf);
    send_to(
      "\r\n"
      "Do you wish to be:\r\n"
      "  1) male\r\n"
      "  2) female\r\n"
      "Your choice? ");
    state = STATE_GETTING_SEX;

    break;
  }
  case STATE_GETTING_SCREEN_SIZE2:
  {
    unsigned short int new_screen_size =
      static_cast<unsigned short int>(atoi(inbuf.c_str()));

    if (new_screen_size < 4 || new_screen_size > 100)
    {
      send_to(
        "Invalid screen size.\r\n"
        "Calculating screen size again...\r\n");
      state = STATE_SHOWING_LINES2;
      handle_state();

      break;
    }

    screen_size = new_screen_size;

    std::ostringstream buf;

    buf << "Screen size reset to " << new_screen_size << ".\r\n";

    send_to(buf);
    state = STATE_PLAYING;
    command::cmd_save(this, "");

    break;
  }
  case STATE_GETTING_SEX:
    if (inbuf == "1")
    {
      send_to("\r\nVery well.  You shall be male.\r\n");
      sex = SEX_MALE;
    }
    else if (inbuf == "2")
    {
      send_to("\r\nVery well.  You shall be female.\r\n");
      sex = SEX_FEMALE;
    }
    else
    {
      send_to(
        "\r\n"
        "That was not a choice; please pick again:\r\n"
        "  1) male\r\n"
        "  2) female\r\n"
        "Your choice? ");
      break;
    }

    s->log("New player named %s.\n", name.c_str());
    send_to("\r\nWelcome " + name + "!\r\r\n\n");
    send_to_all_except("A new player named " + name +
                         " has entered the game!\r\n");
    command::cmd_look(this, "");
    state = STATE_PLAYING;

    break;
  case STATE_PLAYING:
  {
    std::istringstream cmd_line(inbuf);
    std::string cmd = "";

    cmd_line >> cmd;

    bool found = false;

    for (std::map<std::string, fun_ptr>::iterator it = s->command_map.begin();
         it != s->command_map.end(); ++it)
    {
      if (util::str_prefix(cmd, (*it).first))
      {
        if (inbuf.length() == cmd.length())
        {
          if (!(*(*it).second)(this, ""))
            break;
        }
        else
        {
          std::string arg(inbuf, cmd.length() + 1, inbuf.length());

          if (!(*(*it).second)(this, arg))
            break;
        }

        found = true;

        break;
      }
    }

    if (!found)
      send_to(
        "That\'s not a recognized command; type \"help\" for help.\r\n");

    break;
  }
  case STATE_CONFIRMING_DELETE:
    if (*inbuf.begin() == 'y'
        || *inbuf.begin() == 'Y')
    {
      for (std::list<player*>::iterator it = s->player_list.begin();
        it != s->player_list.end(); ++it)
      {
        if (this == *it)
          send(desc,
            "May the gods have mercy on your soul...\r\n", 41, 0);
        else
          (*it)->send_to(
            name + " has forever left the game...\r\n");
      }

      s->log("%s has deleted h%sself.\n", name.c_str(),
             (sex == SEX_MALE ? "im" : "er"));
      remove((PLAYER_DIR + name).c_str());
      state = STATE_EXITTING;
    }
    else if (*inbuf.begin() == 'n' || *inbuf.begin() == 'N')
    {
      send_to("Good choice.\r\n");
      state = STATE_PLAYING;
    }
    else
      send_to("That\'s not an option.  Yes or no: ");

    break;
  case STATE_EXITTING:
    break;
  default:
    break;
  }

  last_inbuf = inbuf;
  inbuf = "";
}



void player::save_player()
{
  std::ofstream file((PLAYER_DIR + name).c_str(),
                     std::ios::out | std::ios::trunc);

  if (file.is_open())
  {
    file << "$password    " << password << '\n'
         << "$immortal    " << (immortal ? "yes" : "no") << '\n'
         << "$screen_size " << screen_size << '\n'
         << "$sex         " << (sex == SEX_MALE ? "male" : "female") << '\n'
         << "$last_room   " << current_room << '\n'
         << '#' << '\n';

    file.close();
  }
  else
  {
    send_to("Sorry, but there was an error saving your character.\r\n");
    s->error("%s: %d: %s: file not opened", __FILE__, __LINE__, __func__);
    state = STATE_GREETING;
  }
}



bool player::read_player()
{
  std::ifstream file((PLAYER_DIR + name).c_str(),
                     std::ios::in);

  try
  {
    if (file.is_open())
    {
      std::string password_key  = "";
      std::string temp_password = "";

      if (!(file >> password_key >> temp_password))
        throw PLAYER_DIR + name + " - EOF before password key";

      if (password_key != "$password")
        throw PLAYER_DIR + name + " - Password section corrupted";

      std::string immortal_key  = "";
      std::string temp_immortal = "";

      if (!(file >> immortal_key >> temp_immortal))
        throw PLAYER_DIR + name + " - EOF before immortal key";

      if (immortal_key != "$immortal")
        throw PLAYER_DIR + name + " - Immortal section corrupted";

      std::string screen_size_key = "";
      unsigned short int temp_screen_size  = 0;

      if (!(file >> screen_size_key >> temp_screen_size))
        throw PLAYER_DIR + name + " - EOF before screen size key";

      if (screen_size_key != "$screen_size")
        throw PLAYER_DIR + name + " - Screen size section corrupted";

      std::string sex_key  = "";
      std::string temp_sex = "";

      if (!(file >> sex_key >> temp_sex))
        throw PLAYER_DIR + name + " - EOF before sex key";

      if (sex_key != "$sex")
        throw PLAYER_DIR + name + " - Sex section corrupted";

      std::string last_room_key   = "";
      unsigned int temp_last_room = 0;

      if (!(file >> last_room_key >> temp_last_room))
        throw PLAYER_DIR + name + " - EOF before last room key";

      if (last_room_key != "$last_room")
        throw PLAYER_DIR + name + " - Last room section corrupted";

      char end_key = '\0';

      if (!(file >> end_key))
        throw PLAYER_DIR + name + " - EOF before end key";

      file.close();

      if (temp_password != password)
      {
        send_to(
          "\r\n"
          "Wrong password.\r\n"
          "\r\n"
          "Password: ");

        return true;
      }

      if (temp_immortal == "yes")
        immortal = true;
      else if (temp_immortal == "no")
        immortal = false;
      else
        throw PLAYER_DIR + name + " - Immortal section corrupted";

      if (temp_sex == "male")
        sex = SEX_MALE;
      else if (temp_sex == "female")
        sex = SEX_FEMALE;
      else
        throw PLAYER_DIR + name + " - Sex section corrupted";

      screen_size = temp_screen_size;
      current_room = temp_last_room;

      send_to(
        "\r\nWelcome back " + name + "!\r\r\n\n");
      command::cmd_look(this, "");
    }
    else
    {
      send_to("\r\nPress enter to start calculating screen size: ");
      process_output();

      do
      {
	      inbuf.erase();
      } while (!process_input());

      state = STATE_SHOWING_LINES;
      handle_state();

      return true;
    }
  }
  catch (const std::string& str)
  {
    send_to("\r\nSorry, but your player file seems to be corrupted.\r\n");
    s->error("%s: %d: %s: %s\n", __FILE__, __LINE__, __func__, str.c_str());

    send_to("\r\nYour name? ");
    state = STATE_GETTING_NAME;

    return true;
  }

  return false;
}



std::string player::show_prompt()
{
  // More to come later...
  return "\r\n- ";
}
