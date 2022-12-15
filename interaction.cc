#include "interaction.h"

namespace convai_sdk {

static constexpr char CONVAI_API_ENDPOINT[] = "34.168.121.146:9175";

void Interaction::Start(std::function<void(GetResponseResponse)> res_func) {
  rpc_channel_ = grpc::CreateChannel(CONVAI_API_ENDPOINT,
                                     grpc::InsecureChannelCredentials());
  stub_ = ConvaiService::NewStub(rpc_channel_);
  stream_ = std::shared_ptr<
      ClientReaderWriter<GetResponseRequest, GetResponseResponse>>(
      stub_->GetResponse(&context_));

  GetResponseRequest first_request;
  *first_request.mutable_get_response_config() = get_response_config_;
  stream_->Write(first_request);

  response_thread_ =
      std::unique_ptr<std::thread>(new std::thread([this, res_func] {
        GetResponseResponse resp;
        while (stream_->Read(&resp)) {
          res_func(resp);
        }
      }));
}

grpc::Status Interaction::Stop() {
  stream_->WritesDone();
  grpc::Status status;
  if (stream_ != nullptr) {
    status = stream_->Finish();
    if (!status.ok()) {
      std::cout << "Convai Service Error: " << status.error_message();
    }
  }
  if (response_thread_ != nullptr) {
    response_thread_->join();
  }
  return status;
}

void Interaction::SendText(const std::string &text) {
  GetResponseRequest req;
  req.mutable_get_response_data()->set_text_data(text);
  stream_->Write(req);
}

void Interaction::SendAudio(char *audio_data, int64_t length) {
  GetResponseRequest req;
  req.mutable_get_response_data()->set_audio_data(audio_data, length);
  stream_->Write(req);
};

}   // namespace convai_sdk