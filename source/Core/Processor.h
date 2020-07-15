#ifndef CL_PROCESSOR_H
#define CL_PROCESSOR_H

#include <sstream>
#include "Inputs.h"

/*	Maximum number of argument stored before processed automatically by operator assignment
*/
#define OS_MAX_ARG_COUNT 5
#define OS_MAX_ARG_INDEX (OS_MAX_ARG_COUNT - 1)
#define OS_FIRST_ARG 0
#define OS_SECOND_ARG 1
#define OS_THIRD_ARG 2

typedef uint16_t unum;
typedef int16_t	snum;

class Processor
{
	enum Error
	{
		OPERATION_SUCESSFUL = 0,
		OPERATION_EXCEPTION_DIV_ZERO = -1,
		OPERATION_EXCEPTION_DIV_ZERO_INF = -2
	};

	enum Status
	{
		INPUT_NONE = -1,
		INPUT_NUMBER,
		INPUT_INVERSE_NUMBER,
		INPUT_OPERATOR,
	};

	const string k_exceptionMsgDivZero		= "Divided by Zero, result : undefinition";
	const string k_exceptionMsgDivZeroInf	= "Zero divided by Zero, result : infinity";

public:
	Processor();
	~Processor();

	void AssignNumber(unum num);
	void AssignOperator(snum op);
	void Clear();

	const string	GetText() { return m_text; }
	const char*		GetTextC() { return m_text.c_str(); }

private:
	template <typename _T>
	int	ProcessResult(_T* argIO);

	template<typename _T>
	void ShiftLeft(_T* data, int start, int size, _T default);

	bool IsHighPriority(snum op);
	void AssignStreamToValue();
	void AssignValueToStream();
	void ResetStream(bool clearText = false);

private:
	union
	{
		int		m_iArg[OS_MAX_ARG_COUNT];
		float	m_fArg[OS_MAX_ARG_COUNT];
	};

	stringstream	m_valueStr;
	string			m_text;

	bool m_bIsFloatingNumber;

	short	m_argIdx;
	snum	m_lastInput;
	snum	m_operator[OS_MAX_ARG_COUNT];
	int		m_lastError;
};

#endif // !CL_PROCESSOR_H

template<typename _T>
inline int Processor::ProcessResult(_T* argIO)
{
	int leftArg, rightArg;

	while (m_argIdx >= OS_SECOND_ARG)
	{
		if (!IsHighPriority(m_operator[OS_SECOND_ARG]) || IsHighPriority(m_operator[OS_FIRST_ARG]) || m_argIdx == OS_SECOND_ARG)
		{
			leftArg = OS_FIRST_ARG;
			rightArg = OS_SECOND_ARG;
		}
		else
		{
			leftArg = OS_SECOND_ARG;
			rightArg = OS_THIRD_ARG;
		}

		try
		{
			switch (m_operator[leftArg])
			{
			case Inputs::Op_Multiplication:
				argIO[leftArg] *= argIO[rightArg];
				break;
			case Inputs::Op_Division:
				if (argIO[rightArg] == 0)
					throw argIO[leftArg] == 0 ? Error::OPERATION_EXCEPTION_DIV_ZERO_INF : Error::OPERATION_EXCEPTION_DIV_ZERO;

				argIO[leftArg] /= argIO[rightArg];
				break;
			case Inputs::Op_Addition:
				argIO[leftArg] += argIO[rightArg];
				break;
			case Inputs::Op_Substraction:
				argIO[leftArg] -= argIO[rightArg];
				break;
			default:
				break;
			}
		}
		catch (Error e)
		{
			return e;
		}

		ShiftLeft<_T>(argIO, rightArg, OS_MAX_ARG_COUNT, 0);
		ShiftLeft<snum>(m_operator, leftArg, OS_MAX_ARG_COUNT, (snum)Inputs::Op_None);

		m_argIdx--;
	}

	return Error::OPERATION_SUCESSFUL;
}

template<typename _T>
void Processor::ShiftLeft(_T* data, int start, int size, _T default)
{
	for (int i = start; i < size - 1; i++)
	{
		data[i] = data[i + 1];
	}

	data[size - 1] = default;
}