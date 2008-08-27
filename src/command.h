/*
 * Kaolin - Exfoliate your face
 * Copyright (c) 2008 Devin Torres
 * Licensed under the MIT license.
 */

#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include "main.h"



/*
 * Command declaration macros.
 */
#define CMD_PROTO(func) \
  bool func(player* const&, const std::string&)
 
#define CMD(func) \
  bool func(player* const& p, const std::string& arg)



/*
 * Player command prototypes.
 */
namespace command
{
  CMD_PROTO(cmd_commands);
  CMD_PROTO(cmd_clear);
  CMD_PROTO(cmd_copyover);
  CMD_PROTO(cmd_credits);
  CMD_PROTO(cmd_delete);
  CMD_PROTO(cmd_down);
  CMD_PROTO(cmd_east);
  CMD_PROTO(cmd_echo);
  CMD_PROTO(cmd_emote);
  CMD_PROTO(cmd_goto);
  CMD_PROTO(cmd_immortalize);
  CMD_PROTO(cmd_help);
  CMD_PROTO(cmd_look);
  CMD_PROTO(cmd_north);
  CMD_PROTO(cmd_northeast);
  CMD_PROTO(cmd_northwest);
  CMD_PROTO(cmd_password);
  CMD_PROTO(cmd_repeat);
  CMD_PROTO(cmd_save);
  CMD_PROTO(cmd_say);
  CMD_PROTO(cmd_screensize);
  CMD_PROTO(cmd_set);
  CMD_PROTO(cmd_shutdown);
  CMD_PROTO(cmd_south);
  CMD_PROTO(cmd_southeast);
  CMD_PROTO(cmd_southwest);
  CMD_PROTO(cmd_up);
  CMD_PROTO(cmd_qui);
  CMD_PROTO(cmd_quit);
  CMD_PROTO(cmd_west);
  CMD_PROTO(cmd_who);
}

#endif // COMMAND_H
