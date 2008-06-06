#ifndef C2_2GRAPH_H
#define C2_2GRAPH_H

#include <string>
#include <list>
#include "c2_lsystem.h"

std::string print_lines2txt(Group & , std::list < line > & , double min_x, double max_x, double min_y, double max_y);
std::string print_lines2mgl(Group & , std::list < line > & , double min_x, double max_x, double min_y, double max_y);
std::string print_lines2png(Group & , std::list < line > & , double min_x, double max_x, double min_y, double max_y);

#endif //C2_2GRAPH_H

