/******************************************************************
/******************************************************************
* Function: Collect the mouth callback
* Description: 
* Commit: Yejun Tang, Yang Hu
* Call:
* Call By:
******************************************************************/

#include "StdAfx.h"
#include "PenDraw.h"

CPenDraw::CPenDraw(void) : 
	m_unBottom(256), m_unRight(512), 
	m_bWriting(FALSE), m_bPenDown(FALSE)
{
	m_unPntNum = m_unStrokeNum = 0;
	m_pPntData = new unsigned short [CHCRDEMO_MAX_POINT_LEN];
}

CPenDraw::~CPenDraw(void)
{
	delete [] m_pPntData;
}

void CPenDraw::StartWriting()
{
	//if (m_unPntNum > 0)
	//{
	//	if ((m_pPntData[2 * m_unPntNum - 2] != 0xffff) && 
	//		(m_pPntData[2 * m_unPntNum - 1] != 0))
	//	{
	//		AfxMessageBox("采样点格式错误");
	//	}
	//}

	// 在绘图区域左键按下，标记开始绘图
	if (!m_bWriting)
	{
		// 开始写第一笔
		m_bWriting = m_bPenDown = TRUE;
	}
	else
	{
		// 继续写
		m_bPenDown = TRUE;
	}

	//PowerOn(FALSE);
}

void CPenDraw::EndWriting(const int nSavePwt, const TCHAR *pszPwtFile)
{
	// 加进字终码
	// 加入笔终码
	if (m_unPntNum > 0)
	{
		unsigned short *p = m_pPntData + 2 * m_unPntNum;
		*p++ = 0xffff;
		*p = 0xffff;
		// 实时点数加1
		m_unPntNum++;
		// 保存笔迹文件
		if (nSavePwt)
		{
			SavePwtFile(pszPwtFile);
		}
		m_bWriting = FALSE;
	}
}

void CPenDraw::AddOnePoint(const unsigned short x, const unsigned short y)
{
	if (m_unPntNum + 1 >= CHCRDEMO_MAX_POINT_NUM)
	{
		// 考虑结束符在内
		return;
	}
	unsigned short *p = m_pPntData + 2 * m_unPntNum;
	*p++ = x;
	*p = y;
	// 实时点数加1
	m_unPntNum++;
}

void CPenDraw::EndOneStroke()
{
	if (!m_bPenDown)
	{
		return;
	}

	// 加入笔划终码
	unsigned short *p = m_pPntData + 2 * m_unPntNum;
	if ((m_unPntNum > 0) && 
		(*(m_pPntData - 2) != 0xffff) && 
		(*(m_pPntData - 1) != 0))
	{
		// 每笔有有效点时方加笔终码
		*p++ = 0xffff; 
		*p = 0;
		// 实时点数加1
		m_unPntNum++;
		// 笔画数加1
		m_unStrokeNum++;
	}
	m_bPenDown = FALSE;
}

// 清除笔迹数据
void CPenDraw::Reset()
{
	m_unPntNum = m_unStrokeNum = 0;
}

void CPenDraw::SavePwtFile(const TCHAR *pszPwtFile)
{
	pwtheader pwtHeader;
	pwtHeader.patternlen = m_unPntNum * 2 * sizeof(unsigned short) + 72;
	pwtHeader.charlen = 1;
	pwtHeader.innercode[0] = (unsigned short)('N');
	pwtHeader.innercode[1] = 0;
	pwtHeader.strokenum = m_unStrokeNum;
	pwtHeader.reserve = 0;

	FILE *fp = 0;
	if (!(fp = _tfopen(pszPwtFile, _T("wb"))))
	{
		return;
	}
	fwrite(&pwtHeader, sizeof(pwtheader), 1, fp);
	fwrite(m_pPntData, sizeof(unsigned short), m_unPntNum * 2, fp);
	fclose(fp);
}
