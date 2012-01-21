//////////////////////////////////////////////////////////////////////////////
// Project...: drax
// File......: Chapter.h
// Date......: 21.11.2008
// Author....: Stephan Arlt
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TimeSpanEx.h"

class CChapter : public CObject
{
public: // ignore information hiding :(
	CTimeSpanEx m_kTime;
	CString m_sName;

public:
	CChapter();
	virtual ~CChapter();
};
