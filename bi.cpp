#include <iostream>
#include <vector>
#include "term.hpp"

using namespace std;

Formula * parseFormula(const char *str_input);


int main(){
  Formula  * f = parseFormula("{x=x, f(x,x,y)=g(x,y)} |- f(3,3,1)=g(3,1)"); 
  cout << "hello" << endl;
  cout << f << endl;
  return 0;

}



