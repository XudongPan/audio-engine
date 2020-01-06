#include "stdafx.h"
#include "filerecorder.h"


filerecorder::filerecorder(void)
{
}


filerecorder::~filerecorder(void)
{
}

int filerecorder::OpenFile( const char* file_name_utf8, bool read_only )
{
	if (id_ != NULL && !managed_file_handle_)
		return -1;
	size_t length = strlen(file_name_utf8);
	if (length > 1000) {
		return -1;
	}

	read_only_ = read_only;

	FILE* tmp_id = NULL;

	wchar_t wide_file_name[kMaxFileNameSize];
	wide_file_name[0] = 0;

	MultiByteToWideChar(CP_UTF8,
		0,  // UTF8 flag
		file_name_utf8,
		-1,  // Null terminated string
		wide_file_name,
		kMaxFileNameSize);
	if (text) {
		if (read_only) {
			tmp_id = _wfopen(wide_file_name, L"rt");
		} else {
			tmp_id = _wfopen(wide_file_name, L"wt");
		}
	} else {
		if (read_only) {
			tmp_id = _wfopen(wide_file_name, L"rb");
		} else {
			tmp_id = _wfopen(wide_file_name, L"wb");
		}
	}


	if (tmp_id != NULL) {
		// +1 comes from copying the NULL termination character.
		memcpy(file_name_utf8_, file_name_utf8, length + 1);
		if (id_ != NULL) {
			fclose(id_);
		}
		id_ = tmp_id;
		managed_file_handle_ = true;
		looping_ = loop;
		open_ = true;
		return 0;
	}
	return -1;
}

int filerecorder::CloseFile()
{

}
