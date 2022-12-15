#ifndef _CONVAI_SDK_AUDIO_H_
#define _CONVAI_SDK_AUDIO_H_

#include "portaudio.h"
#include <cstdint>

namespace convai_sdk {

static constexpr int NUM_CHANNEL = 1;
static constexpr PaSampleFormat FORMAT = paInt16;
static constexpr int SAMPLING_RATE = 44100;
static constexpr int RECORD_SECONDS = 2;
static constexpr int FRAMES_PER_BUFFER = 1024;

// This is a wrapper over the portaudio library's blocking io API.
class Audio {
public:
  Audio() = default;

  // Initializes portaudio stream.
  void Start();

  // Returns true if the audio stream has been started, false otherwise.
  bool Started() const { return started_; }

  // Reads FRAMES_PER_BUFFER worth of frame data from the default microphone
  // device into `buffer`. The size of `buffer`  must be equal to
  // FRAMES_PER_BUFFER.
  void ReadBuffer(int16_t *buffer);

  // Writes `length` frames of paInt16 audio data to the default speaker device.
  // The size of `buffer` must be atleast `length`.
  void WriteBuffer(const int16_t *buffer, int length);

  // Closes portaudio streams and terminates them. It is important to call this
  // method before discarding the Audio object.
  void Close();

  /// Non-copyable and non-movable.
  Audio(const Audio &other) = delete;
  Audio &operator=(const Audio &other) = delete;
  Audio(Audio &&other) noexcept = delete;
  Audio &operator=(Audio &&other) noexcept = delete;

  ~Audio() = default;

private:
  PaStream *stream_;
  PaStreamParameters input_parameters_;
  PaStreamParameters output_parameters_;
  bool started_ = false;
};

}   // namespace convai_sdk

#endif   // _CONVAI_SDK_AUDIO_H_