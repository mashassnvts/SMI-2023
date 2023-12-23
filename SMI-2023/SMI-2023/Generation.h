#pragma once

#include "Parm.h"
#include "LT.h"
#include "IT.h"
#include "LexAnalysis.h"

namespace GenerateAsm
{
	void Generation(LT::LexTable lextable, IT::IdTable idtable, wchar_t out[]);
	void Head(ofstream* out);
	void ConstSegment(IT::IdTable idtable, ofstream* out);
	void DataSegment(LT::LexTable lextable, IT::IdTable idtable, ofstream* out);
	void CodeSegment(LT::LexTable lextable, IT::IdTable idtable, ofstream* out);
};
