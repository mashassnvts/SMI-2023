#pragma once

#include "Error.h"

#define LEXEMA_FIXSIZE		1																 
#define LT_MAXSIZE			4096															 
#define LT_TI_NULLIDX		0xfffffff														 

#define LEX_TYPE			't'						 
#define LEX_DEFINE			'd'						 
#define LEX_ID				'i'						 
#define LEX_LITERAL			'l'						 
#define LEX_PROGRAM			'p'						 
#define LEX_FUNCTION		'f'						 
#define LEX_PROCEDURE		's'						 
#define LEX_GIVE			'r'						 
#define LEX_OUT				'o'						 
#define LEX_OUTLN			'b'						 
#define LEX_OPERATOR		'v'						 
#define LEX_EQUAL			'='						 
#define LEX_LOG_OPERATOR	'g'						 
#define LEX_ÑYCLE			'u'						 
#define LEX_WHEN			'w'						 
#define LEX_OTHERWISE		'!'						 
#define LEX_SEMICOLON		';'						 
#define LEX_COMMA			','						 
#define LEX_LEFTBRACE		'{'						 
#define LEX_BRACELET		'}'						 
#define LEX_LEFTTHESIS		'['						 
#define LEX_RIGHTTHESIS		']'						
#define LEX_LEFTBRACKET		'('						
#define LEX_RIGHTBRACKET	')'						

#define LEX_CONCAT			'+'						
#define LEX_LINELEN			'%'						
#define LEX_SQRT			'q'						
#define LEX_RANDOM			'z'						

namespace LT
{
	enum class OPER { NOT = -1, PLUS = 1, MINUS, MUL, DIV, REST, MORE, LESS, EQU, NOEQU, MOREQ, LESSQ };

	struct Entry
	{
		unsigned char lexema;
		int line;
		int idxTI;
		int priority;
		OPER operation;
	};

	struct LexTable
	{
		int maxsize;
		int size;
		Entry* table;
	};

	LexTable Create(
		int size
	);

	void Add(
		LexTable& lextable,
		Entry entry
	);

	Entry GetEntry(
		LexTable& lextable,
		int n
	);

	void Delete(LexTable& lextable);

	Entry writeEntry(
		Entry& entry,
		unsigned char lexema,
		int indx,
		int line,
		int priority = -1,
		OPER operation = OPER::NOT
	);

	void WriteLexTable(LexTable& lextable);

	void WritePoliz(LexTable lextable);
};