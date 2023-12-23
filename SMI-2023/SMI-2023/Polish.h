#pragma once
#include "stdafx.h"
#include "LexAnalysis.h"
#include "Error.h"

#define LEX_COUNT_PARM0 '0'
#define LEX_COUNT_PARM1 '1'
#define LEX_COUNT_PARM2 '2'
#define LEX_COUNT_PARM3 '3'

namespace Poliz
{
	bool StartPoliz(LexAnalisys::LA& lex);
	bool Poliz(int i, LexAnalisys::LA& lex);
}
