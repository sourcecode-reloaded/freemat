#include "Array.hpp"
#if HAVE_PORTAUDIO19 || HAVE_PORTAUDIO18
#include "portaudio.h"
#endif

const int FRAMES_PER_BUFFER = 64;

class PlayBack {
public:
  char *data;
  int length;
  int channels;
  int framesToGo;
  int ptr;
  int elementSize;
  Array x;
};

#if HAVE_PORTAUDIO19 || HAVE_PORTAUDIO18

#if HAVE_PORTAUDIO19
static int DoPlayBackCallback(const void *, void *output,
			      unsigned long framesPerBuffer,
			      const PaStreamCallbackTimeInfo*,
			      PaStreamCallbackFlags,
			      void *userData) {
#endif

#if HAVE_PORTAUDIO18
static int DoPlayBackCallback(void *, void *output,
			      unsigned long framesPerBuffer,
			      PaTimestamp,
			      void *userData) {
#endif

  PlayBack *pb = (PlayBack*) userData;
  char *out = (char*) output;
  const char *dp = pb->data;
  int framesToCalc;
  int finished = 0;
  int i;
  if( (unsigned long) pb->framesToGo < framesPerBuffer )
    {
      framesToCalc = pb->framesToGo;
      pb->framesToGo = 0;
      finished = 1;
    }
  else
    {
      framesToCalc = framesPerBuffer;
      pb->framesToGo -= framesPerBuffer;
    }
  
  for( i=0; i<framesToCalc; i++ )
    {
      if (pb->channels == 1) {
	for (int j=0;j<pb->elementSize;j++)
	  *out++ = dp[pb->ptr*pb->elementSize + j];
	pb->ptr++;
      } else {
	for (int j=0;j<pb->elementSize;j++) 
	  *out++ = dp[pb->ptr*pb->elementSize + j];
	for (int j=0;j<pb->elementSize;j++)
	  *out++ = dp[(pb->ptr+pb->length)*pb->elementSize + j];
	pb->ptr++;
      }
    }
  /* zero remainder of final buffer */
  for( ; i<(int)framesPerBuffer; i++ )
    {
      if (pb->channels == 1)
	for (int j=0;j<pb->elementSize;j++)
	  *out++ = 0;
      else {
	for (int j=0;j<pb->elementSize;j++) {
	  *out++ = 0; /* left */
	  *out++ = 0; /* right */
	}
      }
    }
  return finished;
}

#if HAVE_PORTAUDIO19
static int DoRecordCallback(const void *input, void *,
			    unsigned long framesPerBuffer,
			    const PaStreamCallbackTimeInfo*,
			    PaStreamCallbackFlags,
			    void *userData) {
#endif

#if HAVE_PORTAUDIO18
static int DoRecordCallback(void *input, void *,
			    unsigned long framesPerBuffer,
			    PaTimestamp,
			    void *userData) {
#endif

  PlayBack *pb = (PlayBack*) userData;
  char *in = (char*) input;
  char *dp = pb->data;
  int framesToCalc;
  int finished = 0;
  int i;
  if( (unsigned long) pb->framesToGo < framesPerBuffer )
    {
      framesToCalc = pb->framesToGo;
      pb->framesToGo = 0;
      finished = 1;
    }
  else
    {
      framesToCalc = framesPerBuffer;
      pb->framesToGo -= framesPerBuffer;
    }
  
  for( i=0; i<framesToCalc; i++ )
    {
      if (pb->channels == 1) {
	for (int j=0;j<pb->elementSize;j++)
	  dp[pb->ptr*pb->elementSize + j] = *in++;
	pb->ptr++;
      } else {
	for (int j=0;j<pb->elementSize;j++) 
	  dp[pb->ptr*pb->elementSize + j] = *in++;
	for (int j=0;j<pb->elementSize;j++)
	  dp[(pb->ptr+pb->length)*pb->elementSize + j] = *in++;
	pb->ptr++;
      }
    }
  return finished;
}

static PaError err;
static bool RunningStream = false;
static PaStream *stream;
static PlayBack *pb_obj;

void PAInit() {
  err = Pa_Initialize();
  if (err != paNoError) 
    throw Exception(QString("An error occured while using the portaudio stream: ") + Pa_GetErrorText(err));
}

void PAShutdown() {
#ifdef HAVE_PORTAUDIO19
  while ( ( err = Pa_IsStreamActive( stream ) ) == 1 )
    Pa_Sleep(100);
#endif

#ifdef HAVE_PORTAUDIO18
  while ( ( err = Pa_StreamActive( stream ) ) == 1 )
    Pa_Sleep(100);
#endif

  err = Pa_CloseStream(stream);
  if (err != paNoError) 
    throw Exception(QString("An error occured while using the portaudio stream: ") + Pa_GetErrorText(err));
  Pa_Terminate();
  delete pb_obj;
  RunningStream = false;
}

void DoPlayBack(const void *data, int count, int channels, 
		int elementSize, unsigned long SampleFormat,
		int Rate, bool asyncMode, Array x) {
  if (RunningStream) 
    PAShutdown();
  PAInit();
  pb_obj = new PlayBack;
  pb_obj->x = x;
  pb_obj->data = (char *)data;
  pb_obj->length = count;
  pb_obj->channels = channels;
  pb_obj->ptr = 0;
  pb_obj->framesToGo = count;
  pb_obj->elementSize = elementSize;
#ifdef HAVE_PORTAUDIO19
  PaStreamParameters outputParameters;
  outputParameters.device = Pa_GetDefaultOutputDevice();
  outputParameters.channelCount = channels;
  outputParameters.sampleFormat = SampleFormat;
  outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;
  err = Pa_OpenStream(&stream,
		      NULL, /* no input */
		      &outputParameters,
		      Rate,
		      FRAMES_PER_BUFFER,
		      paNoFlag,
		      DoPlayBackCallback,
		      pb_obj);
#endif
#ifdef HAVE_PORTAUDIO18
  err = Pa_OpenStream(&stream,
		      paNoDevice, // No input device
		      0,          // No input
		      SampleFormat,
		      NULL,
		      Pa_GetDefaultOutputDeviceID(),
		      channels,
		      SampleFormat,
		      NULL,
		      Rate,
		      FRAMES_PER_BUFFER,
		      0,
		      paClipOff,
		      DoPlayBackCallback,
		      pb_obj);
#endif
  if (err != paNoError) 
    throw Exception(QString("An error occured while using the portaudio stream: ") + Pa_GetErrorText(err));
  err = Pa_StartStream(stream);
  if (err != paNoError) 
    throw Exception(QString("An error occured while using the portaudio stream: ") + Pa_GetErrorText(err));
  if (!asyncMode)
    PAShutdown();
  else
    RunningStream = true;
}

void DoRecord(void *data, int count, int channels, 
	      int elementSize, unsigned long SampleFormat,
	      int Rate) {
  if (RunningStream) 
    PAShutdown();
  PAInit();
  pb_obj = new PlayBack;
  pb_obj->data = (char *)data;
  pb_obj->length = count;
  pb_obj->channels = channels;
  pb_obj->ptr = 0;
  pb_obj->framesToGo = count;
  pb_obj->elementSize = elementSize;
#if HAVE_PORTAUDIO19
  PaStreamParameters inputParameters;
  inputParameters.device = Pa_GetDefaultInputDevice();
  inputParameters.channelCount = channels;
  inputParameters.sampleFormat = SampleFormat;
  inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
  inputParameters.hostApiSpecificStreamInfo = NULL;
  err = Pa_OpenStream(&stream,
		      &inputParameters,
		      NULL,
		      Rate,
		      FRAMES_PER_BUFFER,
		      paNoFlag,
		      DoRecordCallback,
		      pb_obj);
#endif
#if HAVE_PORTAUDIO18
  err = Pa_OpenStream(&stream,
		      Pa_GetDefaultInputDeviceID(),
		      channels,
		      SampleFormat,
		      NULL,
		      paNoDevice,
		      0,
		      SampleFormat,
		      NULL,
		      Rate,
		      FRAMES_PER_BUFFER,
		      0,
		      0,
		      DoRecordCallback,
		      pb_obj);
#endif
  if (err != paNoError) 
    throw Exception(QString("An error occured while using the portaudio stream: ") + Pa_GetErrorText(err));
  err = Pa_StartStream(stream);
  if (err != paNoError) 
    throw Exception(QString("An error occured while using the portaudio stream: ") + Pa_GetErrorText(err));
  PAShutdown();
}
#endif


//!
//@Module WAVPLAY
//@@Section IO
//@@Usage
//Plays a linear PCM set of samples through the audio system.  This
//function is only available if the @|portaudio| library was available
//when FreeMat was built.  The syntax for the command is one of:
//@[
//   wavplay(y)
//   wavplay(y,sampling_rate)
//   wavplay(...,mode)
//@]
//where @|y| is a matrix of audio samples.  If @|y| has two columns, then
//the audio playback is in stereo.  The @|y| input can be of types 
//@|float, double, int32, int16, int8, uint8|.  For @|float| and 
//@|double| types, the sample values in @|y| must be between @|-1| and
//@|1|.  The @|sampling_rate| specifies the rate at which the data is 
//recorded.  If not specified, the @|sampling_rate| defaults to @|11025Hz|.
//Finally, you can specify a playback mode of @|'sync'| which is synchronous
//playback or a playback mode of @|'async'| which is asynchronous playback.
//For @|'sync'| playback, the wavplay function returns when the playback is
//complete.  For @|'async'| playback, the function returns immediately (unless
//a former playback is still issuing).
//@@Signature
//function wavplay
//inputs y sampling_rate mode
//outputs none
//!
ArrayVector WavPlayFunction(int nargout, const ArrayVector& argv) {
#if HAVE_PORTAUDIO18 || HAVE_PORTAUDIO19
  if(argv.size() == 0)
    throw Exception("wavplay requires at least one argument (the audio data to playback)");
  Array y(argv[0]);
  int SampleRate = 11025;
  QString mode = "SYNC";
  if (argv.size() > 1)
    SampleRate = argv[1].asInteger();
  if (argv.size() > 2)
    mode = argv[2].asString().toUpper();
  // Validate that the data is reasonable
  if ((!y.is2D())  || (!y.isVector() && (y.columns() > 2)) || 
      y.isReferenceType() || y.isComplex())
    throw Exception("wavplay only supports playback of 1 or 2 channel signals, which means a 1 or 2 column matrix");
  if (y.dataClass() == Double)
    y = y.toClass(Float);
  if ((y.dataClass() == Int64) || (y.dataClass() == UInt64))
    throw Exception("wavplay does not support 64 bit data types.");
  if (y.dataClass() == UInt32)
    throw Exception("wavplay does not support unsigned 32-bit data types.");
  if (y.dataClass() == UInt16)
    throw Exception("wavplay does not support unsigned 16-bit data types.");
  int samples;
  int channels;
  if (!y.isVector()) {
    channels = int(y.columns());
    samples = int(y.rows());
  } else {
    channels = 1;
    samples = int(y.length());
  }
  if (y.dataClass() == Float)
    DoPlayBack(y.constReal<float>().constData(),samples,channels,sizeof(float),
	       paFloat32,SampleRate,mode != "SYNC",y);
  else if (y.dataClass() == Int32)
    DoPlayBack(y.constReal<int32>().constData(),samples,channels,sizeof(int32),
	       paInt32,SampleRate,mode != "SYNC",y);
  else if (y.dataClass() == Int16)
    DoPlayBack(y.constReal<int16>().constData(),samples,channels,sizeof(int16),
	       paInt16,SampleRate,mode != "SYNC",y);
  else if (y.dataClass() == Int8)
    DoPlayBack(y.constReal<int8>().constData(),samples,channels,sizeof(int8),
	       paInt8,SampleRate,mode != "SYNC",y);
  else if (y.dataClass() == UInt8)
    DoPlayBack(y.constReal<uint8>().constData(),samples,channels,sizeof(uint8),
	       paUInt8,SampleRate,mode != "SYNC",y);
  else
    throw Exception("wavplay does not support this data types.");
#else
  throw Exception("Audio read/write support not available.  Please build the PortAudio library and rebuild FreeMat to enable this functionality.");
#endif
  return ArrayVector();
}

//!
//@Module WAVRECORD
//@@Section IO
//@@Usage
//Records linear PCM sound from the audio system.  This function is
//only available if the @|portaudio| library was available when FreeMat
//was built.  The syntax for this command is one of:
//@[
//  y = wavrecord(samples,rate)
//  y = wavrecord(...,channels)
//  y = wavrecord(...,'datatype')
//@]
//where @|samples| is the number of samples to record, and @|rate| is the
//sampling rate.  If not specified, the @|rate| defaults to @|11025Hz|.
//If you want to record in stero, specify @|channels = 2|.  Finally, you
//can specify the type of the recorded data (defaults to @|FM_DOUBLE|).
//Valid choices are @|float, double, int32, int16, int8, uint8|.
//@@Signature
//function wavrecord
//inputs varargin
//outputs y
//!

template <class T>
static Array WavRecord(int samples, int channels, unsigned long SampleFormat, int Rate) {
  BasicArray<T> dp(NTuple(samples,channels));
  DoRecord(dp.data(),samples,channels,sizeof(T),SampleFormat,Rate);
  return Array(dp);
}

ArrayVector WavRecordFunction(int nargout, const ArrayVector& argv) {
#if HAVE_PORTAUDIO18 || HAVE_PORTAUDIO19
  if (argv.size() < 1)
    throw Exception("wavrecord requires at least 1 argument (the number of samples to record)");
  int samples = argv[0].asInteger();
  int rate = 11025;
  int channels = 1;
  DataClass datatype = Double;
  ArrayVector argvCopy(argv);
  if ((argvCopy.size() > 1) && (argvCopy.back().isString())) {
    QString typestring = argvCopy.back().asString().toUpper();
    if ((typestring == "FLOAT") || (typestring == "SINGLE"))
      datatype = Float;
    else if (typestring == "DOUBLE")
      datatype = Double;
    else if (typestring == "INT32")
      datatype = Int32;
    else if (typestring == "INT16")
      datatype = Int16;
    else if (typestring == "INT8")
      datatype = Int8;
    else if (typestring == "UINT8")
      datatype = UInt8;
    else
      throw Exception("unrecognized data type - expecting one of: double, float, single, int32, int16, int8, uint8");
    argvCopy.pop_back();
  }
  // Check for a channel spec and a sampling rate
  while (argvCopy.size() > 1) {
    int ival = argvCopy.back().asInteger();
    if (ival > 2)
      rate = ival;
    else
      channels = ival;
    argvCopy.pop_back();
  }
  DataClass rdatatype(datatype);
  Array retvec;
  if (rdatatype == Double) rdatatype = Float;
  switch(rdatatype) {
  default: throw Exception("Illegal data type argument for wavrecord");
  case Float: 
    return ArrayVector(WavRecord<float>(samples,channels,paFloat32,rate).toClass(datatype));
  case Int32:
    return ArrayVector(WavRecord<int32>(samples,channels,paInt32,rate).toClass(datatype));
  case Int16:
    return ArrayVector(WavRecord<int16>(samples,channels,paInt16,rate).toClass(datatype));
  case Int8:
    return ArrayVector(WavRecord<int8>(samples,channels,paInt8,rate).toClass(datatype));
  case UInt8:
    return ArrayVector(WavRecord<uint8>(samples,channels,paUInt8,rate).toClass(datatype));
  }
#else
  throw Exception("Audio read/write support not available.  Please build the PortAudio library and rebuild FreeMat to enable this functionality.");
#endif
  return ArrayVector();
}