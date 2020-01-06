
#include "stdafx.h"
// #include "gtest/gtest.h"
// #include "Audio_Device_FuncTest.h"
// #include "device_test/audio_device_test.h"

#include "record_test.h"
// 
// using namespace testing;
// 
 #define TEST_LOG printf
 #define TEST_LOG_ERROR(...) fprintf(stderr, __VA_ARGS__)
 static int warningCount = 0;

// int func_test(int);
// int device_test();

int _tmain(int argc, _TCHAR* argv[])
{
	testing::InitGoogleTest(&argc, argv);

	int testindex = 0;
// 	TEST_LOG("(0) : engine test\n");
// 	TEST_LOG("(1) : audio device test suite\n");
// 	TEST_LOG("(2) : audio device test \n");

// 	if (scanf("%d", &testindex) < 0) {
// 		perror("Failed to get selection.");
// 	}
	switch (testindex)
	{
	case 0:
		main_test();
		//return RUN_ALL_TESTS();
	case 1:

//		func_test(0);
	case 2:
//		device_test();
	default:
		break;
	}
	
	return 0;
}
// 
// 
// int func_test(int sel)
// {
// 	TEST_LOG("=========================================\n");
// 	TEST_LOG("Func Test of the AudioDevice Module\n");
// 	TEST_LOG("=========================================\n\n");
// 
// 	// Initialize the counters here to get rid of "unused variables" warnings.
// 	warningCount = 0;
// 
// 	Audio_Device_FuncTest funcMgr;
// 
// 	funcMgr.Init();
// 
// 	bool quit(false);
// 
// 	while (!quit)
// 	{
// 		TEST_LOG("---------------------------------------\n");
// 		TEST_LOG("Select type of test\n\n");
// 		TEST_LOG("  (0) Quit\n");
// 		TEST_LOG("  (1) All\n");
// 		TEST_LOG("- - - - - - - - - - - - - - - - - - - -\n");
// 		TEST_LOG("  (2) Audio-layer selection\n");
// 		TEST_LOG("  (3) Device enumeration\n");
// 		TEST_LOG("  (4) Device selection\n");
// 		TEST_LOG("  (5) Audio transport\n");
// 		TEST_LOG("  (6) Speaker volume\n");
// 		TEST_LOG("  (7) Microphone volume\n");
// 		TEST_LOG("  (8) Speaker mute\n");
// 		TEST_LOG("  (9) Microphone mute\n");
// 		TEST_LOG(" (10) Microphone boost\n");
// 		TEST_LOG(" (11) Microphone AGC\n");
// 		TEST_LOG(" (12) Loopback measurements\n");
// 		TEST_LOG(" (13) Device removal\n");
// 		//		TEST_LOG(" (14) Advanced mobile device API\n");
// 		//		TEST_LOG(" (66) XTEST\n");
// 		TEST_LOG("- - - - - - - - - - - - - - - - - - - -\n");
// 		TEST_LOG("\n: ");
// 
// 		int selection(0);
// 		enum TestType testType(TTInvalid);
// 
// SHOW_MENU:
// 
// 		if (sel > 0)
// 		{
// 			selection = sel;
// 		}
// 		else
// 		{
// 			if (scanf("%d", &selection) < 0) {
// 				perror("Failed to get selection.");
// 			}
// 		}
// 
// 		switch (selection)
// 		{
// 		case 0:
// 			quit = true;
// 			break;
// 		case 1:
// 			testType = TTAll;
// 			break;
// 		case 2:
// 			testType = TTAudioLayerSelection;
// 			break;
// 		case 3:
// 			testType = TTDeviceEnumeration;
// 			break;
// 		case 4:
// 			testType = TTDeviceSelection;
// 			break;
// 		case 5:
// 			testType = TTAudioTransport;
// 			break;
// 		case 6:
// 			testType = TTSpeakerVolume;
// 			break;
// 		case 7:
// 			testType = TTMicrophoneVolume;
// 			break;
// 		case 8:
// 			testType = TTSpeakerMute;
// 			break;
// 		case 9:
// 			testType = TTMicrophoneMute;
// 			break;
// 		case 10:
// 			testType = TTMicrophoneBoost;
// 			break;
// 		case 11:
// 			testType = TTMicrophoneAGC;
// 			break;
// 		case 12:
// 			testType = TTLoopback;
// 			break;
// 		case 13:
// 			testType = TTDeviceRemoval;
// 			break;
// 			// 		case 14:
// 			// 			testType = TTMobileAPI;
// 			// 			break;
// 			// 		case 66:
// 			// 			testType = TTTest;
// 			// 			break;
// 		default:
// 			testType = TTInvalid;
// 			TEST_LOG(": ");
// 			goto SHOW_MENU;
// 			break;
// 		}
// 
// 		funcMgr.DoTest(testType);
// 
// 		if (sel > 0)
// 		{
// 			quit = true;
// 		}
// 	}
// 
// 	funcMgr.Close();
// 
// 	return 0;
// }
// 
// int device_test()
// {
// 	int ret = 0;
// 	audio_device_test test;
// 	ret = test.create_device();
// 	ret &= test.init();
// 	ret &= test.set_playout();
// 	ret &= test.set_record();
// 	ret &= test.init_playout();
// 	ret &= test.init_record();
// 	
// 	ret &= test.start();
// 	system("PAUSE");
//   	ret &= test.stop();
// 	return ret;
// }
