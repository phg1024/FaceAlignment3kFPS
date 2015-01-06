#pragma once
#include "common.h"

inline string padWith(const string &s, char c, int n)
{
  string res = s;
  while (res.length() < n) {
    res = c + res;
  }
  return res;
}

template <typename T>
string toString(T val) {
  stringstream ss;
  ss << val;
  return ss.str();
}