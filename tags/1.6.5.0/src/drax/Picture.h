//////////////////////////////////////////////////////////////////////////////
// Project...: drax
// File......: Picture.h
// Date......: 22.11.2008
// Author....: Stephan Arlt
//////////////////////////////////////////////////////////////////////////////

#pragma once

class CPicture : public CObject
{
public:
	CPicture(UINT piID, int piX, int piY);
	virtual ~CPicture();

	void Paint(CDC *ppkDC);

protected:
	IPicture *m_pkPicture;
	int m_iX;
	int m_iY;
};
