#pragma once

#include "stdafx.h"
#include "Error.h"
#include "LT.h"

#define ID_MAXSIZE				10
#define ID_PREFIX_MAXSIZE		21
#define TI_MAXSIZE				4096
#define TI_NUM_DEFAULT			0x00000000
#define TI_LINE_DEFAULT			0x00
#define TI_NULLIDX				0xffffffff
#define TI_LINE_MAXSIZE			255													
#define MAX_PARAMS_COUNT		3
#define CONCAT_PARAMS_CNT		2
#define LINELEN_PARAMS_CNT		1
#define RANDOM_PARAMS_CNT		2
#define SQRT_PARAMS_CNT			1

#define CONCAT_TYPE IT::IDDATATYPE::LINE
#define LINELEN_TYPE IT::IDDATATYPE::NUM
#define RANDOM_TYPE IT::IDDATATYPE::NUM
#define SQRT_TYPE IT::IDDATATYPE::NUM

namespace IT
{
	enum class IDDATATYPE { NUM = 1, LINE = 2, PROC = 3, BOOL = 4, UNKNOWN };
	enum class IDTYPE {
		V = 1, F = 2, P = 3, L = 4, S = 5
	};

	static const IDDATATYPE CONCAT_PARAMS[] = { IT::IDDATATYPE::LINE, IT::IDDATATYPE::LINE };
	static const IDDATATYPE LINELEN_PARAMS[] = { IT::IDDATATYPE::LINE };
	static const IDDATATYPE RANDOM_PARAMS[] = { IT::IDDATATYPE::NUM, IT::IDDATATYPE::NUM };
	static const IDDATATYPE SQRT_PARAMS[] = { IT::IDDATATYPE::NUM };

	struct Entry
	{
		int			idxfirstLE;
		unsigned char id[ID_PREFIX_MAXSIZE];
		IDDATATYPE	iddatatype = IDDATATYPE::UNKNOWN;
		IDTYPE		idtype;
		unsigned char visibility[ID_MAXSIZE];

		struct
		{
			int vint;
			struct
			{
				int len;
				unsigned char str[TI_LINE_MAXSIZE - 1];
			} vstr;

			struct
			{
				int count;
				IDDATATYPE* types;
			} params;
		} value = { LT_TI_NULLIDX };
	};

	struct IdTable
	{
		int maxsize;
		int size;
		Entry* table;
	};

	IdTable Create(
		int size
	);

	void Add(
		IdTable& idtable,
		Entry entry
	);

	Entry GetEntry(
		IdTable& idtable,
		int n
	);

	int IsId(
		IdTable& idtable,
		unsigned char id[ID_MAXSIZE]
	);

	void Delete(IdTable& idtable);

	void WriteTable(IdTable& idtable);
};