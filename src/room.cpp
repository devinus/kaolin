/*
 * Kaolin - Exfoliate your face
 * Copyright (c) 2008 Devin Torres
 * Licensed under the MIT license.
 */

#include <string>
#include "main.h"
#include "server.h"
#include "room.h"
#include "util.h"



room::room()
  : id(0), name(""), description(""), north(0), northeast(0), northwest(0),
      south(0), southeast(0), southwest(0), east(0), west(0), up(0), down(0)
{
#ifdef DEBUG
  log("room constructor...\n");
#endif
}



room::~room()
{
#ifdef DEBUG
  log("room destructor...\n");
#endif

  s->room_list.remove(this);
}



void room::set_id(const unsigned long new_id)
{
  id = new_id;
}



unsigned long room::get_id() const
{
  return id;
}



void room::set_name(const std::string& new_name)
{
  name = new_name;
}



std::string room::get_name() const
{
  return name;
}



void room::append_description(const std::string& extra_description)
{
  description += extra_description;
}



std::string room::get_description() const
{
  return description;
}



void room::set_north(const unsigned long new_id)
{
  north = new_id;
}



bool room::exit_north() const
{
  return (north == 0 ? false : true);
}



unsigned long room::get_north() const
{
  return north;
}



void room::set_northeast(const unsigned long new_id)
{
  northeast = new_id;
}



bool room::exit_northeast() const
{
  return (northeast == 0 ? false : true);
}



unsigned long room::get_northeast() const
{
  return northeast;
}



void room::set_northwest(const unsigned long new_id)
{
  northwest = new_id;
}



bool room::exit_northwest() const
{
  return (northwest == 0 ? false : true);
}



unsigned long room::get_northwest() const
{
  return northwest;
}



void room::set_south(const unsigned long new_id)
{
  south = new_id;
}



bool room::exit_south() const
{
  return (south == 0 ? false : true);
}



unsigned long room::get_south() const
{
  return south;
}



void room::set_southeast(const unsigned long new_id)
{
  southeast = new_id;
}



bool room::exit_southeast() const
{
  return (southeast == 0 ? false : true);
}



unsigned long room::get_southeast() const
{
  return southeast;
}



void room::set_southwest(const unsigned long new_id)
{
  southwest = new_id;
}



bool room::exit_southwest() const
{
  return (southwest == 0 ? false : true);
}



unsigned long room::get_southwest() const
{
  return southwest;
}



void room::set_east(const unsigned long new_id)
{
  east = new_id;
}



bool room::exit_east() const
{
  return (east == 0 ? false : true);
}



unsigned long room::get_east() const
{
  return east;
}



void room::set_west(const unsigned long new_id)
{
  west = new_id;
}



bool room::exit_west() const
{
  return (west == 0 ? false : true);
}



unsigned long room::get_west() const
{
  return west;
}



void room::set_up(const unsigned long new_id)
{
  up = new_id;
}



bool room::exit_up() const
{
  return (up == 0 ? false : true);
}



unsigned long room::get_up() const
{
  return up;
}



void room::set_down(const unsigned long new_id)
{
  down = new_id;
}



bool room::exit_down() const
{
  return (down == 0 ? false : true);
}



unsigned long room::get_down() const
{
  return down;
}
