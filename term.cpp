#include <iostream>
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
  if(type() !=  t->type() || f->_name != _name || f->arity() != arity())
    return false;

  for(unsigned i=0; i<arity(); i++){
    if(! _args[i]->isEqual(f->_args[i]))
      return false;
  }

  return true;
}

const std::string & Fn::name(void) const{ return _name; }
Fn & Fn::name(const std::string & name) { _name = name; return *this;}

vector<Term*> Fn::args(void)
{
  return _args;
}

// ---------------

Term * makeConst(const std::string & value){ return new Fn(value, {}); }
Term * makeConst(int value){ return new Fn(to_string(value), {}); }

// ------------




Equality::Equality(Term *_t1, Term *_t2) : t1{_t1}, t2{_t2} {}

bool Equality::isEqual(Equality* eq) const
{
  if (t1->isEqual(eq->t1) && t2->isEqual(eq->t2))
    return true;

  if (t1->isEqual(eq->t2) && t2->isEqual(eq->t1))
    return true;

  return false;
}


Formula::Formula(std::vector<Equality*> * eqL, Equality * eq)
  : eqList{eqL}, toProve{eq }
{}

bool Formula::findEquality(Equality* eq) const
{
  std::vector<Equality*>::const_iterator it= eqList->cbegin();

  while (it != eqList->cend())
  {
    Equality* element = *it;

    if(element->isEqual(eq))
      return true;
    
    it++;
  }

  return false;
}

//---------------

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

std::ostream & operator << (std::ostream & o, Equality * e){
  return o << e->t1 << "=" << e->t2;
}

std::ostream & operator << (std::ostream & o, Formula * f){
  o << '{';
    auto b = f->eqList->cbegin();
    auto e = f->eqList->cend();
    if(b!= e){
      o << *b++;
      for(; b!=e; b++){
        o << ", " << *b;
      }
    }
  o << "} |- " << f->toProve;
  return o;
}
