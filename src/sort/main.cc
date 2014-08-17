/*
 * File:  main.cc
 * Author:  mikolas
 * Created on:  Tue, Aug 05, 2014 4:53:56 PM
 * Copyright (C) 2014, Mikolas Janota
 */
#include "qcir_parse.tab.h"
#include "TopoPrint.hh"
#include "CNFPrint.hh"
#include <string>
#include <stdlib.h>
#include "Options.hh"
using namespace std;
void read_qcir(string filename);
extern QFla qcir_qfla;
ostream& print_usage(const Options& options,ostream& o);

int main(int argc, char** argv) {
  Options options;
  if (!options.parse(argc, argv)) {
     cerr << "ERROR: processing options." << endl;
     print_usage(options,cerr);
     return 100;
  }
  auto& rest = options.get_rest();
  if (rest.size()>1)
    cerr<<"WARNING: garbage at the end of command line."<<endl;

  if (options.get_help()){
     print_usage(options,cout);
     return 0;
  }

  const string filename(rest.size() ? rest[0] : "-");
  read_qcir(filename);
  if(options.get_cnf()){
    CNFPrint cp(qcir_qfla,cout);
    cp.print();
  }  else{
    TopoPrint tp(qcir_qfla,cout);
    tp.print();
  }
}

ostream& print_usage(const Options& options,ostream& o) {
  o << "USAGE: [OPTIONS] [FILENAME]" << endl;
  return options.print(o);
}
