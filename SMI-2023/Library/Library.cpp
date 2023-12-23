#include <iostream>

extern "C"
{
	int RANDOM(int min, int max)
	{
		srand(time(0));
		return rand() % (max - min) + min;
	}

	int SQRT(int value)
	{
		return (int)sqrt(value);
	}

	int LINELEN(char* str)
	{
		int length = 0;
		for (int i = 0; str[i] != '\0'; i++)
		{
			if (i == 255)
				break;
			length++;
		}
		return length;
	}

	char* CONCAT(char* buffer, char* str1, char* str2)
	{
		int i = NULL, len1 = NULL, len2 = NULL;

		for (; str1[len1] != '\0'; len1++);
		for (; str2[len2] != '\0'; len2++);

		for (int j = 0; str1[j] != '\0'; j++)
		{
			if (i == 255)
				break;
			buffer[i++] = str1[j];
		}
		for (int j = 0; str2[j] != '\0'; j++)
		{
			if (i == 255)
				break;
			buffer[i++] = str2[j];
		}
		buffer[i] = '\0';
		return buffer;
	}

	void OutNumberLn(int a)
	{
		std::cout << a << std::endl;
	}

	void OutLineLn(char* ptr)
	{
		if (ptr == nullptr)
		{
			std::cout << std::endl;
			return;
		}
		for (int i = 0; ptr[i] != '\0'; i++)
		{
			std::cout << ptr[i];
		}
		std::cout << std::endl;
	}
	void OutBoolLn(int b)
	{
		if (b)
			std::cout << "true" << std::endl;
		else
			std::cout << "false" << std::endl;
	}

	void OutNumber(int a)
	{
		std::cout << a;
	}

	void OutLine(char* ptr)
	{
		setlocale(LC_ALL, "Rus");
		if (ptr == nullptr)
		{
			std::cout << std::endl;
			return;
		}
		for (int i = 0; ptr[i] != '\0'; i++)
		{
			std::cout << ptr[i];
		}
	}
	void OutBool(int b)
	{
		if (b)
			std::cout << "true";
		else
			std::cout << "false";
	}
}