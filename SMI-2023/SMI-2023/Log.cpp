#include "stdafx.h"	
#include "Log.h"	
#include <time.h>

#pragma warning(disable:4996)
using namespace std;

namespace Log
{
	LOG getlog(wchar_t logfile[])
	{
		LOG log;
		log.stream = new ofstream;
		log.stream->open(logfile);
		if (log.stream->fail())
			throw ERROR_THROW(103);
		wcscpy_s(log.logfile, logfile);
		return log;
	}
	void WriteLine(LOG log, const char* c, ...)
	{
		const char** ptr = &c;
		int i = 0;
		while (ptr[i] != "")
			*log.stream << ptr[i++];
		*log.stream << endl;
	}
	void WriteLine(LOG log, const wchar_t* c, ...)
	{
		const wchar_t** ptr = &c;
		char temp[100];
		int i = 0;
		while (ptr[i] != L"")
		{
			wcstombs(temp, ptr[i++], sizeof(temp));
			*log.stream << temp;
		}
		*log.stream << endl;
	}
	void WriteLog(LOG log)
	{
		tm* timeinf;
		time_t timet;
		time(&timet);
		timeinf = localtime(&timet);
		char temp[100];
		strftime(temp, sizeof(temp), "%d.%m.%y, %T", timeinf);

		*log.stream << setfill('=') << setw(100) << '=' << endl;
		*log.stream << "\t\t\t\t�������� �� " << temp << endl;
		*log.stream << setfill('=') << setw(100) << '=' << endl;

		cout << setfill('=') << setw(100) << '=' << endl;
		cout << "\t\t\t\t�������� �� " << temp << endl;
		cout << setfill('=') << setw(100) << '=' << endl;
	}
	void WriteIn(LOG log, In::IN in)
	{
		*log.stream << "\t\t�������� ������" << endl;
		*log.stream << setfill('-') << setw(100) << '-' << endl;
		*log.stream << "���������� ��������:\t" << in.size << endl;
		*log.stream << "���������������:\t" << in.ignor << endl;
		*log.stream << "���������� �����:\t" << in.lines + 1 << endl;
		*log.stream << setfill('=') << setw(100) << '=' << endl;

		cout << "\t\t�������� ������" << endl;
		cout << setfill('-') << setw(100) << '-' << endl;
		cout << "���������� ��������:\t" << in.size << endl;
		cout << "���������������:\t" << in.ignor << endl;
		cout << "���������� �����:\t" << in.lines + 1 << endl;
		cout << setfill('=') << setw(100) << '=' << endl;
	}
	void WriteParm(LOG log, Parm::PARM parm)
	{
		char in_text[PARM_MAX_SIZE];
		char out_text[PARM_MAX_SIZE];
		char log_text[PARM_MAX_SIZE];
		wcstombs(in_text, parm.in, PARM_MAX_SIZE);
		wcstombs(out_text, parm.out, PARM_MAX_SIZE);
		wcstombs(log_text, parm.log, PARM_MAX_SIZE);

		*log.stream << "\t\t���������" << endl;
		*log.stream << setfill('-') << setw(100) << '-' << endl;
		*log.stream << "-in:\t" << in_text << "\n-out:\t" << out_text << "\n-log:\t" << log_text << endl;
		*log.stream << "�������������: " << (parm.idtable ? "-id  " : "") << (parm.lextable ? "-lt  " : "")
			<< (parm.poliz ? "-poliz  " : "") << endl;
		*log.stream << setfill('-') << setw(100) << '-' << endl;

		cout << "\t\t���������" << endl;
		cout << setfill('-') << setw(100) << '-' << endl;
		cout << "-in:\t" << in_text << "\n-out:\t" << out_text << "\n-log:\t" << log_text << endl;
		cout << "�������������: " << (parm.idtable ? "-id  " : "") << (parm.lextable ? "-lt  " : "")
			<< (parm.poliz ? "-poliz  " : "") << endl;
		cout << setfill('-') << setw(100) << '-' << endl;
	}
	void WriteError(LOG log, Error::ERROR error)
	{
		if (log.stream)
		{
			*log.stream << "������ " << error.id << ": " << error.message << "\n������ " << error.inext.line << " ������� " << error.inext.position << endl;
			Close(log);
		}
		else
		{
			cout << "������ " << error.id << ": " << error.message << "\n������ " << error.inext.line << " ������� " << error.inext.position << endl;
		}
	}
	void Close(LOG log)
	{
		log.stream->close();
		delete log.stream;
	}

	void WriteIDTableLog(IT::IdTable& idtable, LOG log)
	{
		int i, numberNoneId = 0;
		*log.stream << "\t\t������� ���������������" << endl;
		*log.stream << setfill('=') << setw(100) << '=' << endl;
		*log.stream << "   �" << " | " << "    �������������    " << " | " << "��� ������" << " | " << "��� ��������������" << " | " << "������ � ��" << " | " << "��������/���������" << endl;
		*log.stream << setfill('-') << setw(100) << '-' << endl;
		for (i = 0; i < idtable.size; i++)
		{
			*log.stream << setfill('0') << setw(4) << right << i << " | ";
			*log.stream << setfill(' ') << setw(21) << left << idtable.table[i].id << " | ";
			switch (idtable.table[i].iddatatype)
			{
			case IT::IDDATATYPE::NUM:
				*log.stream << setw(10) << left;
				*log.stream << "number" << " | ";
				break;
			case IT::IDDATATYPE::LINE:
				*log.stream << setw(10) << left;
				*log.stream << "line" << " | ";
				break;
			case IT::IDDATATYPE::PROC:
				*log.stream << setw(10) << left;
				*log.stream << "-" << " | ";
				break;
			case IT::IDDATATYPE::BOOL:
				*log.stream << setw(10) << left;
				*log.stream << "bool" << " | ";
				break;
			}
			switch (idtable.table[i].idtype)
			{
			case IT::IDTYPE::V:
				*log.stream << setw(18) << left << "����������" << " | ";
				break;
			case IT::IDTYPE::F:
				*log.stream << setw(18) << left << "�������" << " | ";
				break;
			case IT::IDTYPE::P:
				*log.stream << setw(18) << left << "��������" << " | ";
				break;
			case IT::IDTYPE::L:
				*log.stream << setw(18) << left << "�������" << " | ";
				numberNoneId++;
				break;
			case IT::IDTYPE::S: *log.stream << setw(18) << left << "������� ��. ����." << " | ";
				numberNoneId++;
				break;
			}
			*log.stream << setw(11) << left << idtable.table[i].idxfirstLE << " | ";

			if (idtable.table[i].iddatatype == IT::IDDATATYPE::NUM && (idtable.table[i].idtype == IT::IDTYPE::V || idtable.table[i].idtype == IT::IDTYPE::L))
				*log.stream << setw(18) << left << idtable.table[i].value.vint;
			else if (idtable.table[i].iddatatype == IT::IDDATATYPE::BOOL && (idtable.table[i].idtype == IT::IDTYPE::V || idtable.table[i].idtype == IT::IDTYPE::L))
			{
				if (idtable.table[i].value.vint == 0)
					*log.stream << setw(18) << left << "false";
				else
					*log.stream << setw(18) << left << "true";
			}
			else if (idtable.table[i].iddatatype == IT::IDDATATYPE::LINE && (idtable.table[i].idtype == IT::IDTYPE::V || idtable.table[i].idtype == IT::IDTYPE::L))
				*log.stream << "[" << idtable.table[i].value.vstr.len << "]\"" << idtable.table[i].value.vstr.str << "\"";
			else if (idtable.table[i].idtype == IT::IDTYPE::F || idtable.table[i].idtype == IT::IDTYPE::S)
			{
				for (int k = 0; k < idtable.table[i].value.params.count; k++)
				{
					*log.stream << left << "P" << k << ":";
					switch (idtable.table[i].value.params.types[k])
					{
					case IT::IDDATATYPE::NUM:
						*log.stream << left << "N|"; break;
					case IT::IDDATATYPE::LINE:
						*log.stream << left << "L|"; break;
					case IT::IDDATATYPE::BOOL:
						*log.stream << left << "B|"; break;
					}
				}
				if (idtable.table[i].value.params.count == NULL)
					*log.stream << "-";
			}
			else
				*log.stream << "-";
			*log.stream << endl;
		}
		*log.stream << setfill('-') << setw(100) << '-' << endl;
		*log.stream << "\t����� ���������������: " << i - numberNoneId << endl;
		*log.stream << setfill('=') << setw(100) << '=' << endl;
	}

	void WriteLexTableLog(LT::LexTable& lextable, LOG log)
	{
		int i;
		*log.stream << "\t\t������� ������" << endl;
		*log.stream << setfill('=') << setw(100) << '=' << endl;
		*log.stream << "   �" << " | " << "�������" << setfill(' ') << setw(20) << ' ' << left << " | " << "������" << setw(21) << ' ' << left << " | " << "������ � ��" << endl;
		*log.stream << setfill('-') << setw(100) << '-' << endl;
		for (i = 0; i < lextable.size; i++)
		{
			*log.stream << setfill('0') << setw(4) << right << i << " | " << setfill(' ') << setw(24) << left << lextable.table[i].lexema << "    | " << setw(24) << left << lextable.table[i].line + 1 << "    | ";
			if (lextable.table[i].idxTI == LT_TI_NULLIDX)
				*log.stream << "-" << endl;
			else
				*log.stream << setw(23) << lextable.table[i].idxTI << endl;
		}
		*log.stream << setfill('-') << setw(90) << '-' << endl;
		*log.stream << "\t����� ������: " << i << endl;
		*log.stream << setfill('=') << setw(90) << '=' << endl;
	}

	void WritePolizLog(LT::LexTable& lextable, LOG log)
	{
		int numberOfLine = 1;
		int number = 0;
		*log.stream << "\t\t������������� ������������� ����" << endl;
		*log.stream << setfill('=') << setw(90) << '=';
		for (int i = 0; i < lextable.size; i++)
		{
			if (lextable.table[i].line != number)
			{
				while (lextable.table[i].line - number > 1)
					number++;
				*log.stream << endl << setfill('0') << setw(4) << right << numberOfLine << "\t";
				number++;
				numberOfLine++;
			}
			*log.stream << lextable.table[i].lexema;
			if (lextable.table[i].lexema == LEX_ID || lextable.table[i].lexema == LEX_LITERAL)
				*log.stream << "(" << lextable.table[i].idxTI << ")";
		}
		*log.stream << endl;
		*log.stream << setfill('=') << setw(90) << '=' << endl;
	}
}