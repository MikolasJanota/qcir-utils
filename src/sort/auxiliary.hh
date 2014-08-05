/*
 * File:   auxiliary.hh
 * Author: mikolas
 * Created on October 12, 2011
 */
#ifndef AUXILIARY_HH
#define	AUXILIARY_HH
#include <vector>
#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <assert.h>
#define __PL (std::cerr << __FILE__ << ":" << __LINE__ << std::endl).flush();
#define FOR_EACH(index,iterated)\
  for (auto index = (iterated).begin(); index != (iterated).end();++index)

static inline double read_cpu_time() {
  struct rusage ru; getrusage(RUSAGE_SELF, &ru);
  return (double)ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec / 1000000;
}

struct cstrHash {
  inline size_t operator() (const char* s) const {
    size_t hash = 5381;
    char c;
    while ((c = *s)) {
      hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
      ++s;
    }

    return hash;
  }
};

struct cstrEq {
  inline size_t operator() (const char* s1,const char* s2) const {
    return strcmp(s1,s2)==0; };
};

#endif	/* AUXILIARY_HH */

