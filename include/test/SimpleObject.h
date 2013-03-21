#ifndef SIMPLE_OBJECT_H
#define SIMPLE_OBJECT_H

#include <ostream>
#include <istream>
using namespace std;

class SimpleObject
{
 public:
  int someNumber;
  char someCharacter;
  char someString[20];
  char someOtherCharacter;
  unsigned short someShortNumber;
  float someFloat;

  friend ostream& operator<< (ostream& out, SimpleObject& object)
  {
    out << object.someNumber;
    out << object.someCharacter;
    out << object.someString;
    out << object.someOtherCharacter;
    out << object.someShortNumber;
    out << object.someFloat;
  }

  friend istream& operator>> (istream& in, SimpleObject& object)
  {
    in >> object.someNumber;
    in >> object.someCharacter;
    in >> object.someString;
    in >> object.someOtherCharacter;
    in >> object.someShortNumber;
    in >> object.someFloat;  
  }
  
};
#endif
