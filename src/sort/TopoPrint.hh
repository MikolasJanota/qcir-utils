/*
 * File:  TopoPrint.hh
 * Author:  mikolas
 * Created on:  Sun, Aug 17, 2014 5:18:39 PM
 * Copyright (C) 2014, Mikolas Janota
 */
#ifndef TOPOPRINT_643632
#define TOPOPRINT_643632
#include "MemoVisitor.hh"
class TopoPrint : protected MemoVisitor<int> {
  public:
    TopoPrint(const QFla& qfla,std::ostream& o) : MemoVisitor<int>(qfla), o(o) {}

    void print() {
      o<<"#QCIR-G14"<<std::endl;
      FOR_EACH(i,qfla.pref) {
        const Quantification& q=*i;
        o<<str(q.first)<<"(";
        for(size_t i=0;i<q.second.size();++i){
          const auto v=q.second[i];
          assert(qfla.is_input(v));
          o<<(i?',':' ')<<name(v);
        }
        o<<")"<<std::endl;
      }
      const auto pos=qfla.output>=0;
      o<<"output("<<(pos?' ':'-')<<name(pos?qfla.output:-qfla.output)<<")"<<std::endl;
      visit(qfla.output);
    }

  protected:
    virtual int visit_input(int var) { return var; }
    virtual int visit_exi(int var, const Gate& g) { return visit_quant(var,g); }
    virtual int visit_uni(int var, const Gate& g) { return visit_quant(var,g); }
    virtual int visit_and(int var, const Gate& g) { return visit_op(var,g); }
    virtual int visit_or(int var, const Gate& g) { return visit_op(var,g); }
    virtual int visit_xor(int var, const Gate& g) { return visit_op(var,g); }
    virtual int visit_ite(int var, const Gate& g) { return visit_op(var,g); }
  private:
    std::ostream& o;

    int visit_op(int v, const Gate& g) {
      for(size_t i=0; i<g.lit_count; ++i) visit(qfla.all_lits[g.first_lit+i]);
      o<<name(v)<<" = "<<str(g.t)<<'(';
      for(size_t i=0; i<g.lit_count; ++i) {
        const auto arg = qfla.all_lits[g.first_lit+i];
        const auto pos=arg>=0;
        o<<(i?',':' ')<<(pos?' ':'-')<<name(pos?arg:-arg);
      }
      o<<")"<<std::endl;
      return v;
    }

    int visit_quant(int v, const Gate& g) {
      assert(g.lit_count>0);
      const auto arg=qfla.all_lits[g.first_lit];
      visit(arg);
      o<<name(v)<<" = "<<str(g.t)<<'(';
      for(size_t i=1; i<g.lit_count; ++i) {
        const auto v=qfla.all_lits[g.first_lit+i];
        assert(v>=0);
        o<<(i-1?',':' ')<<name(v);
      }
      const auto pos=arg>=0;
      o<<';'<<(pos?' ':'-')<<name(pos?arg:-arg);
      o<<")"<<std::endl;
      return v;
    }

    static inline const char* str(GateType gt) {
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

    static inline const char* str(QuantifierType qt) {
      switch (qt) {
        case UNIVERSAL: return "forall"; break;
        case EXISTENTIAL: return "exists"; break;
        case FREE: return "free"; break;
      }
      assert(0);
      return "ERROR";
    }
};
#endif
