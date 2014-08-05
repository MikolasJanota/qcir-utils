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
void yyerror(const char *m);
void start_qcir_lex(const string& filename); // in lexer


void semantic_error(string msg) {
  cerr<<filename<<":"<<qcir_line<<": "<<msg<<endl;
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

int define_gate(GateType type, const char* gate_name) {
  const auto gate_id=make_gateid(gate_name);
  assert(0<=gate_id);
  Gate g;
  g.t=type;
  g.first_lit=all_lits.size();
  g.lit_count=qcir_lit_stack.size();
  FOR_EACH(i,qcir_lit_stack) all_lits.push_back(*i);
  qcir_lit_stack.clear();
  const auto i=gates.insert(pair<int,Gate>(gate_id,g));
  if (!i.second)
    semantic_error("gate redefinition '"+string(gate_name)+"'");
  return gate_id;
}

%}

//-- SYMBOL SEMANTIC VALUES -----------------------------
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
%token FORMAT_ID NUM NL FORALL EXISTS XOR AND OR ITE OUTPUT
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

header : FORMAT_ID size nls { }
       ;

size:
    | NUM
    ;

qblocks:
       | qblocks qblock
       ;

qblock: quant LP var_list RP nls {
      const auto sz=qcir_qfla.pref.size();
      auto& pref=qcir_qfla.pref;
      pref.resize(sz+1);
      pref[sz].first=$1;
      FOR_EACH(i,qcir_var_stack)pref[sz].second.push_back(*i);
      qcir_var_stack.clear();
      }
     ;

output_stmt : OUTPUT LP lit RP nls {qcir_qfla.output=$3;};

gates:
  | gates gate
  ;

gate: VAR_ID EQ gate_definition {define_gate($3,$1);}
    ;

gate_definition:
      XOR LP lit COMMA lit RP nls {$$=XOR_GT;qcir_lit_stack.push_back($3);qcir_lit_stack.push_back($5);}
    | AND LP lit_list RP nls      {$$=AND_GT;}
    | OR LP lit_list RP nls       {$$=OR_GT;}
    | ITE LP lit COMMA lit COMMA lit RP nls {
                               $$=ITE_GT;
                               qcir_lit_stack.push_back($3);
                               qcir_lit_stack.push_back($5);
                               qcir_lit_stack.push_back($7);
                               }
//    | quant LP var_list SEMI lit RP nls

quant: EXISTS  { $$=EXISTENTIAL; }
     | FORALL  { $$=UNIVERSAL; }
     ;

new_var: VAR_ID {$$=make_varid($1);}

var_list:
    | new_var                {qcir_var_stack.push_back($1);}
    | var_list COMMA new_var {qcir_var_stack.push_back($3);}
    ;

lit_list:
         | lit                  { qcir_lit_stack.push_back($1);}
         | lit_list COMMA lit   { qcir_lit_stack.push_back($3);}
         ;

optplus:
    | PLUS
    ;

lit: optplus VAR_ID   {$$=get_id($2,true);}
   | MINUS VAR_ID  {$$=get_id($2,false);}
    ;
%%

void read_qcir(string _filename) {
  qcir_line=1;
  filename=_filename;
  start_qcir_lex(filename);
  qcir_parse();
}

void yyerror(const char *msg) {
  cerr<<filename<<":"<<qcir_line<<": "<<msg<<endl;
  exit(100);
}
