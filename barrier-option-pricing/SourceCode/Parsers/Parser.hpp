#pragma once


#include <iostream>
#include <map>
//#include <boost/variant.hpp>
#include <cstring>
#include "pnl/pnl_matrix.h"

#define MAX_CHAR_LINE 1024

/// List of possible types
typedef enum
{
  T_NULL,
  T_INT,
  T_DOUBLE,
  T_VECTOR,
  T_STRING
} T_type;

/// Comparison structure
struct ltstr
{
  bool operator()(const char *s1, const char *s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

/// Value holding structure
struct TypeVal
{
  T_type type; //!< the real type of the value
  boost::variant<double, int, char *, PnlVect *> Val; //!< the value itself
  TypeVal() { };
  // Beware not to delete anything in the destructor because we rely on the
  // copy of addresses and not a hard copy operator.
  ~TypeVal() { };
  void print(const char *key) const; //!< Print the value
};

/// A map holding pairs (key, value)
typedef std::map<char *, TypeVal, ltstr> Hash;

class Param
{
public:
  Hash M;
  Param();

  /**
   * Extract the value associated to a key and store it into T
   *
   * @param key a string
   * @param out contains the value associated to key on output
   *
   * @return true if key was found. False is thekey is not found or the
   * conversion the type of out is not possible.
   *
   */
  template <typename T> bool extract(const char *key, T &out) const
  {
    Hash::const_iterator it;
    out = 0;
    if (check_if_key(it, key) == false) return false;
    try
      {
        out = boost::get<T>(it->second.Val);
        return true;
      }
    catch (boost::bad_get e)
      {
        std::cerr << "Boost bad get for " << key << std::endl;
        return false;
      }
  }

  /**
   * Specialised version of the previous function for PnlVect
   *
   * @param key a string
   * @param out contains the value associated to key on output
   * @param size length of the expected vector
   *
   * @return  true or false
   */
  bool extract(const char *key, PnlVect  *&out, int size) const;

  ~Param();

  void print() const
  {
    Hash::const_iterator it;
    for (it = M.begin() ; it != M.end() ; it++) it->second.print(it->first);
  }
private:
  /**
   * Check if a key is present in the map.
   *
   * @param it an iterator on the map
   * @param key a string
   *
   * @return true or false
   */
  bool check_if_key(Hash::const_iterator &it, const char *key) const;

};

/// A parser class based on Param
/// It parses files with the syntax
///   key \<type\> value
/// where key can contain several words
///       type can be int, float, string, vector
//        value is a string holding data of the corresponding type
class Parser : public Param
{
public:
  Parser();
  Parser(const char *InputFile);
private:
  /// Read a whole file and call add on each line.
  void ReadInputFile(const char *InputFile);
  /// Add the pair (key, value) desribed by the current line to the map.
  void add(char RedLine[]);
  char type_ldelim; //!< left delimiter for the type
  char type_rdelim; //!< right delimiter for the type
};


/* vim: set ft=cpp: */

