#include "stdafx.h"
#include "LexAnalysis.h"
#include <string>

namespace LexAnalisys
{

	WORD* GetWord(unsigned char source[], int size)
	{
		for (int i = 0; i < size; i++)
		{
			if (source[i] == IN_CODE_ENDL)
				source[i] = IN_CODE_END;
		}

		WORD* word = new WORD[LT_MAXSIZE];

		for (int i = 0; i < LT_MAXSIZE; i++)
			word[i].word = new unsigned char[256] { NULL };

		bool findSeparator, findLiteral = false;
		int j = 0, lengthOfLine = 0, position = 1, line = 1, count = 0;

		char separators[] = { " ,;()[]{}=+-*/|<>^\t" };

		for (int i = 0, k = 0; i < size; i++, k++)
		{
			findSeparator = false;
			if (source[i] == '\"')
			{
				findLiteral = !findLiteral;
				if (lengthOfLine + 1 > TI_LINE_MAXSIZE)
					throw ERROR_THROW_IN(304, line, position - lengthOfLine);

				lengthOfLine = 0;
				count++;
			}

			if (findLiteral)
				lengthOfLine++;


			for (int t = 0; t < sizeof(separators) - 1; t++)
			{
				if (source[i] == separators[t] && !findLiteral)
				{
					findSeparator = true;
					if (word[j].word[0] != NULL)
					{
						word[j].position = position - _mbslen(word[j].word);
						word[j++].word[k] = '\0';
						k = 0;
					}
					if (separators[t] == ' ' || separators[t] == '\t')
					{
						k = -1;
						position++;
						break;
					}
					word[j].word[k++] = separators[t];
					if (source[i + 1] == '=')
					{
						word[j].word[k++] = '=';
						i++;
					}
					word[j].position = position;
					position++;

					word[j++].word[k] = '\0';
					k = -1;
					break;
				}
			}
			if (!findSeparator)
			{
				word[j].word[k] = source[i];
				position++;
			}
			if (source[i] == IN_CODE_END && !findLiteral)
			{
				line++;
				position = 1;
			}
		}
		if (count % 2 != 0)
			throw ERROR_THROW_IN(300, line, position - lengthOfLine);

		word[j].word = NULL;

		return word;
	}


	LA Analyze(In::IN in)
	{
		LA lex;
		LT::LexTable lextable = LT::Create(LT_MAXSIZE);
		IT::IdTable idtable = IT::Create(TI_MAXSIZE);
		WORD* word = new WORD[LT_MAXSIZE];
		for (int i = 0; i < LT_MAXSIZE; i++)
			word[i].word = new unsigned char[WORD_MAXSIZE] {NULL};

		do
		{
			word = GetWord(in.text, in.size);
		} while (word == NULL);

		int indxLex = 0;
		int indexID = 0;
		int clit = 1;
		int line = 1;

		unsigned char emptystr[] = "";
		unsigned char* prefix = new unsigned char[ID_PREFIX_MAXSIZE] { "" };
		unsigned char* bufprefix = new unsigned char[ID_PREFIX_MAXSIZE] { "" };
		unsigned char* oldprefix = new unsigned char[ID_PREFIX_MAXSIZE] { "" };
		unsigned char* lit = new unsigned char[4] { "lit" };
		unsigned char* bufL = new unsigned char[TI_LINE_MAXSIZE];
		unsigned char* nameLiteral = new unsigned char[10] { "" };
		char* litCount = new char[10] { "" };

		bool findFunc = false, findStnFunc = false, findParm = false, findProc = false, findUnaryMinus = false, findOut = false;
		int program = 0, idFunc = -1, idParam = -1;

		int open = 0, close = 0;

		IT::Entry entryIT;

		for (int i = 0; word[i].word != NULL; i++, indxLex++)
		{
			bool findSameID = false;

			FST::FST fstDef(word[i].word, GRAPH_DEFINE);
			if (FST::execute(fstDef))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_DEFINE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstTypeNumber(word[i].word, GRAPH_NUMBER);
			if (FST::execute(fstTypeNumber))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_TYPE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				entryIT.iddatatype = IT::IDDATATYPE::NUM;
				continue;
			}

			FST::FST fstTypeLine(word[i].word, GRAPH_LINE);
			if (FST::execute(fstTypeLine))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_TYPE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				entryIT.iddatatype = IT::IDDATATYPE::LINE;
				_mbscpy(entryIT.value.vstr.str, emptystr);
				continue;
			}

			FST::FST fstTypeBool(word[i].word, GRAPH_BOOL);
			if (FST::execute(fstTypeBool))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_TYPE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				entryIT.iddatatype = IT::IDDATATYPE::BOOL;
				continue;
			}

			FST::FST fstFunction(word[i].word, GRAPH_FUNCTION);
			if (FST::execute(fstFunction))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_FUNCTION, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				entryIT.idtype = IT::IDTYPE::F;
				entryIT.value.params.count = 0;
				entryIT.value.params.types = new IT::IDDATATYPE[MAX_PARAMS_COUNT];
				idFunc = indexID;
				findFunc = true;
				continue;
			}

			FST::FST fstProc(word[i].word, GRAPH_PROCEDURE);
			if (FST::execute(fstProc))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_PROCEDURE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				entryIT.idtype = IT::IDTYPE::F;
				entryIT.iddatatype = IT::IDDATATYPE::PROC;
				entryIT.value.params.count = 0;
				entryIT.value.params.types = new IT::IDDATATYPE[MAX_PARAMS_COUNT];
				idFunc = indexID;
				findProc = true;
				findFunc = true;
				continue;
			}

			FST::FST fstConcat(word[i].word, GRAPH_CONCAT);
			if (FST::execute(fstConcat))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_CONCAT, indexID++, line);
				LT::Add(lextable, entryLT);
				entryIT = {};
				_mbscpy(entryIT.id, word[i].word);
				entryIT.idxfirstLE = indxLex;
				entryIT.idtype = IT::IDTYPE::S;
				_mbscpy(entryIT.visibility, emptystr);
				entryIT.iddatatype = CONCAT_TYPE;
				entryIT.value.params.count = CONCAT_PARAMS_CNT;
				entryIT.value.params.types = new IT::IDDATATYPE[CONCAT_PARAMS_CNT];
				for (int k = 0; k < CONCAT_PARAMS_CNT; k++)
					entryIT.value.params.types[k] = IT::CONCAT_PARAMS[k];
				IT::Add(idtable, entryIT);
				entryIT = {};
				findStnFunc = true;
				continue;
			}

			FST::FST fstLinelen(word[i].word, GRAPH_LINELEN);
			if (FST::execute(fstLinelen))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_LINELEN, indexID++, line);
				LT::Add(lextable, entryLT);
				entryIT = {};
				_mbscpy(entryIT.id, word[i].word);
				entryIT.idxfirstLE = indxLex;
				entryIT.idtype = IT::IDTYPE::S;
				_mbscpy(entryIT.visibility, emptystr);
				entryIT.iddatatype = LINELEN_TYPE;
				entryIT.value.params.count = LINELEN_PARAMS_CNT;
				entryIT.value.params.types = new IT::IDDATATYPE[LINELEN_PARAMS_CNT];
				for (int k = 0; k < LINELEN_PARAMS_CNT; k++)
					entryIT.value.params.types[k] = IT::LINELEN_PARAMS[k];
				IT::Add(idtable, entryIT);
				entryIT = {};
				findStnFunc = true;
				continue;
			}

			FST::FST fstRandom(word[i].word, GRAPH_RANDOM);
			if (FST::execute(fstRandom))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_RANDOM, indexID++, line);
				LT::Add(lextable, entryLT);
				entryIT = {};
				_mbscpy(entryIT.id, word[i].word);
				entryIT.idxfirstLE = indxLex;
				entryIT.idtype = IT::IDTYPE::S;
				_mbscpy(entryIT.visibility, emptystr);
				entryIT.iddatatype = RANDOM_TYPE;
				entryIT.value.params.count = RANDOM_PARAMS_CNT;
				entryIT.value.params.types = new IT::IDDATATYPE[RANDOM_PARAMS_CNT];
				for (int k = 0; k < RANDOM_PARAMS_CNT; k++)
					entryIT.value.params.types[k] = IT::RANDOM_PARAMS[k];
				IT::Add(idtable, entryIT);
				entryIT = {};
				findStnFunc = true;
				continue;
			}

			FST::FST fstSqrt(word[i].word, GRAPH_SQRT);
			if (FST::execute(fstSqrt))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_SQRT, indexID++, line);
				LT::Add(lextable, entryLT);
				entryIT = {};
				_mbscpy(entryIT.id, word[i].word);
				entryIT.idxfirstLE = indxLex;
				entryIT.idtype = IT::IDTYPE::S;
				_mbscpy(entryIT.visibility, emptystr);
				entryIT.iddatatype = SQRT_TYPE;
				entryIT.value.params.count = SQRT_PARAMS_CNT;
				entryIT.value.params.types = new IT::IDDATATYPE[SQRT_PARAMS_CNT];
				for (int k = 0; k < SQRT_PARAMS_CNT; k++)
					entryIT.value.params.types[k] = IT::SQRT_PARAMS[k];
				IT::Add(idtable, entryIT);
				entryIT = {};
				findStnFunc = true;
				continue;
			}

			FST::FST fstOutLN(word[i].word, GRAPH_OUTLN);
			if (FST::execute(fstOutLN))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_OUTLN, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstGive(word[i].word, GRAPH_GIVE);
			if (FST::execute(fstGive))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_GIVE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstOut(word[i].word, GRAPH_OUT);
			if (FST::execute(fstOut))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_OUT, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstCycle(word[i].word, GRAPH_CYCLE);
			if (FST::execute(fstCycle))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_ÑYCLE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstWhen(word[i].word, GRAPH_WHEN);
			if (FST::execute(fstWhen))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_WHEN, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstOtherwise(word[i].word, GRAPH_OTHERWISE);
			if (FST::execute(fstOtherwise))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_OTHERWISE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstProgram(word[i].word, GRAPH_PROGRAM);
			if (FST::execute(fstProgram))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_PROGRAM, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				_mbscpy(oldprefix, prefix);
				_mbscpy(prefix, word[i].word);
				_mbscpy(entryIT.visibility, emptystr);
				program++;
				continue;
			}

			FST::FST fstTrue(word[i].word, GRAPH_TRUE);
			FST::FST fstFalse(word[i].word, GRAPH_FALSE);
			if (FST::execute(fstTrue) || FST::execute(fstFalse))
			{
				int value;

				if (!strcmp((char*)word[i].word, "true"))
					value = 1;
				else
					value = 0;

				for (int k = 0; k < idtable.size; k++)
				{
					if (idtable.table[k].value.vint == value && idtable.table[k].idtype == IT::IDTYPE::L && idtable.table[k].iddatatype == IT::IDDATATYPE::BOOL)
					{
						LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, k, line);
						LT::Add(lextable, entryLT);
						findSameID = true;
						break;
					}
				}

				if (findSameID)
					continue;

				LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, indexID++, line);
				LT::Add(lextable, entryLT);
				entryIT.idtype = IT::IDTYPE::L;
				entryIT.iddatatype = IT::IDDATATYPE::BOOL;
				entryIT.value.vint = value;
				entryIT.idxfirstLE = indxLex;

				_itoa_s(clit++, litCount, sizeof(char) * 10, 10);
				_mbscpy(bufL, lit);
				word[i].word = _mbscat(bufL, (unsigned char*)litCount);
				_mbscpy(entryIT.id, word[i].word);
				IT::Add(idtable, entryIT);
				entryIT = {};
				continue;
			}
			FST::FST fstIdentif(word[i].word, GRAPH_ID);
			if (FST::execute(fstIdentif))
			{
				if (strlen(reinterpret_cast<const char*>(word[i].word)) > ID_MAXSIZE)
					throw ERROR_THROW_IN(206, line, word[i].position);

				if (findFunc)
				{
					if (findProc && entryIT.iddatatype != IT::IDDATATYPE::PROC)
						throw ERROR_THROW_IN(312, line, -1);
					int idx = IT::IsId(idtable, word[i].word);
					if (idx != TI_NULLIDX)
						throw ERROR_THROW_IN(308, line, -1);
				}
				else
				{
					int idx = IT::IsId(idtable, word[i].word);
					if (idx != TI_NULLIDX)
					{
						LT::Entry entryLT = writeEntry(entryLT, LEX_ID, idx, line);
						LT::Add(lextable, entryLT);
						findFunc = false;
						continue;
					}
					_mbscpy(bufprefix, prefix);
					_mbscat(bufprefix, VISIBILITY_SEP);
					word[i].word = _mbscat(bufprefix, word[i].word);
					idx = IT::IsId(idtable, word[i].word);
					if (idx != TI_NULLIDX)
					{
						LT::Entry entryLT = writeEntry(entryLT, LEX_ID, idx, line);
						LT::Add(lextable, entryLT);
						if (lextable.table[lextable.size - 3].lexema == LEX_DEFINE)
						{
							if (idtable.table[idx].idtype == IT::IDTYPE::P)
								throw ERROR_THROW_IN(309, line, -1);

							throw ERROR_THROW_IN(310, line, -1);

						}
						continue;
					}
				}

				LT::Entry entryLT = writeEntry(entryLT, LEX_ID, indexID++, line);
				LT::Add(lextable, entryLT);

				if (findParm)
				{
					entryIT.idtype = IT::IDTYPE::P;
					_mbscpy(entryIT.visibility, prefix);
					idParam++;
					idtable.table[idFunc].value.params.count++;

					if (idtable.table[idFunc].value.params.count > 3)
						throw ERROR_THROW_IN(314, line, -1);

					idtable.table[idFunc].value.params.types[idParam] = entryIT.iddatatype;
				}

				else if (!findFunc)
				{
					entryIT.idtype = IT::IDTYPE::V;
					_mbscpy(entryIT.visibility, prefix);
					if (entryIT.iddatatype == IT::IDDATATYPE::NUM || entryIT.iddatatype == IT::IDDATATYPE::BOOL)
						entryIT.value.vint = TI_NUM_DEFAULT;
					if (entryIT.iddatatype == IT::IDDATATYPE::LINE)
					{
						entryIT.value.vstr.len = 0;
						memset(entryIT.value.vstr.str, TI_LINE_DEFAULT, sizeof(char));
					}

					if (lextable.table[lextable.size - 2].lexema == LEX_TYPE && lextable.table[lextable.size - 3].lexema != LEX_DEFINE)
						throw ERROR_THROW_IN(305, line, word[i].position);
					if (lextable.table[lextable.size - 2].lexema == LEX_DEFINE && lextable.table[lextable.size - 2].lexema != LEX_TYPE)
						throw ERROR_THROW_IN(307, line, word[i].position);
				}
				else
				{
					_mbscpy(oldprefix, prefix);
					_mbscpy(prefix, word[i].word);
					_mbscpy(entryIT.visibility, emptystr);
				}

				entryIT.idxfirstLE = indxLex;
				_mbscpy(entryIT.id, word[i].word);

				if (entryIT.idtype == IT::IDTYPE::F && entryIT.iddatatype == IT::IDDATATYPE::UNKNOWN)
					throw ERROR_THROW_IN(311, line, -1);
				if (entryIT.idtype == IT::IDTYPE::V && entryIT.iddatatype == IT::IDDATATYPE::UNKNOWN)
					throw ERROR_THROW_IN(306, line, -1);

				IT::Add(idtable, entryIT);
				entryIT = {};
				findFunc = false;
				findProc = false;
				continue;
			}

			FST::FST fstLiteralNum(word[i].word, GRAPH_NUM_LITERAL);
			if (FST::execute(fstLiteralNum))
			{
				int value = atoi((char*)word[i].word);

				if (findUnaryMinus)
				{
					value = -value;
					findUnaryMinus = false;
				}

				if (value > 32767 || value < -32768)
					throw ERROR_THROW_IN(319, line, word[i].position);

				for (int k = 0; k < idtable.size; k++)
				{
					if (idtable.table[k].value.vint == value && idtable.table[k].idtype == IT::IDTYPE::L && idtable.table[k].iddatatype == IT::IDDATATYPE::NUM)
					{
						LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, k, line);
						LT::Add(lextable, entryLT);
						findSameID = true;
						break;
					}
				}

				if (findSameID)
					continue;

				LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, indexID++, line);
				LT::Add(lextable, entryLT);
				entryIT.idtype = IT::IDTYPE::L;
				entryIT.iddatatype = IT::IDDATATYPE::NUM;
				entryIT.value.vint = value;
				entryIT.idxfirstLE = indxLex;

				_itoa_s(clit++, litCount, sizeof(char) * 10, 10);
				_mbscpy(bufL, lit);
				word[i].word = _mbscat(bufL, (unsigned char*)litCount);
				_mbscpy(entryIT.id, word[i].word);
				IT::Add(idtable, entryIT);
				entryIT = {};
				continue;
			}

			FST::FST fstLiteralString(word[i].word, GRAPH_LINE_LITERAL);
			if (FST::execute(fstLiteralString))
			{
				int length = _mbslen(word[i].word);
				for (int k = 0; k < length; k++)
					word[i].word[k] = word[i].word[k + 1];
				word[i].word[length - 2] = 0;

				if (_mbslen(word[i].word) == 0)
					throw ERROR_THROW_IN(318, line, word[i].position);

				for (int k = 0; k < idtable.size; k++)
				{
					if (!(_mbscmp(idtable.table[k].value.vstr.str, word[i].word)))
					{
						findSameID = true;
						LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, k, line);
						LT::Add(lextable, entryLT);
						break;
					}
				}

				if (findSameID)
					continue;

				LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, indexID++, line);
				LT::Add(lextable, entryLT);
				_mbscpy(entryIT.value.vstr.str, word[i].word);
				entryIT.value.vstr.len = length - 2;
				entryIT.idtype = IT::IDTYPE::L;
				entryIT.iddatatype = IT::IDDATATYPE::LINE;
				entryIT.idxfirstLE = indxLex;

				_itoa_s(clit++, litCount, sizeof(char) * 10, 10);
				_mbscpy(bufL, lit);
				nameLiteral = _mbscat(bufL, (unsigned char*)litCount);
				_mbscpy(entryIT.id, nameLiteral);
				IT::Add(idtable, entryIT);
				entryIT = {};
				continue;
			}

			FST::FST fstOperator(word[i].word, GRAPH_OPERATOR);
			if (FST::execute(fstOperator))
			{
				LT::Entry entryLT;
				switch (word[i].word[0])
				{
				case '+':
					entryLT = writeEntry(entryLT, LEX_OPERATOR, LT_TI_NULLIDX, line, 2, LT::OPER::PLUS);
					LT::Add(lextable, entryLT);
					break;
				case '-':
				{
					if (lextable.table[lextable.size - 1].lexema == LEX_LITERAL || lextable.table[lextable.size - 1].lexema == LEX_ID
						|| lextable.table[lextable.size - 1].lexema == LEX_RIGHTBRACKET || lextable.table[lextable.size - 1].lexema == LEX_RIGHTTHESIS)
					{
						entryLT = writeEntry(entryLT, LEX_OPERATOR, LT_TI_NULLIDX, line, 2, LT::OPER::MINUS);
						LT::Add(lextable, entryLT);
					}
					else
						findUnaryMinus = true;
					break;
				}
				case '/':
					entryLT = writeEntry(entryLT, LEX_OPERATOR, LT_TI_NULLIDX, line, 3, LT::OPER::DIV);
					LT::Add(lextable, entryLT);
					break;
				case '*':
					entryLT = writeEntry(entryLT, LEX_OPERATOR, LT_TI_NULLIDX, line, 3, LT::OPER::MUL);
					LT::Add(lextable, entryLT);
					break;
				case '%':
					entryLT = writeEntry(entryLT, LEX_OPERATOR, LT_TI_NULLIDX, line, 3, LT::OPER::REST);
					LT::Add(lextable, entryLT);
					break;
				}
				continue;
			}

			FST::FST fstEquals(word[i].word, GRAPH_EQUALS);
			if (FST::execute(fstEquals))
			{
				LT::Entry entryLT;
				entryLT = writeEntry(entryLT, LEX_LOG_OPERATOR, LT_TI_NULLIDX, line, -1, LT::OPER::EQU);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstNotEquals(word[i].word, GRAPH_NOTEQUALS);
			if (FST::execute(fstNotEquals))
			{
				LT::Entry entryLT;
				entryLT = writeEntry(entryLT, LEX_LOG_OPERATOR, LT_TI_NULLIDX, line, -1, LT::OPER::NOEQU);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstMoreEquals(word[i].word, GRAPH_MOREEQUALS);
			if (FST::execute(fstMoreEquals))
			{
				LT::Entry entryLT;
				entryLT = writeEntry(entryLT, LEX_LOG_OPERATOR, LT_TI_NULLIDX, line, -1, LT::OPER::MOREQ);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstLessEquals(word[i].word, GRAPH_LESSEQUALS);
			if (FST::execute(fstLessEquals))
			{
				LT::Entry entryLT;
				entryLT = writeEntry(entryLT, LEX_LOG_OPERATOR, LT_TI_NULLIDX, line, -1, LT::OPER::LESSQ);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstLogOperator(word[i].word, GRAPH_LOG_OPERATOR);
			if (FST::execute(fstLogOperator))
			{
				LT::Entry entryLT;
				switch (word[i].word[0])
				{
				case '>':
					entryLT = writeEntry(entryLT, LEX_LOG_OPERATOR, LT_TI_NULLIDX, line, -1, LT::OPER::MORE);
					break;
				case '<':
					entryLT = writeEntry(entryLT, LEX_LOG_OPERATOR, LT_TI_NULLIDX, line, -1, LT::OPER::LESS);
					break;
				}
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstSemicolon(word[i].word, GRAPH_SEMICOLON);
			if (FST::execute(fstSemicolon))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_SEMICOLON, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstComma(word[i].word, GRAPH_COMMA);
			if (FST::execute(fstComma))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_COMMA, LT_TI_NULLIDX, line, 1);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstLeftBrace(word[i].word, GRAPH_LEFTBRACE);
			if (FST::execute(fstLeftBrace))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_LEFTBRACE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				open++;
				continue;
			}

			FST::FST fstRightBrace(word[i].word, GRAPH_BRACELET);
			if (FST::execute(fstRightBrace))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_BRACELET, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				close++;
				continue;
			}

			FST::FST fstLeftThesis(word[i].word, GRAPH_LEFTTHESIS);
			if (FST::execute(fstLeftThesis))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_LEFTTHESIS, LT_TI_NULLIDX, line, 0);
				LT::Add(lextable, entryLT);
				if (indexID > 0 && idtable.table[indexID - 1].idtype == IT::IDTYPE::F)
					findParm = true;
				continue;
			}

			FST::FST fstRightThesis(word[i].word, GRAPH_RIGHTTHESIS);
			if (FST::execute(fstRightThesis))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_RIGHTTHESIS, LT_TI_NULLIDX, line, 0);
				if (findParm && word[i + 1].word[0] != LEX_LEFTBRACE && word[i + 2].word[0] != LEX_LEFTBRACE)
					_mbscpy(prefix, oldprefix);
				idParam = -1;
				findParm = false;
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstLeftBracket(word[i].word, GRAPH_LEFTBRACKET);
			if (FST::execute(fstLeftBracket))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_LEFTBRACKET, LT_TI_NULLIDX, line, 0);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstRightBracket(word[i].word, GRAPH_RIGHTBRACKET);
			if (FST::execute(fstRightBracket))
			{

				LT::Entry entryLT = writeEntry(entryLT, LEX_RIGHTBRACKET, LT_TI_NULLIDX, line, 0);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstEqual(word[i].word, GRAPH_EQUAL);
			if (FST::execute(fstEqual))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_EQUAL, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			if (word[i].word[0] == '|')
			{
				line++;
				indxLex--;
				continue;
			}

			throw ERROR_THROW_IN(205, line, word[i].position);
		}

		if (program == NULL)
			throw ERROR_THROW(302);
		if (program > 1)
			throw ERROR_THROW(303);
		if (open > close)
			throw ERROR_THROW(612);
		if (close > open)
			throw ERROR_THROW(613);

		lex.idtable = idtable;
		lex.lextable = lextable;
		return lex;
	}
}
