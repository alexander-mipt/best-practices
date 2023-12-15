#ifndef JIT_AOT_COMPILERS_COURSE_DEBUG_H_
#define JIT_AOT_COMPILERS_COURSE_DEBUG_H_

#include <iostream>


namespace utils {
void AssertionFail(const char *expr, const char *file, unsigned line, const char *function);
void PrintWarning(const char *mess, const char *file, unsigned line, const char *function);

void AssertionFail(std::string expr, const char *file, unsigned line, const char *function);
void PrintWarning(std::string mess, const char *file, unsigned line, const char *function);
}   // end namespace utils

#endif // JIT_AOT_COMPILERS_COURSE_DEBUG_H_
