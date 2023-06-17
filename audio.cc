#include "audio.h"
#include <iostream>

namespace convai_sdk {

void Audio::Start(int sampling_rate) {
  auto err = Pa_Initialize();
  if (err != paNoError) {
    std::cout << "Error initializing port audio. Exiting.";
    return;
  }

  input_parameters_.device = Pa_GetDefaultInputDevice();
  input_parameters_.channelCount = NUM_CHANNEL;
  input_parameters_.sampleFormat = FORMAT;
  input_parameters_.suggestedLatency =
      Pa_GetDeviceInfo(input_parameters_.device)->defaultLowInputLatency;
  input_parameters_.hostApiSpecificStreamInfo = NULL;

  output_parameters_.device = Pa_GetDefaultOutputDevice();
  output_parameters_.channelCount = NUM_CHANNEL;
  output_parameters_.sampleFormat = FORMAT;
  output_parameters_.suggestedLatency =
      Pa_GetDeviceInfo(output_parameters_.device)->defaultLowOutputLatency;
  // std::cout << "Output device name: " <<
  // Pa_GetDeviceInfo(output_parameters_.device)->name << std::endl;
  output_parameters_.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(&stream_, &input_parameters_, &output_parameters_,
                      sampling_rate, FRAMES_PER_BUFFER,
                      paClipOff, /* we won't output out of range samples so
                                    don't bother clipping them */
                      NULL,      /* no callback, use blocking API */
                      NULL);     /* no callback, so no callback userData */

  if (err != paNoError) {
    std::cout << "Error opening port audio stream. Exiting.";
    return;
  }

  err = Pa_StartStream(stream_);
  if (err != paNoError) {
    std::cout << "Error starting port audio stream. Exiting.";
    return;
  }
  started_ = true;
}

void Audio::ReadBuffer(int16_t *buffer) {
  auto err = Pa_ReadStream(stream_, buffer, FRAMES_PER_BUFFER);
  if (err != paNoError) {
    std::cout << "Error reading port audio stream. Exiting."
              << " Code: " << err << " Error message: " << Pa_GetErrorText(err)
              << std::endl;
    ;
    return;
  }
}
void Audio::WriteBuffer(const int16_t *buffer, int length) {
  auto err = Pa_WriteStream(stream_, buffer, length);
  if (err != paNoError) {
    std::cout << "Error writing port audio stream. Exiting."
              << " Code: " << err << " Error message: " << Pa_GetErrorText(err)
              << std::endl;
    return;
  }
}

void Audio::Close() {
  auto err = Pa_StopStream(stream_);
  if (err != paNoError) {
    std::cout << "Error stopping port audio stream. Exiting."
              << " Code: " << err << " Error message: " << Pa_GetErrorText(err)
              << std::endl;
    return;
  }
  err = Pa_CloseStream(stream_);
  if (err != paNoError) {
    std::cout << "Error closing port audio stream. Exiting.";
    return;
  }
  Pa_Terminate();
}
}   // namespace convai_sdk