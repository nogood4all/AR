#include "term.hpp"

using namespace std;


// ---------------
// Var
// ---------------

Var::Var(const std::string & name) :  _name{name}
{}

Var::Var(Var* var)
{
  _name = string(var->_name);
}

Term::Type Var::type(void) const
{
  return T_VAR;
}

const std::string & Var::name(void) const
{
  return _name;
}

Var & Var::name(const std::string & name)
{
   _name = name;
   return *this;
}

bool Var::isEqual(Term const * t) const
{
  return type() == t->type() && ((Var*)t)->_name == _name;
}



// ---------------
// Fn
// ---------------

Fn::Fn(const std::string & name, std::initializer_list<Term *> args) : _name{name}, _args{args}
{}

Fn::Fn(const std::string & name, std::vector<Term *> & args) : _name{name}, _args{args}
{}

Fn::Fn(Fn* func)
{
  _name = string(func->_name);

  for(unsigned i = 0; i < func->arity() ; i++)
    _args.push_back(Solver::make_copy(func->_args[i]));
}
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

// ---------------


// ---------------
//EQUALITY
// ---------------
Equality::Equality(Term *_t1, Term *_t2) : t1{_t1}, t2{_t2} {}

bool Equality::isEqual(Equality* eq) const
{
  if (t1->isEqual(eq->t1) && t2->isEqual(eq->t2))
    return true;

  if (t1->isEqual(eq->t2) && t2->isEqual(eq->t1))
    return true;

  return false;
}


// ---------------
//FORMULA
// ---------------
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



//------------------
// PRINT OPERATORS
//------------------

std::ostream & operator << (std::ostream & o, Term * t)
{
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

std::ostream & operator << (std::ostream & o, Equality * e)
{
  return o << e->t1 << "=" << e->t2;
}

std::ostream & operator << (std::ostream & o, Formula * f)
{
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




//------------------
// Solver
//------------------
void Solver::instantiate_term(Term* term, Term* what, Term* with)
{

  //Uraditi sa dubokim copy konstruktorima i napraviti duboke copy konstruktore
  if(term->type() == what->type())
  {
    if(term->isEqual(what))
      term = make_copy(with);

  }
  else if (term->type() == 0) // if var or const
  {
    term = make_copy(with);
  }
  else if (term->type() == 1) // if fn
  {
    Fn * func = (Fn*) term;
    vector<Term*> args = func->args();
    for (unsigned i = 0; i < args.size(); i++)
    {
        instantiate_term(args[i], what, with);
    }
  }

  return;
}

Term* Solver::make_copy(Term* term)
{
  if (term->type() == 0)
    return new Var((Var*)term);

    return new Fn((Fn*)term);
}

void Solver::apply_tran(int eq_num, string arg_term)
{
  Term* arg = parseTerm(arg_term.c_str());
  Equality* eq = _prove_stack[eq_num-1];

  if (!arg)
    exit(EXIT_FAILURE);

  Equality* eq1 = new Equality(eq->t1, arg);
  Equality* eq2 = new Equality(arg, eq->t2);

  _prove_stack.erase(_prove_stack.begin()+(eq_num-1));
  _prove_stack.push_back(eq1);
  _prove_stack.push_back(eq2);
}

void Solver::apply_axiom(Formula* f, int eq_num)
{
    if (f->findEquality(_prove_stack[eq_num-1]))
      _prove_stack.erase(_prove_stack.begin()+(eq_num-1));
}

void Solver::apply_inst(int eq_num, string arg_term, string arg_term2)
{
    Term* what = parseTerm(arg_term.c_str());
    Term* with = parseTerm(arg_term2.c_str());

    if(!with || !what)
      exit(EXIT_FAILURE);

    Equality* eq = _prove_stack[eq_num-1];

    instantiate_term(eq->t1, what, with);
    instantiate_term(eq->t2, what, with);
}

void Solver::apply_refl(int eq_num)
{
    Equality* eq = _prove_stack[eq_num-1];

    if (eq->t1->isEqual(eq->t2))
      _prove_stack.erase(_prove_stack.begin()+(eq_num-1));
}

void Solver::apply_sym(int eq_num)
{
    Equality* eq = _prove_stack[eq_num-1];
    Term* swap = eq->t1;
    eq->t1 = eq->t2;
    eq->t2 = swap;

}

void Solver::apply_cong(int eq_num)
{
    Equality* eq = _prove_stack[eq_num-1];
    Term* left = eq->t1;
    Term* right = eq->t2;

    if(left->type() == right->type() && right->type() == 1)
    {
      vector<Term*> lterm = ((Fn*)left)->args();
      vector<Term*> dterm = ((Fn*)right)->args();

      if (lterm.size() == dterm.size())
      {
        for (unsigned i = 0; i< lterm.size(); i++)
        {
          Equality * ins = new Equality(lterm[i], dterm[i]);
          _prove_stack.push_back(ins);
        }

        _prove_stack.erase(_prove_stack.begin()+(eq_num-1));
      }
    }
}


void Solver::quit()
{
    cout << "Exiting..."<< endl;
    exit(EXIT_SUCCESS);
}

void Solver::help()
{
  string help_menu = ":help\t-\tthis menu\n:q\t-\texit program\n";
  cout << help_menu << endl;
}

void Solver::print_status(Formula* f)
{
      //PRINT ELEMENATA
    vector<Equality*>::const_iterator it1= f->eqList->cbegin();
    vector<Equality*>::const_iterator it2= _prove_stack.cbegin();
    vector<Equality*>::const_iterator it3= f->eqList->cend();
    vector<Equality*>::const_iterator it4= _prove_stack.cend();

    for(;it1 < it3 - 1; it1++)
    {
      cout << "{" << *it1 << ", ";
    }
    cout << *it1 << "}" << " |- ";
    for(;it2 < it4 - 1; it2++)
    {
      cout << *it2 << ", ";
    }
    cout << *it2 << endl;
}

bool Solver::solve (Formula* f)
{
  _prove_stack.push_back(f->toProve);

  string line;
  while(1)
  {
    getline(cin, line);
    string pravilo;
    string arg_term, arg_term2;
    stringstream ss(line);
    int eq_num = 1;

    ss >> pravilo >> eq_num >>  arg_term >> arg_term2;
    if(!pravilo.compare("tran"))
    {
        apply_tran(eq_num, arg_term);
    }
    else if(!pravilo.compare("axiom"))
    {
        apply_axiom(f, eq_num);
    }
    else if(!pravilo.compare("inst"))
    {
        apply_inst(eq_num, arg_term, arg_term2);
    }
    else if(!pravilo.compare("refl"))
    {
        apply_refl(eq_num);
    }
    else if(!pravilo.compare("sym"))
    {
        apply_sym(eq_num);
    }
    else if(!pravilo.compare("cong"))
    {
        apply_cong(eq_num);
    }
    else if(!pravilo.compare(":q"))
    {
        quit();
    }
    else if (!pravilo.compare(":help"))
    {
        help();
    }
    else
    {
      cout << "Neposotjece pravilo, probaj ponovo ili ukucaj :help za pomoc" << endl;
    }

    pravilo.clear();
    line.clear();

    print_status(f);

    if (!_prove_stack.size())
    {
      cout<< "Uspesno reseno" << endl;
      break;
    }
  }

  return true;
}