#pragma once

class filerecorder
{
public:
	filerecorder(void);
	virtual ~filerecorder(void);
	int OpenFile(const char* file_name_utf8, bool read_only); 
	int CloseFile();
};

