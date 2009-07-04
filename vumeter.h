
#ifndef _VUMETER_H_
#define _VUMETER_H_

#include "gui/scrobui.h"

////////////////////////////////////////////////////////
// VUMeter -> display level of sound

class VUMeter : public Scrob
{
public:
	VUMeter() : Scrob() {}
	VUMeter(Scrob* pParent, const Rect& r);
	virtual ~VUMeter() {}

public:
	virtual void Draw();
	virtual bool Create(Scrob *pParent, const Rect& r);

	void SetPercentage(int percent);

protected:

    static const unsigned int history_size = 30;

	char m_nPercentage;
	char m_History[history_size];
	int m_nHistoryPos;
};

#endif // _VUMETER_H_
