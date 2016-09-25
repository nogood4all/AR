#include <iostream>
#include <vector>
#include "term.hpp"
#include <sstream>
#include <cstdlib>
using namespace std;

void instantiate_term(Term*, Term*, Term*);
Formula * parseFormula(const char *str_input);
Term * parseTerm(const char *str_input);

string help_menu = ":help\t-\tthis menu\n:q\t-\texit program\n";

vector<Equality*> desna_strana;

int main(){
	string formula;
	getline(cin, formula);
  Formula  * f = parseFormula(formula.c_str());
  desna_strana.push_back(f->toProve);

  string linija;
  while(1)
  {
  	getline(cin, linija);
  	string pravilo;
  	string argTerm, argTerm2;
  	stringstream ss(linija);
  	int jednakost = 1;
  	ss >> pravilo >> jednakost >>  argTerm >> argTerm2;
  	if(!pravilo.compare("tran"))
  	{
				//cout << pravilo << " " << jednakost << endl;
  			Term* arg = parseTerm(argTerm.c_str());
  			Equality* eq = desna_strana[jednakost-1];
  			
  			if (!arg)
  				exit(EXIT_FAILURE);

  			Equality* eq1 = new Equality(eq->t1, arg);
  			Equality* eq2 = new Equality(arg, eq->t2);
  			
  			desna_strana.erase(desna_strana.begin()+(jednakost-1));
  			desna_strana.push_back(eq1);
  			desna_strana.push_back(eq2);
		}
		else if(!pravilo.compare("axiom"))
		{
				//ako imamo jednakost sa leve i desne strane, izbaci sa desne pod datim indeksom
				if (f->findEquality(desna_strana[jednakost-1]))
					desna_strana.erase(desna_strana.begin()+(jednakost-1));

		}
		else if(!pravilo.compare("inst"))
		{
				//menjamo term za term
				Term* what = parseTerm(argTerm.c_str());
				Term* with = parseTerm(argTerm2.c_str());

				if(!with || !what)
					exit(EXIT_FAILURE);

				Equality* eq = desna_strana[jednakost-1];

				instantiate_term(eq->t1, what, with);
				instantiate_term(eq->t2, what, with);

		}
		else if(!pravilo.compare("refl"))
		{
				//Ako nam je jednakost ima istu levu i desnu stranu brisemo je sa liste desna_strana
				Equality* eq = desna_strana[jednakost-1];

				if (eq->t1->isEqual(eq->t2))
					desna_strana.erase(desna_strana.begin()+(jednakost-1));

		}
		else if(!pravilo.compare("sym"))
		{
				//obrcemo termove u zadatoj jednakosti
				Equality* eq = desna_strana[jednakost-1];
				Term* swap = eq->t1;
				eq->t1 = eq->t2;
				eq->t2 = swap;

		}
		else if(!pravilo.compare("cong"))
		{
				//razbijamo funkciju na jednakost argumenata;
				Equality* eq = desna_strana[jednakost-1];
				Term* levi = eq->t1;
				Term* desni = eq->t2;


				if(levi->type() == desni->type() && desni->type() == 1)
				{
					vector<Term*> lterm = ((Fn*)levi)->args();
					vector<Term*> dterm = ((Fn*)desni)->args();
					
					if (lterm.size() == dterm.size())	
					{
						for (unsigned i = 0; i< lterm.size(); i++)
						{
							Equality * ins = new Equality(lterm[i], dterm[i]);
							desna_strana.push_back(ins);	
						}

						desna_strana.erase(desna_strana.begin()+(jednakost-1));
					}
				}


		}
		else if(!pravilo.compare(":q"))
		{
				cout << "Exiting..."<< endl;
				exit(EXIT_SUCCESS);
		}
		else if (!pravilo.compare(":help"))
		{
				cout << help_menu << endl;
		}
		else
		{
			cout << "Neposotjece pravilo, probaj ponovo ili ukucaj :help za pomoc" << endl;
		}

		pravilo.clear();
		linija.clear();

		//TODO
		//PRINT ELEMENATA
		vector<Equality*>::const_iterator it1= f->eqList->cbegin();
		vector<Equality*>::const_iterator it2= desna_strana.cbegin();
		vector<Equality*>::const_iterator it3= f->eqList->cend();
		vector<Equality*>::const_iterator it4= desna_strana.cend();

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
		if (!desna_strana.size())
		{
			cout<< "Uspesno reseno" << endl;
			break;
		}
	}
			
  return 0;

}



void instantiate_term(Term* term, Term* what, Term* with)
{

	//Uraditi sa dubokim copy konstruktorima i napraviti duboke copy konstruktore
	if(term->type() == what->type())
	{
		if(term->isEqual(what))
			term = with;

	}
	else if (term->type() == 0) // if var or const
	{
		term = with;
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