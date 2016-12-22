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

inline Eigen::Vector2d extractPoint(const Eigen::VectorXd &v, int idx) {
  return Eigen::Vector2d(v[idx*2], v[idx*2+1]);
}
