#ifndef _CONVAI_SDK_INTERACTION_H_
#define _CONVAI_SDK_INTERACTION_H_

#include <iostream>
#include <string>
#include <thread>

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/grpcpp.h>

#include "convai.grpc.pb.h"

namespace convai_sdk {

using ::grpc::Channel;
using ::grpc::ClientContext;
using ::grpc::ClientReader;
using ::grpc::ClientReaderWriter;
using ::grpc::ClientWriter;
using ::grpc::Status;
using ::service::ConvaiService;
using ::service::GetResponseRequest;
using ::service::GetResponseResponse;

// This class is a wrapper around the Convai GetResponse API.
// Each interaction is meant to serve a user query and give back a response. To
// provide low latency both the user query and the response may arrive in small
// chunks. This is enabled via gRPC bi-directional streaming.
// 1. Instantiate the Interaction by providing a GetResponseConfig. (A sample
//    has been provided in the repository: 'get_response_config_example.txt').
// 2. Call Interaction::Start when you are ready to begin the interaction. The
//    Start method accepts a callback function which will execute everytime
//    there is a response.
// 3. To send chunks of user query call `SendText` or `SendAudio`. In a given
//    interaction only one of these should be called repeatedly. Mixing the two
//    calls will result in an error.
// 4. After sending all of the user query call `Stop`. This will signal the API
//    to start building a response.
// Before going out of scope the object will wait for all responses to be
// received.
class Interaction {
public:
  Interaction(const GetResponseRequest::GetResponseConfig &get_response_config)
      : get_response_config_(get_response_config) {}
  void Start(std::function<void(GetResponseResponse)> response_callback);
  grpc::Status Stop();
  void SendText(const std::string &text);
  void SendAudio(char *audio_data, int64_t length);
  ~Interaction() = default;

private:
  const GetResponseRequest::GetResponseConfig get_response_config_;

  // Bidirectional stream
  std::shared_ptr<ClientReaderWriter<GetResponseRequest, GetResponseResponse>>
      stream_ = nullptr;

  std::shared_ptr<grpc::Channel> rpc_channel_ = nullptr;
  std::unique_ptr<ConvaiService::Stub> stub_ = nullptr;
  ClientContext context_;

  // Thread under which the rpc response callback will be run.
  std::unique_ptr<std::thread> response_thread_ = nullptr;
};

}   // namespace convai_sdk

#endif   // _CONVAI_SDK_INTERACTION_H_