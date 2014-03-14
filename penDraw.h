#pragma once

class CPenDraw
{
public:
	CPenDraw(void);
	~CPenDraw(void);

public:
	// 存放实时书写时的笔迹点坐标
	unsigned short *m_pPntData;
	// 存放实时书写时的笔迹点数
	unsigned short m_unPntNum;
	// 每个字的笔画数
	unsigned short m_unStrokeNum;

	// 手写识别区域坐标
	const unsigned short m_unBottom;
	const unsigned short m_unRight;

public:
	// 实时书写时
	BOOL m_bWriting;
	// 书写时笔在纸上
	BOOL m_bPenDown;
	////BOOL m_bMouseMoveInClient; // 实时书写时，确定是否在采样区内

public:
	void StartWriting();
	void EndWriting(const int nSavePwt, const TCHAR *pszPwtFile);
	//void DisplayPoint(BOOL bMove);
	//void PowerOn(BOOL on);
	void AddOnePoint(const unsigned short x, const unsigned short y);
	void EndOneStroke();
	void Reset();
	void SavePwtFile(const TCHAR *pszPwtFile);

};
