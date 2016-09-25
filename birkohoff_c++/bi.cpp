#include <iostream>
#include <vector>
#include "term.hpp"

using namespace std;

Term * parseFormula(const char *str_input);


int main(){
  Term * t = parseFormula("f(3,g(x))"); 
  cout << t << endl;
  return 0;

}



