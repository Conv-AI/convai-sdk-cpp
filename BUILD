package(default_visibility = ["//visibility:public"])

load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")
load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

proto_library(
    name = "convai_proto",
    srcs = ["convai.proto"]
)

cc_proto_library(
    name = "convai_cc_proto",
    deps = [":convai_proto"]
)

cc_grpc_library(
    name = "convai_cc_grpc",
    srcs = [":convai_proto"],
    grpc_only = True,
    deps = [":convai_cc_proto"]
)

cc_library(
    name = "interaction",
    srcs = ["interaction.cc"],
    hdrs = ["interaction.h"],
    deps = [
        ":convai_cc_grpc", 
        "@com_github_grpc_grpc//:grpc++"
    ]
)

cc_library(
    name = "audio",
    srcs = ["audio.cc"],
    hdrs = ["audio.h"],
    deps = [
        "@portaudio"
    ]
)

cc_library(
    name = "proto_util",
    hdrs = ["proto_utils.h"],
    deps = [
        ":convai_cc_grpc",
        "@com_google_protobuf//:protobuf",
    ]
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        ":interaction",
        ":audio",
        ":proto_util",
    ],
)