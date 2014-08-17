/*
 * File:  MemoVisitor.hh
 * Author:  mikolas
 * Created on:  Sun, Aug 17, 2014 5:27:28 PM
 * Copyright (C) 2014, Mikolas Janota
 */
#ifndef MEMOVISITOR_HH_20405
#define MEMOVISITOR_HH_20405
#include "qtypes.hh"
#include <unordered_map>
#include <unordered_set>
#include <stdlib.h>
template <class R>
class MemoVisitor {
  public:
    MemoVisitor(const QFla& qfla) : qfla(qfla) {init();}
    virtual R visit(int lit) {
      const int v=lit>0 ? lit : -lit;
      const bool is_input=qfla.is_input(v);
      std::unordered_map<int,Gate>::const_iterator gate_it;
      if(!is_input) {//check for cycles
        gate_it=qfla.gates.find(v);
        if (gate_it==qfla.gates.end()) {
          std::cerr<<"gate undefined: "<<name(v)<<std::endl;
          exit(100);
        }
        if(contains(gates_on_stack,v)) {
          std::cerr<<"gate in a cycle: "<<name(v)<<std::endl;
          exit(100);
        }
      }
      //check memo
      const auto& gi=memo.find(v);
      if(gi!=memo.end()) return gi->second;
      // visit
      if(is_input) {
        const R rv=visit_input(v);
        memo[v]=rv;
        return rv;
      }
      gates_on_stack.insert(v);
      const auto& g=gate_it->second;
      R retv;
      switch(g.t){
        case UNI_GT: retv=visit_uni(v,g); break;
        case EXI_GT: retv=visit_exi(v,g); break;
        case AND_GT: retv=visit_and(v,g); break;
        case OR_GT:  retv=visit_or(v,g); break;
        case XOR_GT: retv=visit_xor(v,g); break;
        case ITE_GT: retv=visit_ite(v,g); break;
        default:
          retv=-1;
          assert(0);
      }
      memo[v]=retv;
      gates_on_stack.erase(v);
      return retv;
    }

    virtual R visit_input(int var) = 0;
    virtual R visit_exi(int var, const Gate& g) = 0;
    virtual R visit_uni(int var, const Gate& g) = 0;
    virtual R visit_and(int var, const Gate& g) = 0;
    virtual R visit_or(int var, const Gate& g) = 0;
    virtual R visit_xor(int var, const Gate& g) = 0;
    virtual R visit_ite(int var, const Gate& g) = 0;
  protected:
    const QFla& qfla;
    std::unordered_map<int,const char*> id2name;
    std::unordered_map<int,R> memo;
    std::unordered_set<int> gates_on_stack;

    inline const char* name(int v) {
      const auto i=id2name.find(v);
      assert(i!=id2name.end());
      return i->second;
    }

    void init() {
      FOR_EACH(i,qfla.name2id)
        id2name[i->second]=i->first;
    }
};
#endif /* MEMOVISITOR_HH_20405 */
