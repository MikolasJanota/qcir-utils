/*
 * File:  main.cc
 * Author:  mikolas
 * Created on:  Tue, Aug 05, 2014 4:53:56 PM
 * Copyright (C) 2014, Mikolas Janota
 */
#include "qcir_parse.tab.h"
#include "qtypes.hh"
#include "auxiliary.hh"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <stdlib.h>
using namespace std;
void read_qcir(string filename);
extern QFla qcir_qfla;
extern unordered_map<const char*,int,cstrHash,cstrEq> name2id;
extern unordered_map<int,Gate> gates;
extern std::vector<int> all_lits;
extern vector<bool> varids;

unordered_map<int,const char*> id2name;
unordered_set<int> visited_gates;
unordered_set<int> gates_on_stack;

template <class T>
inline bool contains(std::unordered_set<T> es, const T& e) {
  return es.count(e);
}

void init() {
  FOR_EACH(i,name2id)
    id2name[i->second]=i->first;
}

inline const char* name(int v) {
  const auto i=id2name.find(v);
  assert(i!=id2name.end());
  return i->second;
}

inline const char* str(QuantifierType qt) {
  switch (qt) {
    case UNIVERSAL: return "forall"; break;
    case EXISTENTIAL: return "exists"; break;
    case FREE: return "free"; break;
  }
  assert(0);
}

inline const char* str(GateType gt) {
  switch (gt) {
    case XOR_GT: return "xor"; break;
    case AND_GT: return "and"; break;
    case OR_GT: return "or"; break;
    case ITE_GT: return "ite"; break;
    case EXI_GT: return "exists"; break;
    case UNI_GT: return "forall"; break;
    default:
        assert(0);
        exit(1000);
  }
}

ostream& print_def(ostream& o, int lit) {
  const int v=lit>0 ? lit : -lit;
  const size_t vx=(size_t)v;
  if(vx<varids.size()&&varids[vx]) return o;//input
  const auto i=gates.find(v);
  if (i==gates.end()) {
    cerr<<"gate undefined: "<<name(v)<<endl;
    exit(100);
  }
  if(contains(gates_on_stack,v)) {
    cerr<<"gate in a cycle: "<<v<<endl;
    exit(100);
  }
  if(contains(visited_gates,v)) return o;
  gates_on_stack.insert(v);
  visited_gates.insert(v);
  const auto& g=i->second;
  if(g.t==UNI_GT||g.t==EXI_GT) {
    assert(g.lit_count>0);
    const auto arg=all_lits[g.first_lit];
    print_def(o,arg);
    o<<name(v)<<" = "<<str(g.t);
    for(size_t i=1; i<g.lit_count; ++i) {
      const auto v=all_lits[g.first_lit+i];
      assert(v>=0);
      o<<(i-1?',':'(')<<name(v);
    }
    const auto pos=arg>=0;
    o<<';'<<(pos?'+':'-')<<name(pos?arg:-arg);
  } else {
    for(size_t i=0; i<g.lit_count; ++i)
      print_def(o,all_lits[g.first_lit+i]);
    o<<name(v)<<" = "<<str(g.t);
    for(size_t i=0; i<g.lit_count; ++i) {
      const auto arg = all_lits[g.first_lit+i];
      const auto pos=arg>=0;
      o<<(i?',':'(')<<(pos?'+':'-')<<name(pos?arg:-arg);
    }
  }
  gates_on_stack.erase(v);
  return  o<<')'<<endl;
}

int main(int argc, char** argv) {
  string filename=argc==1 ? "-" : argv[1];
  read_qcir(filename);
  init();

  cout<<"#QCIR-G14"<<endl;
  FOR_EACH(i,qcir_qfla.pref) {
    const Quantification& q=*i;
    cout<<str(q.first);
    for(size_t i=0;i<q.second.size();++i) cout<<(i?',':'(')<<name(q.second[i]);
    cout<<")"<<endl;
  }
  const auto pos=qcir_qfla.output>=0;
  cout<<"output("<<(pos?'+':'-')<<name(pos?qcir_qfla.output:-qcir_qfla.output)<<")"<<endl;
  print_def(cout,qcir_qfla.output);
}
