/*
 * Kaolin - Exfoliate your face
 * Copyright (c) 2008 Devin Torres
 * Licensed under the MIT license.
 */

#ifndef HELP_H
#define HELP_H

#include <string>
#include <vector>



class help
{
private:
  std::vector<std::string> keywords;
  std::string description;
public:
  help();
  ~help();
  void add_keyword(const std::string&);
  std::vector<std::string> get_keywords() const;
  void append_description(const std::string&);
  std::string get_description() const;
};

#endif // HELP_H
