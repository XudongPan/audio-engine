
#include "include\all_in_one.h"

#include "recorder.h"

using namespace VoIP; 

class receiver : public Transport
{
public:
	receiver();
	~receiver();
	virtual int32_t SendRecPacket(int channel, AudioFrame & recFrame);

	FileRecorder * file;
};


void main_test();

void init();
void SetEnv();
void StartPlayout();
void Stop();
void DeInit();
