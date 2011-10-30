
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

    bool ReadSetting();

	bool WriteSetting(const char* section,
        			  const char* param,
		        	  const char* subparam,
		        	  const char* value);

	bool WriteComment(const char* comment);

    static const int m_nSectionSize=256;
    static const int m_nParamSize=256;
    static const int m_nSubParamSize=256;
    static const int m_nValueSize=1024;

    char m_strSection[m_nSectionSize];
    char m_strParam[m_nParamSize];
    char m_strSubParam[m_nSubParamSize];
    char m_strValue[m_nValueSize];

    bool m_bSectionChanged;

  protected:
	FILE *m_file;
	bool m_bWrite;
	bool m_bLastLineComment;
	char m_strLastSection[m_nSectionSize];
};

#endif // _SETTINGS_H_
