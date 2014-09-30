#include "token.h"

#include <ctype.h>

typedef enum Lexstate
{
	START,
	WORD,
	NUM,
	CHAR,
	QUOTE,
	OP_OR_PUNCT,
	POUND,
	COMMENT,
	LINE_COMMENT,
} Lexstate ;

Lexer::Lexer(ifstream& in)
{
	line = 1 ;
	this->in = &in ;
	this->pushback = 0 ;
}

int Lexer::doPushback(Token *t)
{
	if (this->pushback != 0)
		return 1 ;

	this->pushback = t ;

	return 0 ;
}

Token * Lexer::getToken()
{
	Lexstate lexstate = START ;

	if (this->pushback)
	{
		Token *rv = this->pushback ;
		this->pushback = 0 ;

		return rv ;
	}

	string collected = "" ;
	Token *t ;

	while(in->good())
	{
		char ch ;
		in->get(ch) ;

		switch (lexstate)
		{
		case START:
			if (isspace(ch))
			{
				if (ch == '\n')
					line++ ;
				break ;
			}

			collected = ch ;

			if (isalpha(ch))
				lexstate = WORD ;
			else if (isdigit(ch))
				lexstate = NUM ;
			else if (ch == '\'')
				lexstate = CHAR ;
			else if (ch == '\"')
				lexstate = QUOTE ;
			else if (ch == '#')
				lexstate = POUND ;
			else if (ch == '/' || ch == '*')
				lexstate = COMMENT ;
			else
				lexstate = OP_OR_PUNCT ;

			break ;

		case WORD:
			if (isalpha(ch))
			{
				collected += ch ;
				break ;
			}

			in->putback(ch) ;
			t = new Token(TT_WORD, collected) ;
			return t ;

		case NUM:
			if (isdigit(ch))
			{
				collected += ch ;
				break ;
			}

			in->putback(ch) ;
			t = new Token(TT_NUM, collected) ;
			return t ;

		case CHAR:
			if (ch == '\n')
				line++ ;

			collected += ch ;

			if (ch == '\'')
			{
				t = new Token(TT_CHAR, collected) ;
				return t ;
			}

			break ;

		case QUOTE:
			if (ch == '\n')
				line++ ;

			collected += ch ;

			if (ch == '\"')
			{
				t = new Token(TT_QUOTE, collected) ;
				return t ;
			}

			break ;

		case POUND:
			if (isalpha(ch))
			{
				collected += ch ;
				break ;
			}

			in->putback(ch) ;
			
			if (collected.compare("#define") == 0)
			{
				t = new Token(TT_DEFINE, collected) ;
				return t ;
			}
			else if (collected.compare("#ifdef") == 0)
			{
				t = new Token(TT_IFDEF, collected) ;
				return t ;
			}
			else if (collected.compare("#endif") == 0)
			{
				t = new Token(TT_ENDIF, collected) ;
				return t ;
			}

			t = new Token(TT_WORD, collected) ;
			return t ;

		case COMMENT:
			if (collected.compare("*") == 0)
			{
				if (ch == '/')
				{
					t = new Token(TT_END_COMMENT, collected) ;
					return t ;
				}
			}
			else if (ch == '*')
			{
				t = new Token(TT_START_COMMENT, collected) ;
				return t ;
			}
			else if (ch == '/')
			{
				lexstate = LINE_COMMENT ;
				break ;
			}
			
			if (ch == '\n')
				line++ ;

			in->putback(ch) ;
			lexstate = OP_OR_PUNCT ;
			break ;
			
		case LINE_COMMENT:
			if (ch == '\n')
			{
				line++ ;
				t = new Token(TT_LINE_COMMENT, collected) ;
				return t ;
			}

			break ;

		case OP_OR_PUNCT:
			if (isspace(ch) || isalnum(ch) || ch == '\'' || ch == '\"' || ch == '/')
			{
				in->putback(ch) ;

				t = new Token(TT_OPER, collected) ;
				return t ;
			}

			collected += ch ;
			break ;
		}
	}

	return 0 ;
}