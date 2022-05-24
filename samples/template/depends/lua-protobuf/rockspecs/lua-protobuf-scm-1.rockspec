package = "lua-protobuf"
version = "scm-1"

source = {
  url = "git://github.com/starwing/lua-protobuf.git",
}

description = {
  summary = "protobuf data support for Lua",
  detailed = [[
This project offers a simple C library for basic protobuf wire format encode/decode.
  ]],
  homepage = "https://github.com/starwing/lua-protobuf",
  license = "MIT",
}

dependencies = {
  "lua >= 5.1"
}

build = {
  type = "builtin",
  modules = {
    pb     = "pb.c";
    protoc = "protoc.lua";
  }
}
