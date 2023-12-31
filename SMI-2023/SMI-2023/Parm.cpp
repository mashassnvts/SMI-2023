#include "stdafx.h"
#include "Parm.h"
#include "Error.h"

namespace Parm
{
	PARM getparm(int argc, wchar_t* argv[])
	{
		PARM parm = { L"", L"", L"", false, false, false };
		bool IN_FOUND = false, OUT_FOUND = false, LOG_FOUND = false;

		for (int i = 1; i < argc; i++)
		{
			if (wcslen(argv[i]) > PARM_MAX_SIZE)
				throw ERROR_THROW(101);
			if (wcsstr(argv[i], PARM_IN))
			{
				wcscpy_s(parm.in, argv[i] + wcslen(PARM_IN));
				IN_FOUND = true;
			}
			if (wcsstr(argv[i], PARM_OUT))
			{
				wcscpy_s(parm.out, argv[i] + wcslen(PARM_OUT));
				OUT_FOUND = true;
			}
			if (wcsstr(argv[i], PARM_LOG))
			{
				wcscpy_s(parm.log, argv[i] + wcslen(PARM_LOG));
				LOG_FOUND = true;
			}

			if (!_wcsnicmp(argv[i], PARM_ID, wcslen(PARM_ID)))
				parm.idtable = true;
			if (!_wcsnicmp(argv[i], PARM_LT, wcslen(PARM_LT)))
				parm.lextable = true;
			if (!_wcsnicmp(argv[i], PARM_POLIZ, wcslen(PARM_POLIZ)))
				parm.poliz = true;
		}

		if (!IN_FOUND)
			throw ERROR_THROW(100);
		if (!OUT_FOUND)
		{
			wcscpy_s(parm.out, parm.in);
			wcscat_s(parm.out, PARM_OUT_DEFAULT_EXT);
		}
		if (!LOG_FOUND)
		{
			wcscpy_s(parm.log, parm.in);
			wcscat_s(parm.log, PARM_LOG_DEFAULT_EXT);
		}
		return parm;
	}
}