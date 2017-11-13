#ifndef __UTIL_H_INCLUDED__
#define __UTIL_H_INCLUDED__

#include <vector>
#include <string>
#include <sstream>

using std::vector;
using std::string;
using std::stringstream;

// https://stackoverflow.com/a/236803/4262469
template<typename T>
void str_split(const string &s, char delim, T result) {
  stringstream ss(s);
  string item;
  while (getline(ss, item, delim)) {
    *(result++) = item;
  }
}

vector<string> str_split(const string &s, char delim) {
  vector<string> elems;
  str_split(s, delim, back_inserter(elems));
  return elems;
}

#endif
