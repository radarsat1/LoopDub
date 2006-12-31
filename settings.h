
#ifndef _SETTINGS_H_
#define _SETTINGS_H_

class SettingsFile
{
  public:
	SettingsFile();
	SettingsFile(char *filename);
	~SettingsFile();

	bool OpenForRead(char *filename);
	bool OpenForWrite(char *filename);
	void Close();

	bool IsOpen() { return m_file!=NULL; }
	bool IsOpenForWrite() { return IsOpen() && m_bWrite; }
	bool IsOpenForRead() { return IsOpen() && !m_bWrite; }

	bool ReadSetting(char *section,  int sectionSize,
			 char *param,    int paramSize,
			 char *subparam, int subparamSize,
			 char *value,    int valueSize);

	bool WriteSetting(const char* section,
			  const char* param,
			  const char* subparam,
			  const char* value);

	bool WriteComment(const char* comment);
	
  protected:
	FILE *m_file;
	bool m_bWrite;
	bool m_bLastLineComment;
	char m_strSection[256];
};

#endif // _SETTINGS_H_
