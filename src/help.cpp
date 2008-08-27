/*
 * Kaolin - Exfoliate your face
 * Copyright (c) 2008 Devin Torres
 * Licensed under the MIT license.
 */

#include <string>
#include "main.h"
#include "server.h"
#include "help.h"
#include "util.h"



help::help()
  : description("")
{
#ifdef DEBUG
  log("help constructor...\n");
#endif
}



help::~help()
{
#ifdef DEBUG
  log("help destructor...\n");
#endif

  s->help_list.remove(this);
}



void help::add_keyword(const std::string& new_keyword)
{
  keywords.push_back(new_keyword);
}



std::vector<std::string> help::get_keywords() const
{
  return keywords;
}



void help::append_description(const std::string& extra_description)
{
  description += extra_description;
}



std::string help::get_description() const
{
  return description;
}
