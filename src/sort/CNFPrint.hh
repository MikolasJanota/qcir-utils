/*
 * File:  CNFPrint.hh
 * Author:  mikolas
 * Created on:  Sun, Aug 17, 2014 8:06:44 PM
 * Copyright (C) 2014, Mikolas Janota
 */
#ifndef CNFPRINT_HH_11799
#define CNFPRINT_HH_11799
#include "MemoVisitor.hh"
class ClauseCounter : protected MemoVisitor<int> {
  public:
    ClauseCounter(const QFla& qfla) : MemoVisitor(qfla){}
    int count() {
      total=0;
      visit(qfla.output);
      return total;
    }
  protected:
    virtual int visit_input(int var) {return 0;}
    virtual int visit_exi(int var, const Gate& g) { return visit_quant(var,g); }
    virtual int visit_uni(int var, const Gate& g) { return visit_quant(var,g); }
    virtual int visit_and(int var, const Gate& g) {
      visit_ops(var,g);
      return total+g.lit_count+1;
    }
    virtual int visit_or(int var, const Gate& g)  {
      visit_ops(var,g);
      return total+g.lit_count+1;
    }
    virtual int visit_xor(int var, const Gate& g) {
      assert(g.lit_count==2);
      visit_ops(var,g);
      return total+4;
    }
    virtual int visit_ite(int var, const Gate& g) {
      return total+4;
    }

  protected:
    int total;
    void visit_ops(int v, const Gate& g) {
      for(size_t i=0; i<g.lit_count; ++i)
        visit(qfla.all_lits[g.first_lit+i]);
    }
    int visit_quant(int v, const Gate& g) {
      assert(0);
      std::cerr<<"non-prenex not supported"<<name(v)<<std::endl;
      exit(100);
    }
};

class CNFPrint : protected MemoVisitor<int> {
  public:
    CNFPrint(const QFla& qfla,std::ostream& o) : MemoVisitor(qfla), o(o) {}

    void print() {
      ClauseCounter cc(qfla);
      const int ccnt=cc.count();
      for(int v=1;v<=qfla.last_id;++v) {
        if(!qfla.is_input(v)) continue;
        o<<"c m "<<v<<" "<<name(v)<<std::endl;
      }
      o<<"p cnf "<<qfla.last_id<<" "<<ccnt<<std::endl;
      FOR_EACH(i,qfla.pref) {
        const Quantification& q=*i;
        o<<str(q.first);
        for(size_t i=0;i<q.second.size();++i){
          const auto v=q.second[i];
          assert(qfla.is_input(v));
          o<<' '<<v;
        }
        o<<" 0"<<std::endl;
      }
      o<<encode(qfla.output)<<" 0"<<std::endl;
    }

  protected:
    std::ostream& o;

    int encode(int lit) {
      const bool pos=lit>0;
      return pos ? visit(lit) : -visit(-lit);
    }
    virtual int visit_input(int var) { return var; }
    virtual int visit_exi(int var, const Gate& g) { return visit_quant(var,g); }
    virtual int visit_uni(int var, const Gate& g) { return visit_quant(var,g); }

    virtual int visit_or(int var, const Gate& g) {
      for(size_t i=0; i<g.lit_count; ++i) {
        const int l=encode(qfla.all_lits[g.first_lit+i]);
        o<<var<<" "<<-l<<" 0"<<std::endl;
      }
      o<<-var;
      for(size_t i=0; i<g.lit_count; ++i) {
        const int l=encode(qfla.all_lits[g.first_lit+i]);
        o<<" "<<l;
      }
      o<<" 0"<<std::endl;
      return var;
    }

    virtual int visit_and(int var, const Gate& g) {
      for(size_t i=0; i<g.lit_count; ++i) {
        const int l=encode(qfla.all_lits[g.first_lit+i]);
        o<<-var<<" "<<l<<" 0"<<std::endl;
      }
      o<<var;
      for(size_t i=0; i<g.lit_count; ++i) {
        const int l=encode(qfla.all_lits[g.first_lit+i]);
        o<<" "<<-l;
      }
      o<<" 0"<<std::endl;
      return var;
    }

    virtual int visit_xor(int var, const Gate& g) {
      assert(g.lit_count==2);
      const auto a=encode(qfla.all_lits[g.first_lit]);
      const auto b=encode(qfla.all_lits[g.first_lit+1]);
      o<<-a<<" "<<-b<<" "<<-var<<" 0"<<std::endl;
      o<<a<<" "<<b<<" "<<-var<<" 0"<<std::endl;
      o<<a<<" "<<-b<<" "<<var<<" 0"<<std::endl;
      o<<-a<<" "<<-b<<" "<<var<<" 0"<<std::endl;
      return var;
    }

    virtual int visit_ite(int var, const Gate& g) { 
      assert(g.lit_count==3);
      const auto c=encode(qfla.all_lits[g.first_lit]);
      const auto t=encode(qfla.all_lits[g.first_lit+1]);
      const auto e=encode(qfla.all_lits[g.first_lit+2]);
      o<<-c<<" "<<-t<<" "<<var<<" 0"<<std::endl;
      o<<-c<<" "<<t<<" "<<-var<<" 0"<<std::endl;
      o<<c<<" "<<-e<<" "<<var<<" 0"<<std::endl;
      o<<c<<" "<<e<<" "<<var<<" 0"<<std::endl;
      return var;
    }

    int visit_quant(int v, const Gate& g) {
      assert(0);
      std::cerr<<"non-prenex not supported"<<name(v)<<std::endl;
      exit(100);
    }

    static inline const char* str(QuantifierType qt) {
      switch (qt) {
        case UNIVERSAL: return "a"; break;
        case EXISTENTIAL: return "e"; break;
        case FREE: return "f"; break;
      }
      assert(0);
      return "ERROR";
    }
};
#endif /* CNFPRINT_HH_11799 */
