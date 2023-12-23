#include "stdafx.h"
#include "IT.h"
#include "Error.h"
#include <iomanip>

namespace IT
{
	IdTable Create(int size)
	{
		IdTable* idtable = new IdTable;
		if (size > TI_MAXSIZE)
			throw ERROR_THROW(203);
		idtable->maxsize = size;
		idtable->size = 0;
		idtable->table = new Entry[size];
		return*idtable;
	}

	void Add(IdTable& idtable, Entry entry)
	{
		if (idtable.size + 1 > idtable.maxsize)
			throw ERROR_THROW(204);

		idtable.table[idtable.size++] = entry;
	}

	Entry GetEntry(IdTable& idtable, int n)
	{
		return idtable.table[n];
	}

	int IsId(IdTable& idtable, unsigned char id[ID_MAXSIZE])
	{
		for (int i = 0; i < idtable.size; i++)
		{
			if (strcmp((const char*)idtable.table[i].id, (const char*)id) == 0)
				return i;
		}
		return TI_NULLIDX;
	}

	void Delete(IdTable& idtable)
	{
		delete[] idtable.table;
	}

	void WriteTable(IdTable& idtable)
	{
		int i, numberNoneId = 0;
		cout << "\t\tТАБЛИЦА ИДЕНТИФИКАТОРОВ" << endl;
		cout << setfill('=') << setw(100) << '=' << endl;
		cout << "   №" << " | " << "    Идентификатор    " << " | " << "Тип данных" << " | " << "Тип идентификатора" << " | " << "Индекс в ТЛ" << " | " << "Значение/Параметры" << endl;
		cout << setfill('-') << setw(100) << '-' << endl;
		for (i = 0; i < idtable.size; i++)
		{
			cout << setfill('0') << setw(4) << right << i << " | ";
			cout << setfill(' ') << setw(21) << left << idtable.table[i].id << " | ";

			switch (idtable.table[i].iddatatype)
			{
			case IT::IDDATATYPE::NUM:
				cout << setw(10) << left;
				cout << "number" << " | ";
				break;
			case IT::IDDATATYPE::LINE:
				cout << setw(10) << left;
				cout << "line" << " | ";
				break;
			case IT::IDDATATYPE::PROC:
				cout << setw(10) << left;
				cout << "-" << " | ";
				break;
			case IT::IDDATATYPE::BOOL:
				cout << std::setw(10) << left;
				cout << "bool" << " | ";
				break;
			}

			switch (idtable.table[i].idtype)
			{
			case IT::IDTYPE::V:
				cout << setw(18) << left << "переменная" << " | ";
				break;
			case IT::IDTYPE::F:
				cout << setw(18) << left << "функция" << " | ";
				break;
			case IT::IDTYPE::P:
				cout << setw(18) << left << "параметр" << " | ";
				break;
			case IT::IDTYPE::L:
				cout << setw(18) << left << "литерал" << " | ";
				numberNoneId++;
				break;
			case IT::IDTYPE::S:
				cout << setw(18) << left << "функция ст. библ." << " | ";
				numberNoneId++;
				break;
			default:
				cout << setw(18) << left << "неизвестен" << " | "; break;
			}

			cout << setw(11) << left << idtable.table[i].idxfirstLE << " | ";

			if (idtable.table[i].iddatatype == IT::IDDATATYPE::NUM && (idtable.table[i].idtype == IT::IDTYPE::V || idtable.table[i].idtype == IT::IDTYPE::L))
				cout << setw(18) << left << idtable.table[i].value.vint;
			else if (idtable.table[i].iddatatype == IT::IDDATATYPE::BOOL && (idtable.table[i].idtype == IT::IDTYPE::V || idtable.table[i].idtype == IT::IDTYPE::L))
			{
				if (idtable.table[i].value.vint == 0)
					cout << setw(18) << left << "false";
				else
					cout << setw(18) << left << "true";
			}
			else if (idtable.table[i].iddatatype == IT::IDDATATYPE::LINE && (idtable.table[i].idtype == IT::IDTYPE::V || idtable.table[i].idtype == IT::IDTYPE::L))
				cout << "[" << idtable.table[i].value.vstr.len << "]\"" << idtable.table[i].value.vstr.str << "\"";
			else if (idtable.table[i].idtype == IT::IDTYPE::F || idtable.table[i].idtype == IT::IDTYPE::S)
			{
				for (int k = 0; k < idtable.table[i].value.params.count; k++)
				{
					cout << left << "P" << k << ":";
					switch (idtable.table[i].value.params.types[k])
					{
					case IT::IDDATATYPE::NUM:
						cout << left << "N|"; break;
					case IT::IDDATATYPE::LINE:
						cout << left << "L|"; break;
					case IT::IDDATATYPE::BOOL:
						cout << left << "B|"; break;
					}
				}
				if (idtable.table[i].value.params.count == NULL)
					cout << "-";
			}
			else
				cout << "-";
			cout << endl;
		}
		cout << setfill('-') << setw(100) << '-' << endl;
		cout << "\tВсего идентификаторов: " << i - numberNoneId << endl;
		cout << setfill('=') << setw(100) << '=' << endl;
	}
};