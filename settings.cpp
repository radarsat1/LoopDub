
#include <stdio.h>
#include <string.h>
#include "settings.h"

/*
int main(int argc, char *argv[])
{
	if (argc >= 2) {
		printf("Reading %s...\n", argv[1]);

		SettingsFile f(argv[1]);
		if (!f.IsOpen()) {
			printf("Error opening file.\n");
			return 1;
		}
		printf("File opened.\n");
	
		char param[256];
		char subparam[256];
		char section[256];
		char value[1024];
		while (f.ReadSetting(section,256,param,256,subparam,256,value,1024))
		{
			printf("%s%s%s%s%s = %s\n",
				section[0]?section:"",
				section[0]?".":"",
				param,
				subparam[0]?".":"",
				subparam[0]?subparam:"",
				value);
		}
	}
	else
	{
		printf("Writing to test.conf...\n");

		SettingsFile f;
		if (!f.OpenForWrite("test.conf"))
		{
			printf("Error opening file.\n");
			return 1;
		}
		printf("File opened.\n");

		f.WriteSetting("Section1", "OneParam", 0, "Nothing!");	
		f.WriteSetting("Section1", "TwoParam", "3", "Nothing!");
		f.WriteSetting("Section1", "ThreeParam", 0, "Nothing!");
		f.WriteSetting("Section2", "OneParam", 0, "Mep?");
		f.WriteSetting("Section2", "TwoParam", "seventy", "Seven!");
		f.WriteComment("Die, suckah.");
		f.WriteSetting("Section3", "Blah", "9", "Wicked.");
	}
	
	return 0;
}
*/

SettingsFile::SettingsFile()
{
	m_file = 0;
	m_bWrite = false;
}

SettingsFile::SettingsFile(char *filename)
{
	m_file = 0;
	m_bWrite = false;
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
	if (!IsOpenForRead())
		return false;
	
	char line[4096];
	bool done=false;
	char *s;

	section[0]  = 0;
	param[0]    = 0;
	subparam[0] = 0;
	value[0]    = 0;

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
				strncpy(m_strSection, s, 256);
				break;
			default:
				s = strtok(line, "=");
				strncpy(param, s, 256);
				s = strtok(NULL, " =\r\n");
				strncpy(value, s, 256);
				s = strtok(param, ".");
				s = strtok(NULL, ".");
				if (s) strncpy(subparam, s, 256);
				done = true;
				break;
		}

		strncpy(section, m_strSection, (sectionSize<256)?sectionSize:256);
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
		strcasecmp(section, m_strSection)!=0)
	{
		strncpy(m_strSection, section, 256);
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

	
