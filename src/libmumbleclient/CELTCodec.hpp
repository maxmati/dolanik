#ifndef _LIBMUMBLECLIENT_CELTCODEC_H_
#define _LIBMUMBLECLIENT_CELTCODEC_H_

#include <string>
//iostream wyjebac bo debug dla destruktora bo robie liste kodekuw kurwa
#include <iostream>

#include <celt/celt.h>

#include "Library.hpp"

class CELTCodec {
protected:
	Library celtLibrary;
	const CELTMode *mode;
	std::string version;
	mutable int bitstreamVersion;
	bool valid;
	
	void (*celt_mode_destroy)(CELTMode *mode);
	int (*celt_mode_info)(const CELTMode *mode, int request, celt_int32 *value);
public:
	CELTCodec(const std::string &version);
	virtual ~CELTCodec();
	
	bool isValid() const;
	int getBitstreamVersion() const;
	
	void (*celt_encoder_destroy)(CELTEncoder *encoder);
	int (*celt_encoder_ctl)(CELTEncoder *encoder, int request, ...);
	
	virtual CELTEncoder *encoderCreate() = 0;
	virtual int encode(CELTEncoder *st, const celt_int16 *pcm, unsigned char *compressed, int nbCompressedBytes) = 0;
};

class CELTCodec070 : public CELTCodec {
protected:
	CELTMode *(*celt_mode_create)(celt_int32 fs, int frame_size, int *error);
	CELTEncoder *(*celt_encoder_create)(const CELTMode *mode, int channels, int *error);
	int (*celt_encode)(CELTEncoder *st, const celt_int16 *pcm, celt_int16 *optional_synthesis, unsigned char *compressed, int nbCompressedBytes);
	const char *(*celt_strerror)(int error);

public:
	CELTCodec070(const std::string &version);
	
	virtual CELTEncoder *encoderCreate();
	virtual int encode(CELTEncoder *st, const celt_int16 *pcm, unsigned char *compressed, int nbCompressedBytes);
};

class CELTCodec011 : public CELTCodec {
protected:
	CELTMode *(*celt_mode_create)(celt_int32 fs, int frame_size, int *error);
	CELTEncoder *(*celt_encoder_create_custom)(const CELTMode *mode, int channels, int *error);
	int (*celt_encode)(CELTEncoder *st, const celt_int16 *pcm, int frame_size, unsigned char *compressed, int nbCompressedBytes);
	const char *(*celt_strerror)(int error);

public:
	CELTCodec011(const std::string &version);
	
	virtual CELTEncoder *encoderCreate();
	virtual int encode(CELTEncoder *st, const celt_int16 *pcm, unsigned char *compressed, int nbCompressedBytes);
};

#endif
