#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

#include "parser.hpp"

void TypeVal::print(const char *key) const
{
  cout << key << ": ";
  switch (type)
    {
    case T_INT:
      cout << boost::get<int>(Val);
      cout << endl;
      break;
    case T_DOUBLE:
      cout << boost::get<double>(Val);
      cout << endl;
      break;
    case T_STRING:
      cout << boost::get<char *>(Val);
      cout << endl;
      break;
    case T_VECTOR:
      pnl_vect_print(boost::get<PnlVect *>(Val));
      break;
    default:
      break;
    }
}

Param::Param() { }

Param::~Param()
{
  Hash::iterator it;
  for (it = M.begin() ; it != M.end() ; it++)
    {
      switch (it->second.type)
        {
        case T_VECTOR:
          pnl_vect_free(&(boost::get<PnlVect *>(it->second.Val)));
          break;
        case T_STRING:
          free(boost::get<char *>(it->second.Val));
          break;
        default:
          break;
        }
      free(it->first);
    }
}

Parser::Parser() : Param(), type_ldelim('<'), type_rdelim('>') { }

Parser::Parser(const char *InputFile)
  : Param()
{
  type_ldelim = '<';
  type_rdelim = '>';
  ReadInputFile(InputFile);
}

/* reads the file up to the end. Leading blanks are stripped
 * and lines beginning with # are ignored */
void Parser::ReadInputFile(const char *InputFile)
{

  FILE *fic;
  int j;
  char RedLine[MAX_CHAR_LINE];

  for (j = 0 ; j < MAX_CHAR_LINE ; j++) RedLine[j] = '\0';


  if ((fic = fopen(InputFile, "r")) == NULL)
    {
      printf("Unable to open Input File %s\n", InputFile);
      exit(1);
    }

  j = 0;
  int last_non_blank = 0;
  while (true)
    {
      char c;
      if ((c = fgetc(fic)) == EOF) break;
      switch (c)
        {
        case '#':
          while ((c = fgetc(fic)) != '\n');
          j = 0;
          last_non_blank = 0;
          break;
        case '\n':
          if (j > 0)
            {
              // Trim ending whitespaces
              RedLine[last_non_blank + 1] = '\0';
              add(RedLine);
            }
          j = 0;
          last_non_blank = 0;
          break;
        // Trim leading whitespaces
        case ' ':
          if (j > 0 && RedLine[j - 1] != ' ')
            {
              RedLine[j] = c;
              j++;
            }
          break;
        default:
          RedLine[j] = c;
          last_non_blank = j;
          j++;
          break;
        }
    }
  fclose(fic);
}

/**
 * Create a PnlVect from a string
 *
 * @param s a string containing numbers separated by white space
 *
 * @return
 */
static PnlVect *charPtrTovector(const char *s)
{
  PnlVect *v;
  const char *p = s;
  char *q;
  int len = 0;


  while (true)
    {
      strtod(p, &q);
      if (p == q) break;
      len ++;
      p = q;
    }
  v = pnl_vect_create(len);

  p = s;
  for (int i = 0 ; i < len ; i++)
    {
      LET(v, i) = strtod(p, &q);
      p = q;
    }

  return v;
}

static char *strtolower(char *s)
{
  for (int i = 0 ; i < strlen(s) ; i++) s[i] = tolower(s[i]);
  return s;
}

void Parser::add(char RedLine[])
{
  char *type_str, *key_str, *val_str;
  int type_len = 0, key_len = 0;

  key_str = RedLine;
  while (*key_str == ' ') key_str++;  // Trim leading spaces for the key
  while (key_str[key_len] != type_ldelim)
    {
      key_len ++;  // Find left delimitor for the type
    }
  type_str = key_str + key_len + 1; // Start of the type string
  key_len --;
  while (key_str[key_len] == ' ')
    {
      key_len --;  // Trim ending spaces for the key
    }

  while (*type_str == ' ') key_str++;  // Trim leading spaces for the type
  while (type_str[type_len] != type_rdelim)
    {
      type_len ++;  // Find left delimitor for the type
    }
  val_str = type_str + type_len + 1; // Start of the val string
  type_len --;
  while (type_str[type_len] == ' ')
    {
      type_len --;  // Trim leading spaces for the type
    }
  while (*val_str == ' ')
    {
      val_str ++;  // Trim leading spaces for the value
    }

  type_str[type_len + 1] = '\0';
  key_str[key_len + 1] = '\0';
  TypeVal T;
  if (strcmp(type_str, "int") == 0)
    {
      int x;
      sscanf(val_str, "%d", &x);
      T.type = T_INT;
      T.Val = x;
    }
  else if (strcmp(type_str, "float") == 0)
    {
      double x;
      sscanf(val_str, "%lf", &x);
      T.type = T_DOUBLE;
      T.Val = x;
    }
  else if (strcmp(type_str, "string") == 0)
    {
      T.type = T_STRING;
      T.Val = strdup(val_str);
    }
  else if (strcmp(type_str, "vector") == 0)
    {
      T.type = T_VECTOR;
      T.Val = charPtrTovector(val_str);
    }
  char *copy_key = strtolower(strdup(key_str));
  M[copy_key] = T;
}

bool Param::check_if_key(Hash::const_iterator &it, const char *key) const
{
  it = M.find(const_cast<char *>(key));
  if (it == M.end())
    {
      cerr << key << " entry is missing" << endl;
      return false;
    }
  else
    return true;
}

bool Param::extract(const char *key, PnlVect *&out, int size) const
{
  Hash::const_iterator it;
  out = NULL;

  if (check_if_key(it, key) == false) return false;
  PnlVect *v;
  try
    {
      v = boost::get<PnlVect *>(it->second.Val);
    }
  catch (boost::bad_get e)
    {
      std::cerr << "Boost bad get for " << key << std::endl;
      return false;
    }

  if (v->size == 1)
    {
      out = pnl_vect_create_from_double(size, GET(v, 0));
    }
  else
    {
      if (v->size != size)
        {
          std::cerr << key << " size mismatch for vector " << key << std::endl;
          return false;
        }
      out = pnl_vect_copy(v);
    }
  return true;
}


