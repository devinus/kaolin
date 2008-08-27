/*
 * Kaolin - Exfoliate your face
 * Copyright (c) 2008 Devin Torres
 * Licensed under the MIT license.
 */

#ifndef ROOM_H
#define ROOM_H

#include <string>



class room
{
private:
  unsigned long id;
  std::string name;
  std::string description;
  unsigned long north;
  unsigned long northeast;
  unsigned long northwest;
  unsigned long south;
  unsigned long southeast;
  unsigned long southwest;
  unsigned long east;
  unsigned long west;
  unsigned long up;
  unsigned long down;
public:
  room();
  ~room();
  void set_id(const unsigned long);
  unsigned long get_id() const;
  void set_name(const std::string&);
  std::string get_name() const;
  void append_description(const std::string&);
  std::string get_description() const;
  void set_north(const unsigned long);
  bool exit_north() const;
  unsigned long get_north() const;
  void set_northeast(const unsigned long);
  bool exit_northeast() const;
  unsigned long get_northeast() const;
  void set_northwest(const unsigned long);
  bool exit_northwest() const;
  unsigned long get_northwest() const;
  void set_south(const unsigned long);
  bool exit_south() const;
  unsigned long get_south() const;
  void set_southeast(const unsigned long);
  bool exit_southeast() const;
  unsigned long get_southeast() const;
  void set_southwest(const unsigned long);
  bool exit_southwest() const;
  unsigned long get_southwest() const;
  void set_east(const unsigned long);
  bool exit_east() const;
  unsigned long get_east() const;
  void set_west(const unsigned long);
  bool exit_west() const;
  unsigned long get_west() const;
  void set_up(const unsigned long);
  bool exit_up() const;
  unsigned long get_up() const;
  void set_down(const unsigned long);
  bool exit_down() const;
  unsigned long get_down() const;
};

#endif // ROOM_H
