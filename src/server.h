/*
 * Kaolin - Exfoliate your face
 * Copyright (c) 2008 Devin Torres
 * Licensed under the MIT license.
 */

#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <fstream>
#include <string>
#include <sstream>
#include <list>
#include <map>



class server
{
private:
  std::ofstream flog;
  std::ofstream ferr;

  void load_commands();
  void load_helps();
  void load_zones();
  void create_player(sockaddr_in&);
public:
  int host;
  bool shutdown;
  bool copyover;
  fd_set fd_read;
  fd_set fd_write;
  timeval current_time;
  std::list<room*> room_list;
  std::list<help*> help_list;
  std::list<player*> player_list;
  std::map<std::string, fun_ptr> command_map;

  server();
  ~server();
  void log(const char * const msg, ...);
  void error(const char * const msg, ...);
  void load_world();
  void game_loop(sockaddr_in&);
  void send_to_all(const std::string& msg);
  void send_to_all(const std::ostringstream& msg);
};



#endif // SERVER_H
