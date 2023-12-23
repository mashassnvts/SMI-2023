#include "stdafx.h"
#include "Generation.h"


namespace GenerateAsm
{
	void Generation(LT::LexTable lextable, IT::IdTable idtable, wchar_t outfile[])
	{
		ofstream out(outfile);
		Head(&out);
		ConstSegment(idtable, &out);
		DataSegment(lextable, idtable, &out);
		CodeSegment(lextable, idtable, &out);
	}

	void Head(ofstream* out)
	{
		*out << ".586P\n";
		*out << ".model flat, stdcall\n";

		*out << "includelib libucrt.lib\n";
		*out << "includelib kernel32.lib\n";
		*out << "includelib ..\\Debug\\Library.lib\n";
		*out << "ExitProcess PROTO :DWORD\n\n";

		*out << "EXTRN CONCAT: proc\n";
		*out << "EXTRN LINELEN: proc\n";
		*out << "EXTRN RANDOM: proc\n";
		*out << "EXTRN SQRT: proc\n";
		*out << "EXTRN OutNumber: proc\n";
		*out << "EXTRN OutLine: proc\n";
		*out << "EXTRN OutBool: proc\n";
		*out << "EXTRN OutNumberLn: proc\n";
		*out << "EXTRN OutLineLn: proc\n";
		*out << "EXTRN OutBoolLn: proc\n";
		*out << "\n.stack 4096\n\n";
	}

	void ConstSegment(IT::IdTable idtable, ofstream* out)
	{
		*out << ".const\n";
		*out << "\tnull_division BYTE \"Exception: деление на ноль\", 0\n";
		for (int i = 0; i < idtable.size; i++)
		{
			if (idtable.table[i].idtype == IT::IDTYPE::L)
			{
				*out << "\t" << idtable.table[i].id;
				if (idtable.table[i].iddatatype == IT::IDDATATYPE::LINE)
					*out << "\tBYTE \"" << idtable.table[i].value.vstr.str << "\", 0";
				if (idtable.table[i].iddatatype == IT::IDDATATYPE::NUM || idtable.table[i].iddatatype == IT::IDDATATYPE::BOOL)
					*out << "\tSWORD " << idtable.table[i].value.vint;
				*out << '\n';
			}
		}
	}

	void DataSegment(LT::LexTable lextable, IT::IdTable idtable, ofstream* out)
	{
		*out << "\n.data\n";
		*out << "\tbuffer BYTE 256 dup(0)\n";
		for (int i = 0; i < lextable.size; i++)
		{
			if (lextable.table[i].lexema == LEX_DEFINE)
			{
				if (idtable.table[lextable.table[i + 2].idxTI].idtype == IT::IDTYPE::V)
				{
					*out << "\t" << idtable.table[lextable.table[i + 2].idxTI].id;
					if (idtable.table[lextable.table[i + 2].idxTI].iddatatype == IT::IDDATATYPE::LINE)
					{
						*out << "\tDWORD ?\n";
					}
					if (idtable.table[lextable.table[i + 2].idxTI].iddatatype == IT::IDDATATYPE::NUM || idtable.table[lextable.table[i + 2].idxTI].iddatatype == IT::IDDATATYPE::BOOL)
					{
						*out << "\tSWORD 0\n";
					}
				}
			}
		}
	}

	void CodeSegment(LT::LexTable lextable, IT::IdTable idtable, ofstream* out)
	{
		stack<IT::Entry> stk;
		queue<unsigned char*> queue;
		int numOfPoints = 0, numOfLogExpr = 0, numOfRet = 0,
			numOfEnds = 0, numOfCycles = 0, countParm = 0, numOfLogPoints = 0;

		string strret = string(), cycleCode = string(), funcName = string();

		bool flagFunc = false, flagRet = false, flagBody = false, flagWhen = false,
			flag_then = false, flagOtherwise = false, flagLinelen = false, flagRand = false,
			flagSqrt = false, flagProc = false, flagConcat = false, flagCallfunc = false,
			flagCondition = false, flagCycle = false;

		*out << "\n.code\n\n";
		for (int i = 0; i < lextable.size; i++)
		{
			switch (lextable.table[i].lexema)
			{
			case LEX_PROCEDURE:
			case LEX_FUNCTION:
			{
				while (lextable.table[i].lexema != LEX_RIGHTTHESIS)
				{
					if (lextable.table[i].lexema == LEX_ID && idtable.table[lextable.table[i].idxTI].idtype == IT::IDTYPE::F)
					{
						if (idtable.table[lextable.table[i].idxTI].iddatatype == IT::IDDATATYPE::PROC)
							flagProc = true;
						*out << (funcName = string((char*)idtable.table[lextable.table[i].idxTI].id)) << " PROC ";
					}
					if (lextable.table[i].lexema == LEX_ID && idtable.table[lextable.table[i].idxTI].idtype == IT::IDTYPE::P)
					{
						*out << idtable.table[lextable.table[i].idxTI].id << " : ";
						if (idtable.table[lextable.table[i].idxTI].iddatatype == IT::IDDATATYPE::NUM)
						{
							*out << "SWORD";
						}
						else if (idtable.table[lextable.table[i].idxTI].iddatatype == IT::IDDATATYPE::BOOL)
						{
							*out << "SWORD";
							queue.push(idtable.table[lextable.table[i].idxTI].id);
						}
						else
						{
							*out << "DWORD";
						}
					}
					if (lextable.table[i].lexema == LEX_COMMA)
					{
						*out << ", ";
					}
					i++;
				}
				while (!queue.empty())
				{
					*out << "\n\tmov ax, " << queue.front() << "\n";
					*out << "\tcmp ax, 0\n";
					*out << "\tje l" << numOfLogPoints << "\n";
					*out << "\tjne l" << numOfLogPoints + 1 << "\n";

					*out << "l" << numOfLogPoints << ":\n";
					*out << "\tmov ax, 0\n\tpush ax\n\tjmp endofexpr" << numOfLogExpr << "\n";

					*out << "l" << numOfLogPoints + 1 << ":\n";
					*out << "\tmov ax, 1\n\tpush ax\n\n";

					*out << "endofexpr" << numOfLogExpr << ":\n";
					numOfLogExpr++;
					numOfLogPoints += 2;
					*out << "\tpop " << queue.front() << "\n";
					queue.pop();
				}
				flagFunc = true;
				*out << "\n";
				break;
			}
			case LEX_PROGRAM:
			{
				flagBody = true;
				*out << "program PROC\n";
				break;
			}
			case LEX_EQUAL:
			{
				int result_position = i - 1;
				while (lextable.table[i].lexema != LEX_SEMICOLON)
				{
					switch (lextable.table[i].lexema)
					{
					case LEX_ID:
					case LEX_LITERAL:
					{
						if (idtable.table[lextable.table[i].idxTI].idtype == IT::IDTYPE::F)
						{
							flagCallfunc = true;
						}
						else
						{
							if (idtable.table[lextable.table[i].idxTI].iddatatype == IT::IDDATATYPE::NUM || idtable.table[lextable.table[i].idxTI].iddatatype == IT::IDDATATYPE::BOOL)
							{
								*out << "\tpush " << idtable.table[lextable.table[i].idxTI].id << "\n";
								stk.push(idtable.table[lextable.table[i].idxTI]);
								break;
							}
							if (idtable.table[lextable.table[i].idxTI].iddatatype == IT::IDDATATYPE::LINE)
							{
								if (idtable.table[lextable.table[i].idxTI].idtype == IT::IDTYPE::L)
								{
									*out << "\tpush offset " << idtable.table[lextable.table[i].idxTI].id << "\n";
								}
								else
								{
									*out << "\tpush " << idtable.table[lextable.table[i].idxTI].id << "\n";
								}
								stk.push(idtable.table[lextable.table[i].idxTI]);
								break;
							}
						}
					}
					case LEX_OPERATOR:
					{
						switch (lextable.table[i].operation)
						{
						case LT::OPER::MUL:
						{
							*out << "\tpop ax\n\tpop bx\n";
							*out << "\tmul bx\n\tpush ax\n";
							break;
						}
						case LT::OPER::PLUS:
						{
							*out << "\tpop ax\n\tpop bx\n";
							*out << "\tadd ax, bx\n\tpush ax\n";
							break;
						}
						case LT::OPER::MINUS:
						{
							*out << "\tpop bx\n\tpop ax\n";
							*out << "\tsub ax, bx\n\tpush ax\n";
							break;
						}
						case LT::OPER::DIV:
						{
							*out << "\tpop bx\n\tpop ax\n";
							*out << "\tcmp bx, 0\n\tje nulldiv\n";
							*out << "\tcwd\n\tidiv bx\n\tpush ax\n";
							break;
						}
						case LT::OPER::REST:
						{
							*out << "\tpop bx\n\tpop ax\n";
							*out << "\tcmp bx, 0\n\tje nulldiv\n";
							*out << "\tcwd\n\tidiv bx\n\tpush dx\n";
							break;
						}
						}
						break;
					}
					case LEX_LOG_OPERATOR:
					{
						switch (lextable.table[i].operation)
						{
						case LT::OPER::MORE:
						{
							*out << "\tpop bx\n\tpop ax\n";
							*out << "\tcmp ax, bx\n";
							*out << "\tjg l" << numOfLogPoints << "\n";
							*out << "\tjle l" << numOfLogPoints + 1 << "\n";

							*out << "l" << numOfLogPoints << ":\n";
							*out << "\tmov ax, 1\n\tpush ax\n\tjmp endofexpr" << numOfLogExpr << "\n";

							*out << "l" << numOfLogPoints + 1 << ":\n";
							*out << "\tmov ax, 0\n\tpush ax\n\n";

							*out << "endofexpr" << numOfLogExpr << ":\n";
							numOfLogExpr++;
							numOfLogPoints += 2;
							break;
						}
						case LT::OPER::LESS:
						{
							*out << "\tpop bx\n\tpop ax\n";
							*out << "\tcmp ax, bx\n";
							*out << "\tjl l" << numOfLogPoints << "\n";
							*out << "\tjge l" << numOfLogPoints + 1 << "\n";

							*out << "l" << numOfLogPoints << ":\n";
							*out << "\tmov ax, 1\n\tpush ax\n\tjmp endofexpr" << numOfLogExpr << "\n";

							*out << "l" << numOfLogPoints + 1 << ":\n";
							*out << "\tmov ax, 0\n\tpush ax\n\n";

							*out << "endofexpr" << numOfLogExpr << ":\n";
							numOfLogExpr++;
							numOfLogPoints += 2;
							break;
						}
						case LT::OPER::EQU:
						{
							*out << "\tpop bx\n\tpop ax\n";
							*out << "\tcmp ax, bx\n";
							*out << "\tje l" << numOfLogPoints << "\n";
							*out << "\tjne l" << numOfLogPoints + 1 << "\n";

							*out << "l" << numOfLogPoints << ":\n";
							*out << "\tmov ax, 1\n\tpush ax\n\tjmp endofexpr" << numOfLogExpr << "\n";

							*out << "l" << numOfLogPoints + 1 << ":\n";
							*out << "\tmov ax, 0\n\tpush ax\n\n";

							*out << "endofexpr" << numOfLogExpr << ":\n";
							numOfLogExpr++;
							numOfLogPoints += 2;
							break;
						}
						case LT::OPER::NOEQU:
						{
							*out << "\tpop bx\n\tpop ax\n";
							*out << "\tcmp ax, bx\n";
							*out << "\tjne l" << numOfLogPoints << "\n";
							*out << "\tje l" << numOfLogPoints + 1 << "\n";

							*out << "l" << numOfLogPoints << ":\n";
							*out << "\tmov ax, 1\n\tpush ax\n\tjmp endofexpr" << numOfLogExpr << "\n";

							*out << "l" << numOfLogPoints + 1 << ":\n";
							*out << "\tmov ax, 0\n\tpush ax\n\n";

							*out << "endofexpr" << numOfLogExpr << ":\n";
							numOfLogExpr++;
							numOfLogPoints += 2;
							break;
						}
						case LT::OPER::MOREQ:
						{
							*out << "\tpop bx\n\tpop ax\n";
							*out << "\tcmp ax, bx\n";
							*out << "\tjge l" << numOfLogPoints << "\n";
							*out << "\tjl l" << numOfLogPoints + 1 << "\n";

							*out << "l" << numOfLogPoints << ":\n";
							*out << "\tmov ax, 1\n\tpush ax\n\tjmp endofexpr" << numOfLogExpr << "\n";

							*out << "l" << numOfLogPoints + 1 << ":\n";
							*out << "\tmov ax, 0\n\tpush ax\n\n";

							*out << "endofexpr" << numOfLogExpr << ":\n";
							numOfLogExpr++;
							numOfLogPoints += 2;
							break;
						}
						case LT::OPER::LESSQ:
						{
							*out << "\tpop bx\n\tpop ax\n";
							*out << "\tcmp ax, bx\n";
							*out << "\tjle l" << numOfLogPoints << "\n";
							*out << "\tjg l" << numOfLogPoints + 1 << "\n";

							*out << "l" << numOfLogPoints << ":\n";
							*out << "\tmov ax, 1\n\tpush ax\n\tjmp endofexpr" << numOfLogExpr << "\n";

							*out << "l" << numOfLogPoints + 1 << ":\n";
							*out << "\tmov ax, 0\n\tpush ax\n\n";

							*out << "endofexpr" << numOfLogExpr << ":\n";
							numOfLogExpr++;
							numOfLogPoints += 2;
							break;
						}
						}
						break;
					}
					case LEX_LINELEN:
					{
						flagLinelen = true;
						break;
					}
					case LEX_CONCAT:
					{
						flagConcat = true;
						break;
					}
					case LEX_RANDOM:
					{
						flagRand = true;
						break;
					}
					case LEX_SQRT:
					{
						flagSqrt = true;
						break;
					}
					case '@':
					{
						countParm = (char)lextable.table[i + 1].lexema - '0';

						for (int j = 1; j <= countParm; j++)
						{
							*out << "\tpop dx\n";
						}

						for (int j = 1; j <= countParm; j++)
						{
							if (stk.top().iddatatype == IT::IDDATATYPE::LINE)
							{
								if (stk.top().idtype == IT::IDTYPE::L)
									*out << "\tpush offset " << stk.top().id << "\n";
								else
									*out << "\tpush " << stk.top().id << "\n";
							}
							else
							{
								*out << "\tmovsx eax, " << stk.top().id << "\n";
								*out << "\tpush eax" << "\n";
							}
							stk.pop();
						}

						if (flagCallfunc && !flagConcat && !flagLinelen && !flagSqrt && !flagRand)
						{
							if (idtable.table[lextable.table[i - countParm - 1].idxTI].iddatatype == IT::IDDATATYPE::LINE)
								*out << "\tcall " << idtable.table[lextable.table[i - countParm - 1].idxTI].id << "\n\tpush eax\n";
							else
								*out << "\tcall " << idtable.table[lextable.table[i - countParm - 1].idxTI].id << "\n\tpush ax\n";
							flagCallfunc = false;
						}

						if (flagLinelen)
						{
							*out << "\tcall LINELEN" << "\n" << "\tpush ax\n";
							flagLinelen = false;
						}
						if (flagConcat)
						{
							*out << "\tpush offset buffer\n";
							*out << "\tcall CONCAT\n" << "\tpush eax\n";
							flagConcat = false;
						}
						if (flagRand)
						{
							*out << "\tcall RANDOM" << "\n" << "\tpush ax\n";
							flagRand = false;
						}
						if (flagSqrt)
						{
							*out << "\tcall SQRT" << "\n" << "\tpush ax\n";
							flagSqrt = false;
						}
						break;
					}
					}
					i++;
				}
				if (idtable.table[lextable.table[result_position].idxTI].iddatatype == IT::IDDATATYPE::BOOL)
				{
					*out << "\tpop ax\n";
					*out << "\tcmp ax, 0\n";
					*out << "\tje l" << numOfLogPoints << "\n";
					*out << "\tjne l" << numOfLogPoints + 1 << "\n";

					*out << "l" << numOfLogPoints << ":\n";
					*out << "\tmov ax, 0\n\tpush ax\n\tjmp endofexpr" << numOfLogExpr << "\n";

					*out << "l" << numOfLogPoints + 1 << ":\n";
					*out << "\tmov ax, 1\n\tpush ax\n\n";

					*out << "endofexpr" << numOfLogExpr << ":\n";
					numOfLogExpr++;
					numOfLogPoints += 2;
				}
				*out << "\tpop " << idtable.table[lextable.table[result_position].idxTI].id << "\n";
				break;
			}
			case '@':
			{
				countParm = (char)lextable.table[i + 1].lexema - '0';
				for (int j = 1; j <= countParm; j++)
				{
					if (idtable.table[lextable.table[i - j].idxTI].iddatatype == IT::IDDATATYPE::NUM || idtable.table[lextable.table[i - j].idxTI].iddatatype == IT::IDDATATYPE::BOOL)
					{
						*out << "\tmovsx eax, " << idtable.table[lextable.table[i - j].idxTI].id << "\n";
						*out << "\tpush eax" << "\n";
					}
					else
					{
						if (idtable.table[lextable.table[i - j].idxTI].idtype == IT::IDTYPE::L)
							*out << "\tpush offset" << idtable.table[lextable.table[i - j].idxTI].id << "\n";
						else
							*out << "\tpush " << idtable.table[lextable.table[i - j].idxTI].id << "\n";
					}
				}
				*out << "\tcall " << idtable.table[lextable.table[i - countParm - 1].idxTI].id << "\n";
				break;
			}
			case LEX_GIVE:
			{
				*out << "\tpush ";
				i++;
				if (idtable.table[lextable.table[i + 1].idxTI].idtype == IT::IDTYPE::L)
				{
					if (idtable.table[lextable.table[i + 1].idxTI].iddatatype == IT::IDDATATYPE::NUM)
						*out << idtable.table[lextable.table[i + 1].idxTI].value.vint << "\n";
					else if (idtable.table[lextable.table[i + 1].idxTI].iddatatype == IT::IDDATATYPE::LINE)
						*out << "offset " << idtable.table[lextable.table[i + 1].idxTI].id << "\n";
				}
				else
				{
					*out << idtable.table[lextable.table[i + 1].idxTI].id << "\n";
				}
				if (flagFunc)
				{
					*out << "\tjmp local" << numOfRet << "\n";
					flagRet = true;
				}
				if (flagBody)
				{
					*out << "\t\tjmp theend\n";
					flagRet = true;
				}
				break;
			}
			case LEX_BRACELET:
			{
				if (flagBody && !flag_then && !flagOtherwise && !flagFunc && !flagCycle)
				{
					if (flagRet)
					{
						*out << "theend:\n";
						flagRet = false;
					}
					*out << "\tpush 0\n\tcall ExitProcess";
					*out << "\nnulldiv:\n\tpush offset null_division\n\tcall OutLineLn\n\tpush -1\n\tcall ExitProcess";
					*out << "\nprogram ENDP\nend program";
				}
				if (flagFunc && !flag_then && !flagOtherwise && !flagCycle)
				{
					if (flagRet)
					{
						*out << "local" << numOfRet++ << ":\n";
						*out << "\tpop eax\n\tret\n";
					}
					if (flagProc)
					{
						*out << "\tret\n";
						flagProc = false;
					}
					*out << funcName << " ENDP\n\n";
					flagFunc = false;
				}
				if (flag_then)
				{
					flag_then = false;
					if (flagOtherwise)
					{
						*out << "\tjmp e" << numOfEnds << "\n";
						flagOtherwise = false;
					}
					*out << "m" << numOfPoints++ << ":\n";
				}
				if (flagOtherwise)
				{
					flagOtherwise = false;
					*out << "e" << numOfEnds++ << ":\n";
				}
				if (flagCycle)
				{
					*out << cycleCode << "cyclenext" << numOfCycles << ":\n";
					cycleCode.clear();
					numOfCycles++;
					flagCycle = false;
				}
				break;
			}
			case LEX_WHEN:
			{
				flagWhen = true;
				break;
			}
			case LEX_СYCLE:
			{
				flagCycle = true;
				flagCondition = true;
				break;
			}
			case LEX_OTHERWISE:
			{
				flagOtherwise = true;
				break;
			}
			case LEX_LEFTTHESIS:
			{
				if (flagWhen)
				{
					if (lextable.table[i + 2].lexema == LEX_RIGHTTHESIS)
					{
						*out << "\tmov ax, " << idtable.table[lextable.table[i + 1].idxTI].id << "\n";
						*out << "\tcmp ax, 0\n";
						*out << "\tjnz m" << numOfPoints << "\n";
						*out << "\tjz m" << numOfPoints + 1 << "\n";
					}
					else
					{
						*out << "\tmov ax, " << idtable.table[lextable.table[i + 1].idxTI].id << "\n";
						*out << "\tcmp ax, " << idtable.table[lextable.table[i + 3].idxTI].id << "\n";
						if (lextable.table[i + 2].operation == LT::OPER::MORE)
						{
							*out << "\tjg m" << numOfPoints << "\n";
							*out << "\tjl m" << numOfPoints + 1 << "\n";
						}
						else if (lextable.table[i + 2].operation == LT::OPER::LESS)
						{
							*out << "\tjl m" << numOfPoints << "\n";
							*out << "\tjg m" << numOfPoints + 1 << "\n";
						}
						else if (lextable.table[i + 2].operation == LT::OPER::EQU)
						{
							*out << "\tje m" << numOfPoints << "\n";
							*out << "\tjne m" << numOfPoints + 1 << "\n";
						}
						else if (lextable.table[i + 2].operation == LT::OPER::NOEQU)
						{
							*out << "\tjne m" << numOfPoints << "\n";
							*out << "\tje m" << numOfPoints + 1 << "\n";
						}
						else if (lextable.table[i + 2].operation == LT::OPER::MOREQ)
						{
							*out << "\tjge m" << numOfPoints << "\n";
							*out << "\tjle m" << numOfPoints + 1 << "\n";
						}
						else if (lextable.table[i + 2].operation == LT::OPER::LESSQ)
						{
							*out << "\tjle m" << numOfPoints << "\n";
							*out << "\tjge m" << numOfPoints + 1 << "\n";
						}
					}
					int j = i;
					while (lextable.table[j++].lexema != LEX_BRACELET)
					{
						if (lextable.table[j + 1].lexema == LEX_OTHERWISE)
						{
							flagOtherwise = true;
							break;
						}
					}
				}
				if (flagCondition)
				{
					if (idtable.table[lextable.table[i + 1].idxTI].iddatatype != IT::IDDATATYPE::LINE && lextable.table[i + 2].lexema == LEX_RIGHTTHESIS)
					{
						*out << "\tmov ax, " << idtable.table[lextable.table[i + 1].idxTI].id << "\n";
						*out << "\tcmp ax, 0\n";
						cycleCode = "\tmov ax, " + string((char*)idtable.table[lextable.table[i + 1].idxTI].id) + "\n" + "\tcmp ax, 0\n";
						cycleCode += "\tjnz cycle" + to_string(numOfCycles) + "\n";
						*out << "\tjnz cycle" << numOfCycles << "\n";
					}
					else
					{
						cycleCode = "\tmov ax, " + string((char*)idtable.table[lextable.table[i + 1].idxTI].id) + "\n" +
							"\tcmp ax, " + string((char*)idtable.table[lextable.table[i + 3].idxTI].id) + "\n";
						*out << "\tmov ax, " << idtable.table[lextable.table[i + 1].idxTI].id << "\n";
						*out << "\tcmp ax, " << idtable.table[lextable.table[i + 3].idxTI].id << "\n";
						if (lextable.table[i + 2].operation == LT::OPER::MORE)
						{
							cycleCode += "\tjg cycle" + to_string(numOfCycles) + "\n";
							*out << "\tjg cycle" << numOfCycles << "\n";
						}
						else if (lextable.table[i + 2].operation == LT::OPER::LESS)
						{
							cycleCode += "\tjl cycle" + to_string(numOfCycles) + "\n";
							*out << "\tjl cycle" << numOfCycles << "\n";
						}
						else if (lextable.table[i + 2].operation == LT::OPER::EQU)
						{
							cycleCode += "\tje cycle" + to_string(numOfCycles) + "\n";
							*out << "\tje cycle" << numOfCycles << "\n";
						}
						else if (lextable.table[i + 2].operation == LT::OPER::NOEQU)
						{
							cycleCode += "\tjne cycle" + to_string(numOfCycles) + "\n";
							*out << "\tjne cycle" << numOfCycles << "\n";
						}
						else if (lextable.table[i + 2].operation == LT::OPER::MOREQ)
						{
							cycleCode += "\tjge cycle" + to_string(numOfCycles) + "\n";
							*out << "\tjge cycle" << numOfCycles << "\n";
						}
						else if (lextable.table[i + 2].operation == LT::OPER::LESSQ)
						{
							cycleCode += "\tjle cycle" + to_string(numOfCycles) + "\n";
							*out << "\tjle cycle" << numOfCycles << "\n";
						}
					}
					*out << "\tjmp cyclenext" << numOfCycles << "\n";
				}
				break;
			}
			case LEX_RIGHTTHESIS:
			{
				if (lextable.table[i + 1].lexema == LEX_LEFTBRACE && flagWhen)
				{
					flag_then = true;
					*out << "m" << numOfPoints++ << ":\n";
					flagWhen = false;
				}
				if (lextable.table[i + 1].lexema == LEX_LEFTBRACE && flagCondition)
				{
					*out << "cycle" << numOfCycles << ":\n";
					flagCondition = false;
				}
				break;
			}
			case LEX_OUT:
			{
				if (idtable.table[lextable.table[i + 2].idxTI].iddatatype == IT::IDDATATYPE::NUM)
				{
					*out << "\tmovsx eax, " << idtable.table[lextable.table[i + 2].idxTI].id << "\n";
					*out << "\tpush eax" << "\n\tcall OutNumber\n";
				}
				else if (idtable.table[lextable.table[i + 2].idxTI].iddatatype == IT::IDDATATYPE::BOOL)
				{
					*out << "\tmovsx eax, " << idtable.table[lextable.table[i + 2].idxTI].id << "\n";
					*out << "\tpush eax" << "\n\tcall OutBool\n";
				}
				else
				{
					if (idtable.table[lextable.table[i + 2].idxTI].idtype == IT::IDTYPE::L)
						*out << "\tpush offset ";
					else
						*out << "\tpush ";
					*out << idtable.table[lextable.table[i + 2].idxTI].id << "\n\tcall OutLine\n";
				}
				break;
			}
			case LEX_OUTLN:
			{
				if (idtable.table[lextable.table[i + 2].idxTI].iddatatype == IT::IDDATATYPE::NUM)
				{
					*out << "\tmovsx eax, " << idtable.table[lextable.table[i + 2].idxTI].id << "\n";
					*out << "\tpush eax" << "\n\tcall OutNumberLn\n";
				}
				else if (idtable.table[lextable.table[i + 2].idxTI].iddatatype == IT::IDDATATYPE::BOOL)
				{
					*out << "\tmovsx eax, " << idtable.table[lextable.table[i + 2].idxTI].id << "\n";
					*out << "\tpush eax" << "\n\tcall OutBoolLn\n";
				}
				else
				{
					if (idtable.table[lextable.table[i + 2].idxTI].idtype == IT::IDTYPE::L)
						*out << "\tpush offset ";
					else
						*out << "\tpush ";
					*out << idtable.table[lextable.table[i + 2].idxTI].id << "\n\tcall OutLineLn\n";
				}
				break;
			}
			}
		}
	}
};