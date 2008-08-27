/*
 * Kaolin - Exfoliate your face
 * Copyright (c) 2008 Devin Torres
 * Licensed under the MIT license.
 */

#include <iostream>
#include <string>
#include <vector>
#include "config.h"
#include "main.h"
#include "server.h"



namespace util
{

/*
 * Utility functions declatations.
 */
std::string center_str(const std::string& s)
{
  unsigned short int len = s.length();

  if (len >= 80)
    return s;

  std::string new_s(40 - (len / 2), ' ');

  new_s += s;

  return new_s;
}



std::string numberize(short int n)
{
  static const std::string ones_numerals[] = {
    "",
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine"
  };

  static const std::string tens_numerals[] = {
    "",
    "",
    "twenty",
    "thirty",
    "forty",
    "fifty",
    "sixty",
    "seventy",
    "eighty",
    "ninety"
  };

  static const std::string special_numbers[] = {
    "ten",
    "eleven",
    "twelve",
    "thirteen",
    "fourteen",
    "fifteen",
    "sixteen",
    "seventeen",
    "eighteen",
    "nineteen"
  };

  std::string buf = "";
  std::vector<short int> digits(0);

  if (n == 0)
  {
    buf += "zero";
    return buf;
  }

  if (n <= 9)
  {
    buf += ones_numerals[n];
    return buf;
  }

  if (n <= 19)
  {
    buf += special_numbers[n - 10];
    return buf;
  }

  if (n >= 10000 || n < 0)
    buf += n;

  digits[3] = n / 1000;
  n -= digits[3] * 1000;
  digits[2] = n / 100;
  n -= digits[2] * 100;
  digits[1] = n / 10;
  n -= digits[1] * 10;
  digits[0] = n;

  if (digits[3] > 0)
    buf += ones_numerals[digits[3]] + " thousand ";

  if (digits[2] > 0)
    buf += ones_numerals[digits[2]] + " hundred ";

  if (digits[1] > 0)
  {
    buf += tens_numerals[digits[1]];

    if (digits[0] >= 1)
      buf += "-";
  }

  if (digits[0] > 0)
    buf += ones_numerals[digits[0]];

  if (buf[buf.length() - 1] == ' ')
    buf.erase(buf.end() - 1);

  return buf;
}



bool is_number(const std::string& arg)
{
  for (unsigned short int i = 0; i < arg.length(); ++i)
  {
    if (!isdigit(arg[i]))
      return false;
  }

  return true;
}



bool str_prefix(const std::string& s1, const std::string& s2)
{
  for (unsigned short int i = 0; i < s1.length(); ++i)
  {
    if (tolower(s1[i]) != tolower(s2[i]))
      return false;
  }

  return true;
}

}
