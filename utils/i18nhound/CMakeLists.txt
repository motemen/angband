cmake_minimum_required(VERSION 3.0)
project(i18nhound VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

add_executable(i18nhound main.cpp)

target_include_directories(i18nhound PRIVATE ${CLANG_INCLUDE_DIRS})
target_link_libraries(i18nhound PRIVATE
  clangAST
  clangBasic
  clangFrontend
  clangTooling
)
