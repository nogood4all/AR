#include <iostream>
#include <vector>
#include "term.hpp"

using namespace std;

// Var

Var::Var(const std::string & name) :  _name{name} {}

Term::Type Var::type(void) const {return T_VAR;}

const std::string & Var::name(void) const{ return _name; }

Var & Var::name(const std::string & name) { _name = name; return *this;}

bool Var::isEqual(Term const * t) const {
  return type() == t->type() && ((Var*)t)->_name == _name;
}


// Fn

Fn::Fn(const std::string & name, std::initializer_list<Term *> args) : _name{name}, _args{args} {}
Fn::Fn(const std::string & name, std::vector<Term *> & args) : _name{name}, _args{args} {}

unsigned Fn::arity(void) const {return _args.size(); }

Term::Type Fn::type(void) const {return T_FN; }

bool Fn::isEqual(Term const * t) const {
  auto f = ((Fn*)t);
  if(type() != t->type() || f->_name != _name || f->arity() != arity())
    return false;

  for(unsigned i=0; i<arity(); i++){
    if(! _args[i]->isEqual(f->_args[i]))
      return false;
  }

  return true;
}

const std::string & Fn::name(void) const{ return _name; }
Fn & Fn::name(const std::string & name) { _name = name; return *this;}

// ---------------

Term * makeConst(const std::string & value){ return new Fn(value, {}); }
Term * makeConst(int value){ return new Fn(to_string(value), {}); }

// ------------

std::ostream & operator << (std::ostream & o, Term * t){
  if(t->type() == Term::T_VAR)
    o <<((Var*)t)->name();
  else{
    o <<((Fn*)t)->name();
    auto b = ((Fn*)t)->_args.cbegin();
    auto e = ((Fn*)t)->_args.cend();
    if(b!= e){
      o << "(" << *b++;
      for(; b!=e; b++){
        o << ", " << *b;
      }
      o << ")";
    }
  }

  return o;
}

