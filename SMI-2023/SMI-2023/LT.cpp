#include "LT.h"
#include "stdafx.h"
#include "Error.h"
#include "Parm.h"
#include "Log.h"
using namespace Log;
using namespace std;

namespace LT
{
	Entry writeEntry(Entry& entry, unsigned char lexema, int indx, int line, int priority, OPER operation)
	{
		entry.lexema = lexema;
		entry.idxTI = indx;
		entry.line = line;
		entry.priority = priority;
		entry.operation = operation;
		return entry;
	}

	LexTable Create(int size)
	{
		if (size > LT_MAXSIZE)
			throw ERROR_THROW(201);
		LexTable lexTable;
		lexTable.maxsize = size;
		lexTable.size = NULL;
		lexTable.table = new Entry[size];
		return lexTable;
	}

	void Add(LexTable& lexTable, Entry entry)
	{
		if (lexTable.size >= lexTable.maxsize)
			throw ERROR_THROW(202);
		lexTable.table[lexTable.size++] = entry;
	}

	Entry GetEntry(LexTable& lexTable, int n)
	{
		return lexTable.table[n];
	}

	void Delete(LexTable& lexTable)
	{
		delete[] lexTable.table;
	}

	void WritePoliz(LexTable lextable)
	{
		int numberOfLine = 1;
		int number = 0;
		cout << "\t\tÏÐÎÌÅÆÓÒÎ×ÍÎÅ ÏÐÅÄÑÒÀÂËÅÍÈÅ ÊÎÄÀ";
		for (int i = 0; i < lextable.size; i++)
		{
			if (lextable.table[i].line != number && lextable.table[i].line != 0)
			{
				while (lextable.table[i].line - number > 1)
					number++;

				cout << endl << setfill('0') << setw(4) << right << numberOfLine << "\t";
				number++;
				numberOfLine++;
			}
			cout << lextable.table[i].lexema;
			if (lextable.table[i].lexema == LEX_ID || lextable.table[i].lexema == LEX_LITERAL)
				cout << "(" << lextable.table[i].idxTI << ")";
		}
		cout << endl;
		cout << setfill('=') << setw(100) << '=' << endl;
	}

	void WriteLexTable(LT::LexTable& lextable)
	{
		int i;
		cout << "\t\tÒÀÁËÈÖÀ ËÅÊÑÅÌ" << endl;
		cout << setfill('=') << setw(100) << '=' << endl;
		cout << "   ¹" << " | " << "Ëåêñåìà" << setfill(' ') << setw(20) << ' ' << left << " | " << "Ñòðîêà" << setw(21) << ' ' << left << " | " << "Èíäåêñ â ÒÈ" << endl;
		cout << setfill('-') << setw(100) << '-' << endl;
		for (i = 0; i < lextable.size; i++)
		{
			cout << setfill('0') << setw(4) << right << i << " | " << setfill(' ') << setw(24) << left << lextable.table[i].lexema << "    | " << setw(24) << left
				<< lextable.table[i].line + 1 << "    | ";
			if (lextable.table[i].idxTI == LT_TI_NULLIDX)
				cout << "-" << endl;
			else
				cout << setw(23) << lextable.table[i].idxTI << endl;
		}
		cout << setfill('-') << setw(100) << '-' << endl;
		cout << "\tÂñåãî ëåêñåì: " << i << endl;
		cout << setfill('=') << setw(100) << '=' << endl;
	}
}