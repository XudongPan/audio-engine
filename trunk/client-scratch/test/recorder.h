#pragma once


class FileRecorder {
public:
	FileRecorder();
	virtual ~FileRecorder();

	 int FileName(char* file_name_utf8,
		size_t size) const ;

	 bool Open() const ;

	 int OpenFile(const char* file_name_utf8,
		bool read_only,
		bool loop = false,
		bool text = false) ;

	 int OpenFromFileHandle(FILE* handle,
		bool manage_file,
		bool read_only,
		bool loop = false) ;

	 int CloseFile() ;
	 int SetMaxFileSize(size_t bytes) ;
	 int Flush() ;

	 int Read(void* buf, int length) ;
	 bool Write(const void* buf, int length) ;
	 int WriteText(const char* format, ...) ;
	 int Rewind() ;

	static const size_t kMaxFileNameSize = 1024;
private:
	int CloseFileImpl();
	int FlushImpl();


	FILE* id_;
	bool managed_file_handle_;
	bool open_;
	bool looping_;
	bool read_only_;
	size_t max_size_in_bytes_;  // -1 indicates file size limitation is off
	size_t size_in_bytes_;
	char file_name_utf8_[kMaxFileNameSize];
};
