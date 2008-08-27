/*
 * Kaolin - Exfoliate your face
 * Copyright (c) 2008 Devin Torres
 * Licensed under the MIT license.
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <sstream>



enum states
{
  STATE_CONNECTING,
  STATE_GREETING,
  STATE_GETTING_NAME,
  STATE_GETTING_PASSWORD,
  STATE_GETTING_SEX,
  STATE_SHOWING_LINES,
  STATE_SHOWING_LINES2,
  STATE_GETTING_SCREEN_SIZE,
  STATE_GETTING_SCREEN_SIZE2,
  STATE_PLAYING,
  STATE_CONFIRMING_DELETE,
  STATE_EXITTING
};

enum sexes
{
  SEX_MALE,
  SEX_FEMALE,
  SEX_NONE
};



class player
{
private:
  socklen_t desc;
  states state;
  bool immortal;
  std::string address;
  std::string name;
  std::string password;
  std::string inbuf;
  std::string last_inbuf;
  std::string outbuf;
  std::string last_outbuf;
  unsigned short int screen_size;
  sexes sex;
  unsigned long current_room;
public:
  player();
  ~player();
  void set_desc(socklen_t&);
  socklen_t get_desc() const;
  void set_state(states);
  states get_state() const;
  void set_immortal(bool);
  bool get_immortal() const;
  void set_address(const std::string&);
  std::string get_address() const;
  void set_name(const std::string&);
  std::string get_name() const;
  void set_password(const std::string&);
  std::string get_password() const;
  void append_inbuf(const std::string&);
  void set_inbuf(const std::string&);
  std::string get_inbuf() const;
  void set_last_outbuf(const std::string&);
  std::string get_last_outbuf() const;
  void set_screen_size(const unsigned short int);
  unsigned short int get_screen_size() const;
  void send_to(const std::string&);
  void send_to(const std::ostringstream&);
  void send_to_room(const std::string&);
  void send_to_room(const std::ostringstream&);
  void send_to_all_except(const std::string&);
  void send_to_all_except(const std::ostringstream&);
  void set_outbuf(const std::string&);
  std::string get_outbuf() const;
  void set_last_inbuf(const std::string&);
  std::string get_last_inbuf() const;
  void set_sex(const sexes);
  sexes get_sex() const;
  void set_current_room(const unsigned long);
  unsigned long get_current_room() const;
  void lookup_address(sockaddr_in&);
  bool process_input();
  bool process_output();
  bool valid_name();
  void handle_state();
  void save_player();
  bool read_player();
  std::string show_prompt();
};

#endif // PLAYER_H
