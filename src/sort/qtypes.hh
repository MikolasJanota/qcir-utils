/*
 * File:  qtypes.hh
 * Author:  mikolas
 * Created on:  Fri, Aug 01, 2014 4:42:21 PM
 * Copyright (C) 2014, Mikolas Janota
 */
#ifndef QTYPES_HH_15436
#define QTYPES_HH_15436
#include <vector>
#include <ostream>
enum QuantifierType {UNIVERSAL, EXISTENTIAL};
typedef std::vector<int> VarVec;
typedef std::pair<QuantifierType,VarVec> Quantification;
typedef std::vector<Quantification> Prefix;
struct QFla {
  Prefix pref;
  int     output;
};

typedef enum { AND_GT, OR_GT, XOR_GT, ITE_GT, UNI_GT, EXI_GT }  GateType;
struct Gate {
  GateType t;
  size_t   first_lit;
  size_t   lit_count;
};
#endif /* QTYPES_HH_15436 */
