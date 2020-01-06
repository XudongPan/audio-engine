 #include "stdafx.h"
#include "include\all_in_one.h"
#include "system_wrappers\interface\scoped_ptr.h"

#include "recorder.h"

/*#pragma (disable:4996)*/

using namespace VoIP;

AudioEngine* m_aoe = NULL;
AoEBase* base1 = NULL;
AoEAudioProcessing* apm = NULL;
AoEHardware* hardware = NULL;
AoEIO * io = NULL;

int res = 0;
int _channel = -1;

#define VALIDATE                                           \
	EXPECT_EQ(res, 0);if (res != 0) {                                          \
	printf("*** Error at line %i \n", __LINE__);           \
	printf("*** Error code = %i \n", base1->LastError());  \
}



class proxy : public Transport
{
public:
// 	virtual int SendPacket(int channel, const void * data, int length);
// 	virtual int SendCtrlPacket(int channel, const void *data, int length){return 0;}
	proxy();
	~proxy();
	virtual int32_t SendRecPacket(int channel, AudioFrame & recFrame);

	FileRecorder * file;
};

int g_num_of_frame = 0;
int pkg_length = 0;

int32_t proxy::SendRecPacket(int channel, AudioFrame & recFrame)
{
	printf("size = %d \r\n", sizeof(recFrame.data_)/sizeof(int16_t));
	printf("vad status = %d \r\n", recFrame.vad_activity_);

	// record routine
	io->ReceivedPlayoutData(channel, recFrame);
	if (file->Open())
	{
		file->Write(recFrame.data_, sizeof(recFrame.data_)/(2*sizeof(int16_t)));
		file->Flush();
	}

// 
// 	int8_t audioSamples[1024] = {0};
// 	AudioFrame playoutFrame;
// 

// 	if (file->Open())

// 	{

// 		int16_t fileBuf[480];

// 

// 		// read mono-file

// 		int32_t len = file->Read((int8_t*) fileBuf, 2

// 			* recFrame.samples_per_channel_);

// 		if (len != 2 * (int32_t) recFrame.samples_per_channel_)

// 		{

// 			file->Rewind();

// 			file->Read((int8_t*) fileBuf, 2 * recFrame.samples_per_channel_);

// 		}

// 

// 		// convert to stero if required

// 		if (recFrame.num_channels_ == 1)

// 		{

// 			memcpy(audioSamples, fileBuf, 2 * recFrame.samples_per_channel_);

// 		} else

// 		{

// 			// mono sample from file is duplicated and sent to left and right

// 			// channels

// 			int16_t* audio16 = (int16_t*) audioSamples;

// 			for (unsigned int i = 0; i < recFrame.samples_per_channel_; i++)

// 			{

// 				(*audio16) = fileBuf[i]; // left

// 				audio16++;

// 				(*audio16) = fileBuf[i]; // right

// 				audio16++;

// 			}

// 		}

// 	}  // if (_playFromFile && _playFile.Open())


// 


// 	playoutFrame.CopyFrom(recFrame);


// 	playoutFrame.UpdateFrame(recFrame.id_, recFrame.timestamp_, 


// 						(int16_t*)audioSamples, recFrame.samples_per_channel_,


// 						recFrame.sample_rate_hz_, recFrame.speech_type_,


// 						recFrame.vad_activity_);


// 


// 	io->ReceivedPlayoutData(playoutFrame.id_, playoutFrame



	return 0;
}

proxy::proxy()
{
	file = new FileRecorder;
	file->OpenFile("c:\\work\\123.pcm", false);
}

proxy::~proxy()
{
	file->CloseFile();
}

proxy rec_receiver;


TEST(AudioEngineTest, Init){
	m_aoe = AudioEngine::Create();
	ASSERT_TRUE((m_aoe != NULL));

	ASSERT_TRUE((base1 = AoEBase::GetInterface(m_aoe))!=NULL);
	ASSERT_TRUE((apm = AoEAudioProcessing::GetInterface(m_aoe))!=NULL);
	ASSERT_TRUE((hardware = AoEHardware::GetInterface(m_aoe))!=NULL);
	ASSERT_TRUE((io = AoEIO::GetInterface(m_aoe))!=NULL);
		
}
TEST(AudioEngineTest, SetEnv){
	ASSERT_EQ(base1->Init(), 0);
	printf("Version\n");
	char tmp[1024];
	res = base1->GetVersion(tmp);
	printf("%s\n", tmp);
	
	_channel = base1->CreateChannel();
	ASSERT_LE(0, _channel);

	bool enable_aec = true;
	bool enable_agc = true;
	bool enable_ns = true;

 	res = apm->SetAgcStatus(enable_agc, VoIP::kAgcFixedDigital);
 	EXPECT_EQ(0, res);
 	res = apm->SetNsStatus(enable_ns, VoIP::kNsModerateSuppression);
	EXPECT_EQ(0, res);
	res = apm->SetEcStatus(enable_aec);
	EXPECT_EQ(0, res);

	io->RegisterExternalTransport(_channel, rec_receiver);
	
}
/*
TEST(AudioEngineTest, NetworkConfigTest)
{
	voice_channel_transport = new VoiceChannelTransport(io, _channel);
	char ip[64];
	printf("1. 127.0.0.1 \n");
	printf("2. Specify IP \n");
	int ip_selection;
	ASSERT_EQ(1, scanf("%i", &ip_selection));

	if (ip_selection == 1) {
		strcpy(ip, "127.0.0.1");
	} else {
		printf("Specify remote IP: ");
		ASSERT_EQ(1, scanf("%s", ip));
	}

	int rPort;
	printf("Specify remote port (1=1234): ");
	ASSERT_EQ(1, scanf("%i", &rPort));
	if (1 == rPort)
		rPort = 1234;
	printf("Set Send port \n");

	printf("Set Send IP \n");
	res = voice_channel_transport->SetSendDestination(ip, rPort);
	VALIDATE;

	int lPort;
	printf("Specify local port (1=1234): ");
	ASSERT_EQ(1, scanf("%i", &lPort));
	if (1 == lPort)
		lPort = 1234;
	printf("Set Rec Port \n");

	res = voice_channel_transport->SetLocalReceiver(lPort);
	VALIDATE;

}
*/
TEST(AudioEngineTest, BeforeStreamingTest){

	int rd(-1), pd(-1);
	res = hardware->GetNumOfRecordingDevices(rd);
	VALIDATE;
	res = hardware->GetNumOfPlayoutDevices(pd);
	VALIDATE;

	char dn[128] = { 0 };
	char guid[128] = { 0 };
	printf("\nPlayout devices (%d): \n", pd);
	for (int j = 0; j < pd; ++j) {
		res = hardware->GetPlayoutDeviceName(j, dn, guid);
		VALIDATE;
		printf("  %d: %s \n", j, dn);

		printf("Recording devices (%d): \n", rd);
		for (int j = 0; j < rd; ++j) {
			res = hardware->GetRecordingDeviceName(j, dn, guid);
			VALIDATE;
			printf("  %d: %3s \n", j, dn);
		}
// 
// 		res = hardware->SetPlayoutDevice(pd);
// 		VALIDATE;
// 
// 		res = hardware->SetRecordingDevice(rd);
// 		VALIDATE;
	}

	bool playout_available = false;
	hardware->GetPlayoutDeviceStatus(playout_available);
	ASSERT_TRUE(playout_available) <<
		"Ensures that the method works and that hardware is in the right state.";;

	bool recording_available = false;
	EXPECT_EQ(0, hardware->GetRecordingDeviceStatus(recording_available));
	EXPECT_TRUE(recording_available) <<
		"Ensures that the method works and that hardware is in the right state.";
 }
TEST(AudioEngineTest, SetCodecTest){

// 	CodecInst codec_inst ; 
// 	memcpy(codec_inst.plname,  "opus", sizeof("opus"));
// 	codec_inst.pltype = 120;
// 	codec_inst.plfreq = 48000;
// 	codec_inst.packsize = 960;
// 	codec_inst.channels = 2;
// 	codec_inst.rate = 64000;


/*	{107, "L16", 8000, 80, 1, 128000},
	{108, "L16", 16000, 160, 1, 256000},
	{109, "L16", 32000, 320, 1, 512000},
	{111, "L16", 8000, 80, 2, 128000},
	{112, "L16", 16000, 160, 2, 256000},
	{113, "L16", 32000, 320, 2, 512000},
	{0, "PCMU", 8000, 160, 1, 64000},
	{8, "PCMA", 8000, 160, 1, 64000},
	{110, "PCMU", 8000, 160, 2, 64000},
	{118, "PCMA", 8000, 160, 2, 64000},
	{120, "opus", 48000, 960, 2, 64000},*/

// 	CodecInst codec_inst ; 
// 	memcpy(codec_inst.plname,  "L16", sizeof("L16"));
// 	codec_inst.pltype = 107 ;
// 	codec_inst.plfreq = 8000;
// 	codec_inst.packsize = 80;
// 	codec_inst.channels = 1;
// 	codec_inst.rate = 128000;
//	EXPECT_EQ(0, codec->SetSendCodec(_channel, codec_inst));
	

}

#include "audio_device.h"


TEST(AudioEngineTest, SetDeviceTest){

	EXPECT_EQ(0, hardware->SetRecordingDevice(VoIP::AudioDeviceModule::kDefaultCommunicationDevice));
	EXPECT_EQ(0, hardware->SetPlayoutDevice(VoIP::AudioDeviceModule::kDefaultCommunicationDevice));

	// 	pr = FileRecorder::CreateFileRecorder(_channel, VoIP::kFileFormatWavFile);
// 
// 	CodecInst inst ;
// 	pr->StartRecordingAudioFile("c:\\work\\filerecord.wav", inst, 0);
}

#include <windows.h>
clock_t t1, t2;

TEST(AudioEngineTest, StartPlayout){
// 	
// 	EXPECT_EQ(0, base1->StartSend(_channel));
// 	EXPECT_EQ(0, base1->StartReceive(_channel));
// 	EXPECT_EQ(0, base1->StartPlayout(_channel));
// 
// 	
	
	EXPECT_EQ(0, base1->Start(_channel));

	t1 = clock();
	//here should block the main thread ,
	
// 	for (int i  = 0 ; i < 15; i ++)
// 	{
// 	
// 	}
	system("PAUSE");

}

TEST(AudioEngineTest, Stop){
	


// 	res = base1->StopPlayout(_channel);
// 	VALIDATE;
// 
// 	res = base1->StopSend(_channel);
// 	VALIDATE;
// 
// 	res = base1->StopReceive(_channel);
// 	VALIDATE;
	res = base1->Stop(_channel);
	VALIDATE;


	/////summary/////
	t2 = clock();
	double t = (double)(t2 - t1)/CLOCKS_PER_SEC;
// 	std::cout<<"------------------------------------"<<"\r\n";
// 	std::cout<<"number of frame = "<<g_num_of_frame<<"\r\n";
// 	std::cout<<"total length = "<<pkg_length<<"\r\n";
// 	std::cout<<"average every pkg length = "<<(pkg_length/g_num_of_frame)<<"\r\n";
// 	std::cout<<"average every pkg bits = "<<(pkg_length* sizeof(pkg_length)/g_num_of_frame)<<"\r\n";
// 	std::cout<<"time spent = "<<t<<"\r\n";
// 	std::cout<<g_num_of_frame/t<<" packages per second"<<"\r\n";
// 
// 	std::cout<<"------------------------------------"<<"\r\n";

	std::cout<<"------------------------------------"<<"\r\n";
	system("PAUSE");
	//res = io->DeRegisterExternalTransport(_channel);
	VALIDATE;

//	base1->DeRegisterAudioEngineObserver();
	
	res = base1->Terminate();
	VALIDATE;



}
TEST(AudioEngineTest, DeInit){
	
	
	if (base1)
		base1->Release();

	if (apm)
		apm->Release();

	if (hardware)
		hardware->Release();

	if (io)
		io->Release();
	

	AudioEngine::Delete(m_aoe);
}


