#pragma once

#include <vector>
#include <map>
#include <cvt/wstring>
typedef std::codecvt<wchar_t, char, std::mbstate_t> cvt_t;

#define BLOCK_X 8
#define BLOCK_Y 12
#define LBLOCK_X BLOCK_X * 2
#define LBLOCK_Y BLOCK_Y * 2


class CDigitPattern
{
public:
	CDigitPattern();
	virtual ~CDigitPattern();

	void GenerateDigitTargetPattern(int nOutputNode);
	void ApplyPreprocessing(MatrixXb* pmtrxGridBlocks = NULL);
	void ApplyPreprocessing2(MatrixXb* pmtrxGridBlocks=NULL);

public:
	int nDigit;
	CString szPattern;
	std::vector<int> vecOriginalPattern;
	std::vector<int> vecPreprocessedPattern;
	std::vector<int> vecTargetPattern;
};

typedef std::multimap<std::size_t/*hash key*/, CDigitPattern>::value_type PATTERNST;

std::size_t GenerateHash(CString& label);
std::size_t GenerateHash(CString& label, std::vector<int>& vec);
