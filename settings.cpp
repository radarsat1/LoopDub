
#include <stdio.h>
#include <string.h>
#include "settings.h"

SettingsFile::SettingsFile()
{
	m_file = 0;
	m_bWrite = false;
    m_strSection[0] = 0;
    m_strParam[0] = 0;
    m_strSubParam[0] = 0;
    m_strValue[0] = 0;
}

SettingsFile::SettingsFile(char *filename)
{
    m_file = 0;
	m_bWrite = false;
    m_strSection[0] = 0;
    m_strParam[0] = 0;
    m_strSubParam[0] = 0;
    m_strValue[0] = 0;
	OpenForRead(filename);
}

SettingsFile::~SettingsFile()
{
	Close();
}

bool SettingsFile::OpenForRead(char *filename)
{
	if (IsOpen()) return false;

	m_file = fopen(filename, "r");
	if (!m_file) return false;

	m_bWrite = false;
    m_strLastSection[0] = 0;
    m_bSectionChanged = false;
	
    return true;
}

bool SettingsFile::OpenForWrite(char *filename)
{
	if (IsOpen()) return false;

	m_file = fopen(filename, "w");
	if (!m_file) return false;

	m_bWrite = true;
	m_bLastLineComment = false;
	
	return true;
}

void SettingsFile::Close()
{
	if (!IsOpen())
		return;

	fclose(m_file);
	m_file = 0;
}

bool SettingsFile::ReadSetting(	char *section,  int sectionSize,
                                char *param,    int paramSize,
                				char *subparam, int subparamSize,
			                	char *value,    int valueSize )
{
        bool rc = ReadSetting();
        if (rc) {
                strncpy(section,  m_strSection,  sectionSize);
                strncpy(param,    m_strParam,    paramSize);
                strncpy(subparam, m_strSubParam, subparamSize);
                strncpy(value,    m_strValue,    valueSize);
        }
        return rc;
}

bool SettingsFile::ReadSetting()
{
	if (!IsOpenForRead())
		return false;
	
	char line[4096];
	bool done=false;
	char *s;

	m_strSection[0]  = 0;
	m_strParam[0]    = 0;
	m_strSubParam[0] = 0;
	m_strValue[0]    = 0;
    m_bSectionChanged = false;

	while (!done) {
		if (!fgets(line, 4096, m_file))
			return false;

		switch (line[0]) {
			case ';':
			case '\r':
			case '\n':
			case 0:
				break;
			case '[':
				s = strtok(line, "[]\r\n");
				strncpy(m_strLastSection, s, m_nSectionSize);
                m_bSectionChanged = true;
				break;
			default:
				s = strtok(line, "=");
				strncpy(m_strParam, s, m_nParamSize);
				s = strtok(NULL, " =\r\n");
				strncpy(m_strValue, s, m_nValueSize);
				s = strtok(m_strParam, ".");
				s = strtok(NULL, ".");
				if (s) strncpy(m_strSubParam, s, m_nSubParamSize);
				done = true;
				break;
		}

		strncpy(m_strSection, m_strLastSection, m_nSectionSize);
	}
	
	return true;
}

bool SettingsFile::WriteSetting(const char* section,
				const char* param,
				const char* subparam,
				const char* value)
{
	if (!IsOpenForWrite())
		return false;

	if (section && section[0] &&
		strcasecmp(section, m_strLastSection)!=0)
	{
		strncpy(m_strLastSection, section, m_nSectionSize);
		fprintf(m_file, "\n[%s]\n", section);
	}
	
	fprintf(m_file, "%s%s%s = %s\n",
			param,
			(subparam && subparam[0])?".":"",	
			(subparam && subparam[0])?subparam:"",
			value);

	m_bLastLineComment = false;
	return true;
}

bool SettingsFile::WriteComment(const char* comment)
{
	if (!IsOpenForWrite())
		return false;

	if (!m_bLastLineComment)
		fprintf(m_file, "\n");
	
	fprintf(m_file, "; %s\n", comment);
	
	m_bLastLineComment = true;
	return true;
}

	
