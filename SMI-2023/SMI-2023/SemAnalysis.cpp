#include "stdafx.h"
#include "SemAnalysis.h"

namespace Semantics
{
	void AnalyzeSem(LexAnalisys::LA lex)
	{
		for (int i = 0; i < lex.lextable.size; i++)
		{
			switch (lex.lextable.table[i].lexema)
			{
			case LEX_LINELEN:
			case LEX_CONCAT:
			case LEX_RANDOM:
			case LEX_SQRT:
			case LEX_ID:
			{
				if ((lex.idtable.table[lex.lextable.table[i].idxTI].idtype == IT::IDTYPE::F || lex.idtable.table[lex.lextable.table[i].idxTI].idtype == IT::IDTYPE::S)
					&& lex.lextable.table[i - 2].lexema != LEX_FUNCTION && lex.lextable.table[i - 1].lexema != LEX_PROCEDURE)
				{
					IT::Entry e = lex.idtable.table[lex.lextable.table[i].idxTI];
					int countParm = 0;
					if (lex.lextable.table[i + 1].lexema != LEX_LEFTTHESIS)
						throw ERROR_THROW_IN(608, lex.lextable.table[i].line, -1);

					for (int j = i + 2; lex.lextable.table[j].lexema != LEX_RIGHTTHESIS; j++)
					{
						if (lex.lextable.table[j].lexema == LEX_ID || lex.lextable.table[j].lexema == LEX_LITERAL)
						{
							countParm++;
							if (countParm > e.value.params.count)
								throw ERROR_THROW_IN(316, lex.lextable.table[j].line, -1);
							if (lex.idtable.table[lex.lextable.table[j].idxTI].iddatatype == IT::IDDATATYPE::NUM && e.value.params.types[countParm - 1] == IT::IDDATATYPE::LINE)
								throw ERROR_THROW_IN(315, lex.lextable.table[j].line, -1);
							if (lex.idtable.table[lex.lextable.table[j].idxTI].iddatatype == IT::IDDATATYPE::LINE && e.value.params.types[countParm - 1] != IT::IDDATATYPE::LINE)
								throw ERROR_THROW_IN(315, lex.lextable.table[j].line, -1);
						}

					}
					if (countParm < e.value.params.count)
						throw ERROR_THROW_IN(317, lex.lextable.table[i].line, -1);
				}
				break;
			}

			case LEX_EQUAL:
			{
				if (lex.lextable.table[i - 1].lexema == LEX_ID || lex.lextable.table[i - 1].lexema == LEX_LITERAL)
				{
					IT::IDDATATYPE lefttype = lex.idtable.table[lex.lextable.table[i - 1].idxTI].iddatatype;
					bool ignore = false;

					for (int k = i + 1; lex.lextable.table[k].lexema != LEX_SEMICOLON; k++)
					{
						if (lex.lextable.table[k].lexema == LEX_ID || lex.lextable.table[k].lexema == LEX_LITERAL ||
							lex.lextable.table[k].lexema == LEX_CONCAT || lex.lextable.table[k].lexema == LEX_LINELEN)
						{
							if (!ignore)
							{
								IT::IDDATATYPE righttype = lex.idtable.table[lex.lextable.table[k].idxTI].iddatatype;
								if (lefttype == IT::IDDATATYPE::NUM || lefttype == IT::IDDATATYPE::BOOL)
								{
									if (righttype != IT::IDDATATYPE::NUM && righttype != IT::IDDATATYPE::BOOL)
										throw ERROR_THROW_IN(320, lex.lextable.table[i].line, -1);
								}
								else if (lefttype != righttype)
									throw ERROR_THROW_IN(320, lex.lextable.table[i].line, -1);
							}

							if (lex.lextable.table[k + 1].lexema == LEX_LEFTTHESIS)
							{
								ignore = true;
								continue;
							}

							if (ignore && lex.lextable.table[k + 1].lexema == LEX_RIGHTTHESIS)
							{
								ignore = false;
								continue;
							}
						}

						if (lefttype == IT::IDDATATYPE::LINE)
						{
							char l = lex.lextable.table[k].lexema;
							if (l == LEX_OPERATOR)
								throw ERROR_THROW_IN(321, lex.lextable.table[i].line, -1);
						}
					}
					break;
				}
				break;

			}

			case LEX_FUNCTION:
			{
				int j = i;
				if (lex.lextable.table[i + 2].lexema == LEX_ID)
				{
					IT::Entry e = lex.idtable.table[lex.lextable.table[j + 2].idxTI];
					j++;
					while (j <= lex.lextable.size && lex.lextable.table[j].lexema != LEX_FUNCTION || j <= lex.lextable.size && lex.lextable.table[j].lexema != LEX_PROGRAM
						|| j <= lex.lextable.size && lex.lextable.table[j].lexema != LEX_PROCEDURE)
					{
						if (lex.lextable.table[j].lexema == LEX_GIVE && (lex.lextable.table[j + 2].lexema == LEX_ID || lex.lextable.table[j + 2].lexema == LEX_LITERAL))
						{
							int next = lex.lextable.table[j + 2].idxTI;
							if (next != TI_NULLIDX)
							{
								if (lex.idtable.table[next].iddatatype != e.iddatatype)
									throw ERROR_THROW_IN(313, lex.lextable.table[j].line, -1);
							}
						}
						j++;
					}
				}
				break;
			}

			case LEX_LOG_OPERATOR:
			{
				bool flag = true;
				if (lex.lextable.table[i - 1].lexema == LEX_ID || lex.lextable.table[i - 1].lexema == LEX_LITERAL)
				{
					if (lex.idtable.table[lex.lextable.table[i - 1].idxTI].iddatatype != IT::IDDATATYPE::NUM)
						flag = false;
				}
				if (lex.lextable.table[i + 1].lexema == LEX_ID || lex.lextable.table[i + 1].lexema == LEX_LITERAL)
				{
					if (lex.idtable.table[lex.lextable.table[i + 1].idxTI].iddatatype != IT::IDDATATYPE::NUM)
						flag = false;
				}
				if (lex.lextable.table[i - 1].lexema == LEX_ID
					&& lex.idtable.table[lex.lextable.table[i - 1].idxTI].idtype == IT::IDTYPE::F)
					throw ERROR_THROW_IN(615, lex.lextable.table[i].line, -1);
				if (lex.lextable.table[i + 1].lexema == LEX_ID
					&& lex.idtable.table[lex.lextable.table[i + 1].idxTI].idtype == IT::IDTYPE::F)
					throw ERROR_THROW_IN(615, lex.lextable.table[i].line, -1);

				if (!flag)
				{
					throw ERROR_THROW_IN(322, lex.lextable.table[i].line, -1);
				}
				break;
			}

			case LEX_OPERATOR:
			{
				if (lex.lextable.table[i + 1].lexema != LEX_LEFTBRACKET && lex.lextable.table[i + 1].lexema == LEX_LITERAL && lex.idtable.table[lex.lextable.table[i + 1].idxTI].value.vint < 0)
					throw ERROR_THROW_IN(614, lex.lextable.table[i + 1].line, -1);
				if (lex.lextable.table[i + 1].lexema == LEX_ID || lex.lextable.table[i + 1].lexema == LEX_LITERAL)
				{
					if (lex.lextable.table[i].operation == LT::OPER::DIV || lex.lextable.table[i].operation == LT::OPER::REST)
					{
						LT::Entry e;
						IT::Entry et;
						bool equ = false;
						if (lex.lextable.table[i + 1].lexema == LEX_LITERAL || lex.lextable.table[i + 1].lexema == LEX_ID)
							e = lex.lextable.table[i + 1];
						else
							e = lex.lextable.table[i + 2];
						et = lex.idtable.table[e.idxTI];

						if (lex.lextable.table[i + 1].lexema == LEX_ID && lex.idtable.table[lex.lextable.table[i + 1].idxTI].idtype == IT::IDTYPE::V)
						{
							for (int n = i; n >= 0; n--)
							{
								if (lex.lextable.table[n].lexema == LEX_ID && !strcmp((char*)lex.idtable.table[lex.lextable.table[n].idxTI].id, (char*)et.id)
									&& lex.lextable.table[n + 1].lexema == LEX_EQUAL)
								{
									if (lex.lextable.table[n + 2].lexema == LEX_LITERAL || lex.lextable.table[n + 2].lexema == LEX_ID && (lex.idtable.table[lex.lextable.table[n + 2].idxTI].iddatatype == IT::IDDATATYPE::NUM))
									{
										equ = true;
										if (lex.idtable.table[lex.lextable.table[n + 2].idxTI].value.vint == 0)
											throw ERROR_THROW_IN(323, lex.lextable.table[i].line, -1);
										break;
									}
									if (lex.lextable.table[n + 2].lexema == LEX_ID && (lex.idtable.table[lex.lextable.table[n + 2].idxTI].iddatatype == IT::IDDATATYPE::NUM)
										&& lex.idtable.table[lex.lextable.table[n + 2].idxTI].idtype == IT::IDTYPE::V)
									{
										strcpy((char*)et.id, (char*)lex.idtable.table[lex.lextable.table[n + 2].idxTI].id);
										continue;
									}
								}
							}
							if (!equ && lex.idtable.table[lex.lextable.table[i + 1].idxTI].value.vint == 0)
								throw ERROR_THROW_IN(323, lex.lextable.table[i].line, -1);
						}
						if (lex.lextable.table[i + 1].lexema == LEX_LITERAL || lex.lextable.table[i + 1].lexema == LEX_ID)
						{
							if (lex.idtable.table[e.idxTI].value.vint == 0)
								throw ERROR_THROW_IN(323, lex.lextable.table[i].line, -1);
						}
					}
				}
				break;
			}

			case LEX_ÑYCLE:
			case LEX_WHEN:
			{
				if (lex.lextable.table[i + 1].lexema == LEX_LEFTTHESIS && lex.lextable.table[i + 3].lexema == LEX_RIGHTTHESIS)
				{
					if ((lex.lextable.table[i + 2].lexema == LEX_ID || lex.lextable.table[i + 2].lexema == LEX_LITERAL) && lex.idtable.table[lex.lextable.table[i + 2].idxTI].iddatatype == IT::IDDATATYPE::LINE)
						throw ERROR_THROW_IN(301, lex.lextable.table[i].line, -1);
				}
				if (lex.lextable.table[i + 2].lexema == LEX_ID && lex.idtable.table[lex.lextable.table[i + 2].idxTI].idtype == IT::IDTYPE::F)
					throw ERROR_THROW_IN(615, lex.lextable.table[i].line, -1);
			}
			}
		}
	}
}