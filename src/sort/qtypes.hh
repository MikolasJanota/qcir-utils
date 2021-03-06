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
#include <unordered_map>
#include "auxiliary.hh"
enum QuantifierType {UNIVERSAL, EXISTENTIAL, FREE};
typedef std::vector<int> VarVec;
typedef std::pair<QuantifierType,VarVec> Quantification;
typedef std::vector<Quantification> Prefix;
typedef enum { AND_GT, OR_GT, XOR_GT, ITE_GT, UNI_GT, EXI_GT}  GateType;
struct Gate {
  GateType t;
  size_t   first_lit;
  size_t   lit_count;
};

struct QFla {
  Prefix pref;
  int     output;
  int     last_id;
  std::unordered_map<int,Gate> gates;
  std::vector<int> all_lits;
  std::unordered_map<const char*,int,cstrHash,cstrEq> name2id;
  std::vector<bool> varids;
  inline bool is_input(int v) const {
    assert(v>=0);
    const auto vx=(size_t)v;
    return vx<varids.size()&&varids[vx];
  }
};
#endif /* QTYPES_HH_15436 */
