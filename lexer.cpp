/*CS 280 101 Fall 2013
Tanner DeLuca
program 2*/

#include <string>
#include <vector>
#include <map>
using namespace std;

#include "token.h"

typedef enum State
{
	READING,
	GOTDEFINE,
	GOTDEFWORD,
	GOTIFDEF,
} State ;

int main (int argc, char *argv[])
{
	string infile = "" ;
	string outfile = "" ;

	if (argc == 2)
	{
		infile = argv[1] ;

		if (infile.substr(infile.length() - 2).compare(".c") == 0)
				outfile = infile.substr(0, infile.length() - 1) + "i" ;
		else
			exit(1) ;
	}
	else
		exit(1) ;

	ifstream in (infile) ;
	ofstream out (outfile) ;

	if (!in.is_open() || !out.is_open())
		exit(1) ;

	Lexer *L = new Lexer(in) ;
	
	Token *t ;

	State state = READING ;

	string defkey, defval ;
	map<string,string> defines ;

	int start_comment_line = -1 ;

	vector<bool> do_print ;
	do_print.push_back(true) ;

	vector<int> start_ifdef ;

	while (t = L->getToken())
	{
		switch (state)
		{
		case READING:
			if (t->getType() == TT_START_COMMENT)
			{
				do_print.push_back(false) ;
				start_comment_line = L->getLine() ;
			}
			else if (t->getType() == TT_END_COMMENT)
			{
				start_comment_line = -1 ;
				do_print.pop_back() ;
			}
			else if (t->getType() == TT_DEFINE)
			{
				state = GOTDEFINE ;
			}
			else if (t->getType() == TT_IFDEF)
			{
				state = GOTIFDEF ;
				start_ifdef.push_back(L->getLine()) ;
			}
			else if(t->getType() == TT_ENDIF)
			{
				if (!start_ifdef.empty())
					start_ifdef.pop_back() ;
				do_print.pop_back() ;
			}
			else if (do_print.back() && t->getType() != TT_LINE_COMMENT)
			{
				if (defines.count(t->getStr()) == 1)
					out << defines[defkey] << "\n" ;
				else
					out << t->getStr() << "\n" ;
			}

			break ;

		case GOTDEFINE:
			if (t->getType() != TT_WORD)
				state = READING ;
			else
			{
				state = GOTDEFWORD ;
				defkey = t->getStr() ;
			}

			break ;

		case GOTDEFWORD:
			defval = t->getStr() ;

			if (defines.count(defkey) == 0)
				defines[defkey] = defval ;
			else
				out << "\n?!?Duplicate definition of " << defkey << " at line " << L->getLine() << " ignored?!?\n" ;

			state = READING ;
			break ;

		case GOTIFDEF:
			if (t->getType() != TT_WORD)
			{
				state = READING ;
				break ;
			}
			else
			{
				defkey = t->getStr() ;

				if (defines.count(defkey) == 1)
					do_print.push_back(true) ;
				else
					do_print.push_back(false) ;
			}

			state = READING ;
			break ;
		}

		delete t ;
	}

	while (!start_ifdef.empty())
	{
		out << "\n?!?Missing #endif for #ifdef that began at line " << start_ifdef.back() << "?!?\n" ;
		start_ifdef.pop_back() ;
	}

	if (start_comment_line != -1)
		out << "\n?!?Unterminated comment that began at line " << start_comment_line << "?!?\n" ;
}