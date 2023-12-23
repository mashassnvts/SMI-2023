#pragma once
#include "In.h"
#include "Error.h"
#include "Log.h"
#include "IT.h"
#include "LT.h"
#include "FST.h"
#include "Graphs.h"

#define WORD_MAXSIZE 256
#define VISIBILITY_SEP (unsigned char*)"_"

namespace LexAnalisys
{
	struct LA
	{
		IT::IdTable idtable;
		LT::LexTable lextable;
	};

	struct WORD
	{
		unsigned char* word;
		unsigned int position;
	};

	LA Analyze(In::IN in);

	WORD* GetWord(unsigned char source[], int size);
}