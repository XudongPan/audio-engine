 #include "stdafx.h"

#include "record_test.h"


/*#pragma (disable:4996)*/

using namespace VoIP;

AudioEngine* aoe = NULL;
AoEBase* aoe_base = NULL;
AoEAudioProcessing* aoe_audio_processor = NULL;
AoEHardware* aoe_hardware = NULL;
AoEIO * aoe_io = NULL;

int _audio_channel = -1;


receiver rec_receiver;

int32_t receiver::SendRecPacket(int channel, AudioFrame & recFrame)
{
	// record routine
	aoe_io->ReceivedPlayoutData(channel, recFrame);

	if (file->Open())
	{
		file->Write(recFrame.data_, sizeof(recFrame.data_)/(2*sizeof(int16_t)));
		file->Flush();
	}
	
	return 0;
}

receiver::receiver()
{
	file = new FileRecorder;
	file->OpenFile("c:\\work\\123.pcm", false);
}

receiver::~receiver()
{
	file->CloseFile();
}

void init(){

	aoe = AudioEngine::Create();
	if(NULL == aoe)return;

	aoe_base = AoEBase::GetInterface(aoe);
	aoe_audio_processor = AoEAudioProcessing::GetInterface(aoe);
	aoe_hardware = AoEHardware::GetInterface(aoe);
	aoe_io = AoEIO::GetInterface(aoe);
			
}

void SetEnv()
{
	aoe_base->Init();

	_audio_channel = aoe_base->CreateChannel();

	bool enable_aec = true;
	bool enable_agc = true;
	bool enable_ns = true;

 	aoe_audio_processor->SetAgcStatus(enable_agc);
 	aoe_audio_processor->SetNsStatus(enable_ns);
	aoe_audio_processor->SetEcStatus(enable_aec);

	aoe_io->RegisterExternalTransport(_audio_channel, rec_receiver);
	
}


void StartPlayout(){

	aoe_base->StartSend(_audio_channel);
	aoe_base->StartReceive(_audio_channel);
	aoe_base->StartPlayout(_audio_channel);

	system("PAUSE");

}

void Stop()
{
	aoe_base->StopPlayout(_audio_channel);
	aoe_base->StopSend(_audio_channel);
	aoe_base->StopReceive(_audio_channel);
	aoe_base->Terminate();
}

void DeInit(){	
	
	if (aoe_base)
		aoe_base->Release();

	if (aoe_audio_processor)
		aoe_audio_processor->Release();

	if (aoe_hardware)
		aoe_hardware->Release();

	if (aoe_io)
		aoe_io->Release();
	
	AudioEngine::Delete(aoe);
}

void main_test()
{
	init();
	SetEnv();
	StartPlayout();
	Stop();
	DeInit();
}

// int _tmain(int argc, _TCHAR* argv[])
//{
//	main_test();
//}