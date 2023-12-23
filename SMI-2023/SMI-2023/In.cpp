#include "stdafx.h"
#include "In.h"
#include "Error.h"
#include "Parm.h"


namespace In
{
	IN getin(Parm::PARM parm)
	{
		IN in;

		int i = 0, position = 0;
		bool isLineLit = false;
		in.size = 0;
		in.lines = 0;
		in.ignor = 0;
		in.text = new unsigned char[IN_MAX_LEN_TEXT];


		ifstream file(parm.in);
		if (file.fail())
			throw ERROR_THROW(102);


		while (in.size < IN_MAX_LEN_TEXT)
		{
			char c;
			file.get(c);
			unsigned char letter = c;

			if (file.eof())
			{
				in.text[in.size] = '\0';
				break;
			}

			switch (in.code[(int)letter])
			{
			case in.BL:
			{
				in.lines++;
				position = 0;
				in.text[i] = letter;
				i++;
				in.size++;
				break;
			}
			case in.L:
				isLineLit = !isLineLit;
				in.text[i] = letter;
				i++;
				in.size++;
				position++;
				break;
			case in.C:
			{
				if (!isLineLit)
				{
					while (!file.eof() && in.code[(int)letter] != in.BL)
					{
						letter = file.get();
						in.ignor++;
					}
					in.lines++;
					position = 0;
					in.text[i] = letter;
					i++;
					in.size++;
				}
				else
				{
					in.text[i] = letter;
					i++;
					in.size++;
					position++;
				}
				break;
			}
			case in.T:
			{
				in.text[i] = letter;
				i++;
				in.size++;
				position++;
				break;
			}
			case in.F:
			{
				throw ERROR_THROW_IN(200, in.lines + 1, position);
				break;
			}
			default:
			{
				in.text[i] = in.code[(int)letter];
				i++;
				in.size++;
				position++;
				break;
			}
			}
		}
		file.close();
		in.text[i] = '\0';
		return in;
	}
}
