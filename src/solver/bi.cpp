#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "term.hpp"

using namespace std;


int main(){
	string formula;
	getline(cin, formula);
  	Formula  * f = parseFormula(formula.c_str());
  	Solver s;

  	s.solve(f);

  return 0;

}



