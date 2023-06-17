##### Protobuf Rules for Bazel
##### See https://github.com/bazelbuild/rules_proto
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")

http_archive(
    name = "rules_proto",
    sha256 = "80d3a4ec17354cccc898bfe32118edd934f851b03029d63ef3fc7c8663a7415c",
    strip_prefix = "rules_proto-5.3.0-21.5",
    urls = [
        "https://github.com/bazelbuild/rules_proto/archive/refs/tags/5.3.0-21.5.tar.gz",
    ],
)
load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")
rules_proto_dependencies()
rules_proto_toolchains()

##### gRPC Rules for Bazel
##### See https://github.com/grpc/grpc/blob/master/src/cpp/README.md#make
##### Release: https://github.com/grpc/grpc/commit/0e34ec3396726ba44a7aaf752eccac0e998beaa8
http_archive(
    name = "com_github_grpc_grpc",
    urls = [
        "https://github.com/grpc/grpc/archive/0e34ec3396726ba44a7aaf752eccac0e998beaa8.tar.gz",
    ],
    strip_prefix = "grpc-0e34ec3396726ba44a7aaf752eccac0e998beaa8",
)
load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")
grpc_deps()
load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")
grpc_extra_deps()

new_git_repository(
    name = "portaudio",
    build_file_content = """
load("@rules_cc//cc:defs.bzl", "cc_library")
package(default_visibility = ["//visibility:public"])
licenses(["reciprocal"])
exports_files(["LICENSE.txt"])
cc_library(
    name = "portaudio",
    srcs = glob([
        "src/common/*.c",
        "src/common/*.h",
        "include/*.h",
    ]) + select({
        "@platforms//os:linux": glob([
            "src/hostapi/alsa/*.c",
            "src/hostapi/alsa/*.h",
            "src/os/unix/*.c",
            "src/os/unix/*.h",
        ]),
        "@platforms//os:osx": glob([
            "src/hostapi/coreaudio/*.c",
            "src/hostapi/coreaudio/*.h",
            "src/os/unix/*.c",
            "src/os/unix/*.h",
        ]),
        "@platforms//os:windows": glob([
            "src/hostapi/wmme/*.c",
            "src/hostapi/wmme/*.h",
            "src/os/win/*.c",
            "src/os/win/*.h",
        ]),
        "//conditions:default": [],
    }),
    hdrs = ["include/portaudio.h"],
    copts = select({
        "@platforms//os:windows": ["/w"],
        "//conditions:default": ["-w"],
    }),
    defines = select({
        "@platforms//os:linux": ["PA_USE_ALSA=1"],
        "@platforms//os:osx": ["PA_USE_COREAUDIO=1"],
        "@platforms//os:windows": ["PA_USE_WMME=1"],
        "//conditions:default": [],
    }),
    includes = [
        "include",
        "src/common",
        "src/os/unix",
    ],
    linkopts = select({
        "@platforms//os:linux": [
            "-lasound",
            "-pthread",
        ],
        "@platforms//os:osx": [
            "-framework AudioUnit",
            "-framework CoreAudio",
            "-framework AudioToolbox",
            "-framework Carbon",
        ],
        "//conditions:default": [],
    }),
)
    """,
    commit = "147dd722548358763a8b649b3e4b41dfffbcfbb6",
    remote = "https://github.com/PortAudio/portaudio.git",
    shallow_since = "1617236250 -0700",
)

