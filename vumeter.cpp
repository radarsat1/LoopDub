
#include "vumeter.h"
#include <string.h>

/*
** VUMeter
*/

VUMeter::VUMeter(Scrob* pParent, const Rect& r)
	 : Scrob(pParent, r)
{
	 Create(pParent, r);
}

bool VUMeter::Create(Scrob *pParent, const Rect& r)
{
	if (!Scrob::Create(pParent, r))
		return false;

	m_nPercentage = 60;
	memset(m_History, 0, sizeof(char)*history_size);
	m_nHistoryPos = 0;

	return true;
}

void VUMeter::Draw()
{
	 dt.SetCurrentObject(this);
	 dt.DrawRect(Rect(0, 0, m_Rect.Width(), m_Rect.Height()), 3);
	 int x = m_nPercentage * (m_Rect.Width()-2) / 100;
	 dt.FillRect(Rect(1, 1, x+1, m_Rect.Height()-1), 2);
	 dt.FillRect(Rect(x+1, 1, m_Rect.Width()-1, m_Rect.Height()-1), 1);

	 int percent_max=0;
	 for (int i=0; i<history_size; i++)
		  if (m_History[i] > percent_max)
			   percent_max = m_History[i];

	 x = percent_max * (m_Rect.Width()-2) / 100;
	 dt.FillRect(Rect(x+1, 1, x+2, m_Rect.Height()-1), 3);
}

void VUMeter::SetPercentage(int percent)
{
	 if (percent > 100) percent = 100;
	 if (percent < 0) percent = 0;
	 m_nPercentage=percent;
	 m_History[m_nHistoryPos] = percent;
	 m_nHistoryPos = (m_nHistoryPos+1)%history_size;
	 SetDirty();
}

