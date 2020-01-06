#pragma once


#include "include/common.h"
#include "include/common_types.h"

namespace VoIP{
// Video codec
enum { kConfigParameterSize = 128};
enum { kPayloadNameSize = 32};
enum { kMaxSimulcastStreams = 4};
enum { kMaxTemporalStreams = 4};

enum VideoCodecComplexity
{
	kComplexityNormal = 0,
	kComplexityHigh    = 1,
	kComplexityHigher  = 2,
	kComplexityMax     = 3
};

enum VideoCodecProfile
{
	kProfileBase = 0x00,
	kProfileMain = 0x01
};

enum VP8ResilienceMode {
	kResilienceOff,    // The stream produced by the encoder requires a
	// recovery frame (typically a key frame) to be
	// decodable after a packet loss.
	kResilientStream,  // A stream produced by the encoder is resilient to
	// packet losses, but packets within a frame subsequent
	// to a loss can't be decoded.
	kResilientFrames   // Same as kResilientStream but with added resilience
	// within a frame.
};

// VP8 specific
struct VideoCodecVP8 {
	bool                 pictureLossIndicationOn;
	bool                 feedbackModeOn;
	VideoCodecComplexity complexity;
	VP8ResilienceMode    resilience;
	unsigned char        numberOfTemporalLayers;
	bool                 denoisingOn;
	bool                 errorConcealmentOn;
	bool                 automaticResizeOn;
	bool                 frameDroppingOn;
	int                  keyFrameInterval;

	bool operator==(const VideoCodecVP8& other) const {
		return pictureLossIndicationOn == other.pictureLossIndicationOn &&
			feedbackModeOn == other.feedbackModeOn &&
			complexity == other.complexity &&
			resilience == other.resilience &&
			numberOfTemporalLayers == other.numberOfTemporalLayers &&
			denoisingOn == other.denoisingOn &&
			errorConcealmentOn == other.errorConcealmentOn &&
			automaticResizeOn == other.automaticResizeOn &&
			frameDroppingOn == other.frameDroppingOn &&
			keyFrameInterval == other.keyFrameInterval;
	}

	bool operator!=(const VideoCodecVP8& other) const {
		return !(*this == other);
	}
};

// Video codec types
enum VideoCodecType
{
	kVideoCodecVP8,
	kVideoCodecI420,
	kVideoCodecRED,
	kVideoCodecULPFEC,
	kVideoCodecGeneric,
	kVideoCodecUnknown
};

union VideoCodecUnion
{
	VideoCodecVP8       VP8;
};


// Simulcast is when the same stream is encoded multiple times with different
// settings such as resolution.
struct SimulcastStream {
	unsigned short      width;
	unsigned short      height;
	unsigned char       numberOfTemporalLayers;
	unsigned int        maxBitrate;  // kilobits/sec.
	unsigned int        targetBitrate;  // kilobits/sec.
	unsigned int        minBitrate;  // kilobits/sec.
	unsigned int        qpMax; // minimum quality

	bool operator==(const SimulcastStream& other) const {
		return width == other.width &&
			height == other.height &&
			numberOfTemporalLayers == other.numberOfTemporalLayers &&
			maxBitrate == other.maxBitrate &&
			targetBitrate == other.targetBitrate &&
			minBitrate == other.minBitrate &&
			qpMax == other.qpMax;
	}

	bool operator!=(const SimulcastStream& other) const {
		return !(*this == other);
	}
};

enum VideoCodecMode {
	kRealtimeVideo,
	kScreensharing
};

// Common video codec properties
struct VideoCodec {
	VideoCodecType      codecType;
	char                plName[kPayloadNameSize];
	unsigned char       plType;

	unsigned short      width;
	unsigned short      height;

	unsigned int        startBitrate;  // kilobits/sec.
	unsigned int        maxBitrate;  // kilobits/sec.
	unsigned int        minBitrate;  // kilobits/sec.
	unsigned int        targetBitrate;  // kilobits/sec.

	unsigned char       maxFramerate;

	VideoCodecUnion     codecSpecific;

	unsigned int        qpMax;
	unsigned char       numberOfSimulcastStreams;
	SimulcastStream     simulcastStream[kMaxSimulcastStreams];

	VideoCodecMode      mode;

	// When using an external encoder/decoder this allows to pass
	// extra options without requiring VoIP to be aware of them.
	Config*  extra_options;

	bool operator==(const VideoCodec& other) const {
		bool ret = codecType == other.codecType &&
			(STR_CASE_CMP(plName, other.plName) == 0) &&
			plType == other.plType &&
			width == other.width &&
			height == other.height &&
			startBitrate == other.startBitrate &&
			maxBitrate == other.maxBitrate &&
			minBitrate == other.minBitrate &&
			targetBitrate == other.targetBitrate &&
			maxFramerate == other.maxFramerate &&
			qpMax == other.qpMax &&
			numberOfSimulcastStreams == other.numberOfSimulcastStreams &&
			mode == other.mode;
		if (ret && codecType == kVideoCodecVP8) {
			ret &= (codecSpecific.VP8 == other.codecSpecific.VP8);
		}

		for (unsigned char i = 0; i < other.numberOfSimulcastStreams && ret; ++i) {
			ret &= (simulcastStream[i] == other.simulcastStream[i]);
		}
		return ret;
	}

	bool operator!=(const VideoCodec& other) const {
		return !(*this == other);
	}
};

}