#include <iostream>
#include <fstream>
#include <string>
using namespace std ;

enum TokenType
{
	TT_WORD,
	TT_NUM,
	TT_CHAR,
	TT_QUOTE,
	TT_OPER,
	TT_DEFINE,
	TT_IFDEF,
	TT_ENDIF,
	TT_START_COMMENT,
	TT_END_COMMENT,
	TT_LINE_COMMENT,
};

class Token
{
private:
	TokenType t ;
	string v ;

public:
	Token (TokenType t, string v)
	{
		this->t = t ;
		this->v = v ;
	}

	TokenType getType() {return t ;}
	string getStr() {return v ;}
};

class Lexer
{
	int line ;
	istream *in ;
	Token *pushback ;

public:
	Lexer(ifstream& in) ;

	int doPushback(Token *t) ;
	Token *getToken() ;
	int getLine() {return line ;}
};