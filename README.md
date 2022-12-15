## Convai SDK (C++)
This repository provides a set of libraries to access the Convai Character GetResponse API via gRPC streaming.

#### Request and Response
The request and response use the protocol buffers format are defined in the file `convai.proto`.

### Libraries
1. **Interaction**: Provides one round of conversation between the user and the Convai Character API. Both the request and the response can be delivered in small chunks for better latency. Each response returns a `session_id`. Subsequent Interactions may use the same `session_id` to simulate a conversation.
2. **Audio**: Provides a utility to read audio and play audio from default microphone and speaker devices. Currently we only support the following audio format:
  - Format: Linear PCM
  - Sampling rate: 16kHz or 44.1kHz
  - Number of channels: 1 (Mono)
  - Bitrate: 16bits

### Build and Run
The sdk uses Bazel to manage build targets and dependencies.
1. Install Bazel at https://bazel.build/install.
2. Verify the install using:
```sh
bazel --version
```
3. Build the example binary using the following command.
```sh
bazel build --cxxopt='-std=c++14' :main
```
4. Run the build using the following command.
```sh
./bazel-bin/main
```
5. To clean the build files.
```sh
bazel clean
```
  
### Example usage
1. Define the GetResponseConfig in a txt file: `example_config.txt`.
```
character_id: "<character_id>"
api_key: "<api_key>"
audio_config {
    sample_rate_hertz: 44100
}
```
2. Parse the file into a proto message.
```cpp
#include "interaction.h"

service::GetResponseRequest::GetResponseConfig get_response_config =
      GetResponseConfigFromFile("example_config.txt");
```
3. Create two audio streams for input and output.
```cpp
Audio audio_in;
Audio audio_out;
```
4. Create a new Interaction and initialize it.
```cpp
Interaction interaction(get_response_config);
interaction.Start([&audio_out](service::GetResponseResponse resp) {
  // This callback is triggered for every chunk of response.
  // Each chunk may have some response audio data or a
  // response action.
  if (resp.has_action_response()) {
    std::cout << "Action: " << resp.action_response.DebugString();
  }
  if (resp.has_audio_response()) {
    std::cout << "Audio text: " << resp.audio_data().text_data();
    // Play the audio using `audio_out`
    if (!audio_out.Started()) {
      audio_out.Start()
    }
    
    // Skip wav header size of 44 bytes.
    const std::string audio_d =
                  resp.audio_response().audio_data().substr(44);
                  
    // Cast the audio byte data into paInt16 format.
    const int16_t *audio_int = (const int16_t *) audio_d.data();
    int64_t num_frames =
        (audio_d.length() * sizeof(char)) / sizeof(int16_t);
    audio_out.WriteBuffer(audio_int, num_frames);   
  }
});
```
5. Record user input using `audio_in` and add to Interaction.
```cpp
audio_in.Start();
// Record audio for `RECORD_SECONDS`
for (int i = 0; i < (SAMPLING_RATE / FRAMES_PER_BUFFER * RECORD_SECONDS);
     ++i) {
  int16_t buffer[FRAMES_PER_BUFFER];
  audio_in.ReadBuffer(buffer);
  char *audio_data = (char *) buffer;
  int length = FRAMES_PER_BUFFER * sizeof(int16_t) / sizeof(char);
  interaction.SendAudio(audio_data, length);
}
// Stop recording
audio_in.Close();
// Finish the interaction and wait for all response callbacks to finish.
auto status = interaction.Stop();
// Close audio_out stream once interaction has finished.
if (audio_out.Started()) {
  audio_out.Close();
}
if (!status.ok()) {
    exit(1);
}
```
6. A version of this example is present in `main.cc`.