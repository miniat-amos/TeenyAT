#ifndef __WARNINGS_H__
#define __WARNINGS_H__

#include <string>

void new_warning(int lineno, std::string msg);
void print_warnings();

#endif /* __WARNINGS_H__ */