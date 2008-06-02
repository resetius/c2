#ifndef LSYSTEM_H
#define LSYSTEM_H
#include <string>
#include <map>

struct Parser
{
  double alpha;
  double theta;
    std::string axiom;
    std::map < char, std::string > r;
  typedef std::map < char, std::string >::iterator iterator;

    Parser ():alpha (0.0), theta (0.0)
  {
  }
};
#endif //LSYSTEM_H

