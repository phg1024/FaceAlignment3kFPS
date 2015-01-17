#pragma once
#include "common.h"

#include "numerical.hpp"

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

inline arma::vec2 extractPoint(const arma::vec &v, int idx) {
  arma::vec2 p;
  p(1) = v(idx * 2); p(2) = v(idx * 2 + 1);
  return p;
}