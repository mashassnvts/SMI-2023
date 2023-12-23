#include "stdafx.h"
#include "Polish.h"


namespace Poliz
{
	bool Poliz(int i, LexAnalisys::LA& lex)
	{
		std::stack<LT::Entry> stack;
		std::queue<LT::Entry> queue;

		LT::Entry temp;
		temp.idxTI = LT_TI_NULLIDX;
		temp.lexema = '*';
		temp.line = lex.lextable.table[i].line;

		LT::Entry func;
		func.idxTI = LT_TI_NULLIDX;
		func.lexema = '@';
		func.line = lex.lextable.table[i].line;

		int countLex = 0, countComma = 0;
		int countParm = 1;
		int posLex = i;
		char* buf = new char[1];

		bool findComma = false;
		bool findParm = false;
		bool result = true;

		for (i; lex.lextable.table[i].lexema != LEX_SEMICOLON; i++, countLex++)
		{
			switch (lex.lextable.table[i].lexema)
			{
			case LEX_ID:
			{
				queue.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_CONCAT:
			{
				queue.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_LINELEN:
			{
				queue.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_RANDOM:
			{
				queue.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_SQRT:
			{
				queue.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_LITERAL:
			{
				queue.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_OPERATOR:
			{
				while (!stack.empty() && lex.lextable.table[i].priority <= stack.top().priority)
				{
					queue.push(stack.top());
					stack.pop();
				}
				stack.push(lex.lextable.table[i]);
				continue;
			}
			
			case LEX_LOG_OPERATOR:
			{
				while (!stack.empty() && lex.lextable.table[i].priority <= stack.top().priority)
				{
					queue.push(stack.top());
					stack.pop();
				}
				stack.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_COMMA:
			{
				findComma = true;
				countComma++;
				continue;
			}
			case LEX_LEFTBRACKET:
			{
				stack.push(lex.lextable.table[i]);
				continue;
			}

			case LEX_RIGHTBRACKET:
			{
				while (stack.top().lexema != LEX_LEFTBRACKET)
				{
					queue.push(stack.top());
					stack.pop();
					if (stack.empty())
						return false;
				}
				stack.pop();
				continue;
			}
			case LEX_LEFTTHESIS:
			{
				stack.push(lex.lextable.table[i]);
				if (lex.lextable.table[i + 1].lexema == LEX_ID || lex.lextable.table[i + 1].lexema == LEX_LITERAL)
					findParm = true;
				continue;
			}
			case LEX_RIGHTTHESIS:
			{
				while (stack.top().lexema != LEX_LEFTTHESIS)
				{
					queue.push(stack.top());
					stack.pop();
					if (stack.empty())
						return false;
				}
				if (findComma)
					countParm = countComma + 1;
				else if (findParm)
					countParm = 1;
				else
					countParm = 0;
				countComma = 0;
				lex.lextable.table[i] = func;
				queue.push(lex.lextable.table[i]);
				_itoa_s(countParm, buf, 2, 10);
				stack.top().lexema = buf[0];
				stack.top().idxTI = func.idxTI;
				stack.top().line = func.line;
				queue.push(stack.top());
				stack.pop();
				continue;
			}
			}
		}

		while (!stack.empty())
		{
			if (stack.top().lexema == LEX_LEFTTHESIS || stack.top().lexema == LEX_RIGHTTHESIS || stack.top().lexema == LEX_LEFTBRACKET || stack.top().lexema == LEX_RIGHTBRACKET)
				return false;
			queue.push(stack.top());
			stack.pop();
		}

		while (countLex)
		{
			if (!queue.empty())
			{
				lex.lextable.table[posLex++] = queue.front();
				queue.pop();
			}
			else
			{
				lex.lextable.table[posLex++] = temp;
			}
			countLex--;
		}

		for (int i = 0; i < posLex; i++)
		{
			if (lex.lextable.table[i].lexema == LEX_LITERAL || lex.lextable.table[i].lexema == LEX_ID)
				lex.idtable.table[lex.lextable.table[i].idxTI].idxfirstLE = i;
		}
		return true;
	}

	bool StartPoliz(LexAnalisys::LA& lex)
	{
		bool result = false;
		for (int i = 0; i < lex.lextable.size; i++)
		{
			if (lex.lextable.table[i].lexema == LEX_EQUAL ||
				(lex.lextable.table[i].lexema == LEX_ID && lex.lextable.table[i - 1].lexema != LEX_EQUAL
					&& lex.lextable.table[i - 1].lexema != LEX_COUNT_PARM0 && lex.lextable.table[i - 1].lexema != LEX_COUNT_PARM1
					&& lex.lextable.table[i - 1].lexema != LEX_COUNT_PARM2 && lex.lextable.table[i - 1].lexema != LEX_COUNT_PARM3
					&& lex.lextable.table[i - 1].lexema != LEX_OPERATOR && lex.lextable.table[i - 1].lexema != LEX_LOG_OPERATOR
					&& lex.idtable.table[lex.lextable.table[i].idxTI].idtype == IT::IDTYPE::F
					&& lex.lextable.table[i - 1].lexema != LEX_PROCEDURE && lex.lextable.table[i - 2].lexema != LEX_FUNCTION))
			{
				result = Poliz(i + 1, lex);
				if (!result)
					return result;
			}
		}
		return result;
	}
}