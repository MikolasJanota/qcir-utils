%{
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unordered_map>
#include "auxiliary.hh"
#include "qtypes.hh"
using namespace std;
QFla qcir_qfla;
unordered_map<int,Gate> gates;
std::vector<int> all_lits;
unordered_map<const char*,int,cstrHash,cstrEq> name2id;
vector<bool> varids;
int qcir_last_id;

static std::vector<int> qcir_lit_stack;
static std::vector<int> qcir_var_stack;
static string filename("<ERR>");

int yylex();
int qcir_line;  // incremented by the lexer
int qcir_column; // updated by the lexer
void yyerror(const char *m);
void start_qcir_lex(const string& filename); // in lexer

void semantic_error(string msg) {
  cerr<<filename<<":"<<qcir_line<<":"<<qcir_column<<": "<<msg<<endl;
  exit(100);
}

int make_varid(const char* var_name) {
  ++qcir_last_id;
  varids.resize(qcir_last_id+1,false);
  const auto retv=qcir_last_id;
  varids[retv]=true;
  const auto i=name2id.insert(std::pair<const char*,int>(var_name,retv));
  if (!i.second) semantic_error("var redefinition '"+string(var_name)+"'");
  return retv;
}

int make_gateid(const char *s) {
  const auto i=name2id.find(s);
  if(i!=name2id.end()) {
     const auto retv=i->second;//already defined
     const size_t ix=(size_t)retv;
     if(ix<varids.size()&&varids[ix])
        semantic_error("trying to define a variable '"+string(s)+"'");
     return retv;
  }
  const auto id=++qcir_last_id;
  name2id[s]=id;
  return id;
}

int get_id(const char* s, bool polarity) {
 const auto i=name2id.find(s);
 const auto id=(i==name2id.end()) ? make_gateid(s)
                                  : i->second;
 return polarity ? id : -id;
}

void define_bool_gate(/*out*/Gate& g) {
  g.first_lit=all_lits.size();
  g.lit_count=qcir_lit_stack.size();
  FOR_EACH(i,qcir_lit_stack) all_lits.push_back(*i);
  qcir_lit_stack.clear();
}

void define_q_gate(/*out*/Gate& g) {
  g.first_lit=all_lits.size();
  assert(qcir_lit_stack.size()==1);
  g.lit_count=qcir_var_stack.size()+1;
  all_lits.push_back(qcir_lit_stack[0]);
  FOR_EACH(i,qcir_var_stack) all_lits.push_back(*i);
  qcir_lit_stack.clear();
  qcir_var_stack.clear();
}

int define_gate(GateType type, const char* gate_name) {
  const auto gate_id=make_gateid(gate_name);
  assert(0<=gate_id);
  Gate g;
  g.t=type;
  switch(type){
  case UNI_GT:
  case EXI_GT: define_q_gate(g); break;
  default: define_bool_gate(g);
  }
  const auto i=gates.insert(pair<int,Gate>(gate_id,g));
  if (!i.second)
    semantic_error("gate redefinition '"+string(gate_name)+"'");
  return gate_id;
}

void add_quant(QuantifierType qt) {
  const auto sz=qcir_qfla.pref.size();
  assert(qt!=FREE||!sz);
  auto& pref=qcir_qfla.pref;
  pref.resize(sz+1);
  pref[sz].first=qt;
  FOR_EACH(i,qcir_var_stack)
    pref[sz].second.push_back(*i);
  qcir_var_stack.clear();
}

%}

//-- SYMBOL SEMANTIC VALUES -----------------------------
%locations
%defines
%name-prefix "qcir_"

%code requires {
#include "qtypes.hh"
}

%union {
  unsigned long long val;
  GateType gt;
  QuantifierType qt;
  char* str;
}

%token <str> VAR_ID
%token FORMAT_ID NUM NL FORALL EXISTS FREE_TOK XOR AND OR ITE OUTPUT
%token LP RP EQ SEMI PLUS MINUS COMMA
%token ERROR_TOKEN
%type <val> new_var
%type <val> lit
%type <qt> quant
%type <gt> gate_definition

//-- GRAMMAR RULES ---------------------------------------
%%

qcir_file: header qblocks output_stmt gates

nls: NL
   | nls NL
   ;

header: FORMAT_ID optsize nls { }
      ;

optsize: /* empty */
       | NUM
       ;

optfree: /* empty */
       | FREE_TOK LP var_list RP {add_quant(FREE);}  nls
       ;

qblocks: optfree qblocks_
       ;

qblocks_: /* empty */
        | qblocks_ qblock
        ;

qblock: quant LP var_list RP {add_quant($1);} nls
      ;

output_stmt : OUTPUT LP lit RP {qcir_qfla.output=$3;} nls
            ;

gates: /* empty */
     | gates gate
     ;

gate: VAR_ID EQ gate_definition {define_gate($3,$1);} nls
    ;

gate_definition
    : XOR LP lit COMMA lit RP {$$=XOR_GT;qcir_lit_stack.push_back($3);qcir_lit_stack.push_back($5);}
    | AND LP lit_list RP {$$=AND_GT;}
    | OR LP lit_list RP {$$=OR_GT;}
    | ITE LP lit COMMA lit COMMA lit RP {
                               $$=ITE_GT;
                               qcir_lit_stack.push_back($3);
                               qcir_lit_stack.push_back($5);
                               qcir_lit_stack.push_back($7);
                               }
    | quant LP var_list SEMI lit RP {$$=($1==EXISTENTIAL)?EXI_GT:UNI_GT; qcir_lit_stack.push_back($5);}
    ;

quant: EXISTS  { $$=EXISTENTIAL; }
     | FORALL  { $$=UNIVERSAL; }
     ;

new_var: VAR_ID {$$=make_varid($1);}
       ;

var_list: new_var                {qcir_var_stack.push_back($1);}
        | var_list COMMA new_var {qcir_var_stack.push_back($3);}
        ;

lit_list: /* empty */
        | lit                  { qcir_lit_stack.push_back($1);}
        | lit_list COMMA lit   { qcir_lit_stack.push_back($3);}
        ;

optplus: /* empty */
       | PLUS
       ;

lit: optplus VAR_ID   {$$=get_id($2,true);}
   | MINUS VAR_ID  {$$=get_id($2,false);}
   ;
%%

void read_qcir(string _filename) {
  qcir_line=1;
  qcir_column=0;
  filename=_filename;
  start_qcir_lex(filename);
  qcir_parse();
}

void yyerror(const char *msg) {
  cerr<<filename<<":"<<qcir_line<<":"<<qcir_column<<": "<<msg<<endl;
  exit(100);
}
