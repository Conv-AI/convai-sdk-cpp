#include "audio.h"
#include "interaction.h"
#include "proto_utils.h"
#include <cmath>
#include <string>

using convai_sdk::Audio;
using convai_sdk::FRAMES_PER_BUFFER;
using convai_sdk::Interaction;
using convai_sdk::RECORD_SECONDS;
using convai_sdk::SAMPLING_RATE;
using convai_sdk::proto_util::GetResponseConfigFromFile;
using service::GetResponseRequest;
using service::GetResponseResponse;

static constexpr int WAV_HEADER_SIZE = 44;
static constexpr char GET_RESPONSE_CONFIG_FILE[] =
    "get_response_config_example.txt";

int main() {
  GetResponseRequest::GetResponseConfig get_response_config =
      GetResponseConfigFromFile(GET_RESPONSE_CONFIG_FILE);
  while (1) {
    char c;
    std::cout << "\n1. Press N to start new round of Conversation."
              << "\n2. Press Q to exit.\n";
    std::cin >> c;
    if (c == 'q' || c == 'Q') {
      break;
    }
    Audio audio_in;
    Audio audio_out;
    Interaction interaction(get_response_config);

    std::string transcript = "";
    std::string temp_transcript = "";
    interaction.Start(
        /*response_callback=*/[&audio_out, &get_response_config, &transcript,
                               &temp_transcript](GetResponseResponse resp) {
          if (resp.has_user_query()) {
            auto user_query = resp.user_query();
            if (user_query.is_final()) {
              transcript += user_query.text_data();
              temp_transcript = "";
            } else {
              temp_transcript = user_query.text_data();
            }
            std::cout << "You: " << transcript << temp_transcript;
            if (!user_query.end_of_response()) {
              std::cout << '\r';
              std::cout.flush();
            } else {
              std::cout << std::endl;
              std::cout << "Bot: ";
            }
          } else if (resp.has_action_response()) {
            std::cout << std::endl << "Action: ";
            std::cout << resp.action_response().action();
            std::cout << std::endl;
            std::cout << "Bot: ";
          } else if (resp.has_audio_response()) {
            // Set session id for all future interactions in this
            // conversation.
            if (get_response_config.session_id() == "") {
              get_response_config.set_session_id(resp.session_id());
            }
            std::cout << resp.audio_response().text_data() << std::endl;
            if (resp.audio_response().end_of_response()) {
              std::cout << std::endl;
            }

            if (resp.audio_response().audio_data() != "") {
              if (!audio_out.Started()) {
                audio_out.Start(
                    resp.audio_response().audio_config().sample_rate_hertz());
              }
              // Skip the wav header in the response. This is currently
              // hardcoded and may be changed in the future.
              const std::string audio_d =
                  resp.audio_response().audio_data().substr(WAV_HEADER_SIZE);

              // Cast the audio byte data into paInt16 format.
              const int16_t *audio_int = (const int16_t *) audio_d.data();
              int64_t num_frames =
                  (audio_d.length() * sizeof(char)) / sizeof(int16_t);
              audio_out.WriteBuffer(audio_int, num_frames);
            }
          }
        });

    std::cout << "Started Recording: " << std::endl;
    audio_in.Start();
    for (int i = 0; i < (SAMPLING_RATE / FRAMES_PER_BUFFER * RECORD_SECONDS);
         ++i) {
      int16_t buffer[FRAMES_PER_BUFFER];
      audio_in.ReadBuffer(buffer);
      char *audio_data = (char *) buffer;
      int length = FRAMES_PER_BUFFER * sizeof(int16_t) / sizeof(char);
      interaction.SendAudio(audio_data, length);
    }
    audio_in.Close();
    auto status = interaction.Stop();
    if (audio_out.Started()) {
      audio_out.Close();
    }
    if (!status.ok()) {
      exit(1);
    }
  }
  return 0;
}