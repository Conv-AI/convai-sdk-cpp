#ifndef _CONVAI_SDK_PROTO_UTILS_H_
#define _CONVAI_SDK_PROTO_UTILS_H_

#include "convai.grpc.pb.h"
#include "google/protobuf/text_format.h"
#include <fstream>
#include <sstream>
#include <string>

namespace convai_sdk {
namespace proto_util {

using ::service::GetResponseRequest;

// Parses the file content in `file_path` into GetResponseConfig proto and
// returns the proto object.
static inline GetResponseRequest::GetResponseConfig
GetResponseConfigFromFile(const std::string &file_path) {
  std::ifstream f(file_path);
  std::stringstream buffer;
  buffer << f.rdbuf();
  GetResponseRequest::GetResponseConfig get_response_config;
  bool proto_parse_success = google::protobuf::TextFormat::ParseFromString(
      buffer.str(), &get_response_config);
  if (!proto_parse_success) {
    std::cout << "Cannot parse GetResponseConfig from file: " << file_path
              << "\n";
    exit(1);
  }
  return get_response_config;
};
}   // namespace proto_util
}   // namespace convai_sdk

#endif   // _CONVAI_SDK_PROTO_UTILS_H_