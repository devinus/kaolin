/*
 * Kaolin - Exfoliate your face
 * Copyright (c) 2008 Devin Torres
 * Licensed under the MIT license.
 */

#ifndef UTIL_H
#define UTIL_H

#include <string>



namespace util
{

/*
 * Utility function prototypes.
 */
std::string center_str(const std::string&);
std::string numberize(short int);
bool is_number(const std::string&);
bool str_prefix(const std::string&, const std::string&);

}



#endif // UTIL_H
