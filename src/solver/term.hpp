
#ifndef _TERM_
#define _TERM_

#include <iostream>
#include <vector>
#include <iterator>


class Term
{
public:
  Term() {}
  enum Type {T_VAR = 0, T_FN = 1};
  virtual Type type(void) const = 0 ;
  virtual bool isEqual(Term const * t) const = 0 ;

};

class TermCopy
{
public:
  static Term* makeCopy(Term* term);
};


class Var : public Term
{
public:
  Var(const std::string & name);
  Var(Var* var);

  Type type(void) const override;
  const std::string & name(void) const;
  Var & name(const std::string & name);

  bool isEqual(Term const * t) const override;

private:
    std::string _name;
};

class Fn : public Term
{
public:
  Fn(const std::string & name, std::initializer_list<Term *> args);
  Fn(const std::string & name, std::vector<Term *> & args);
  Fn(Fn*);

  Type type(void) const override;
  const std::string & name(void) const;
  Fn & name(const std::string & name);
  std::vector<Term*> args(void);

  bool isEqual(Term const * t) const override;
  unsigned arity(void) const;

  friend std::ostream & operator << (std::ostream & o, Term * t);

private:
    std::string _name;
    std::vector<Term *> _args;
};

Term * makeConst(const std::string & value);
Term * makeConst(int value);




struct Equality{
  Equality(Term *, Term *);
  Term * t1;
  Term * t2;

  bool isEqual(Equality* eq) const;
};

struct Formula{
  Formula(std::vector<Equality*> * eqL, Equality * eq);
  std::vector<Equality*> * eqList;
  Equality * toProve;

  bool findEquality(Equality* eq) const;
};



std::ostream & operator << (std::ostream & o, Term * t);
std::ostream & operator << (std::ostream & o, Equality * e);
std::ostream & operator << (std::ostream & o, Formula * f);










#endif
