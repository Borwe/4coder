cmake_minimum_required(VERSION 3.10)

project(4ed LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#for creating compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

#setup build mode
if(NOT BUILD_MODE)
  message(STATUS "Using Dev build mode none provided")
  set(BUILD_MODE -DDEV_BUILD)
endif()

#set warnings to be ignored
set(WARNINGS -Wno-write-strings -Wno-comment)

# use -fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#some compile flags
set(LINUX_COMPILE_FLAGS -D_GNU_SOURCE ${BUILD_MODE}-fpermissive)

#mark all sub scripts to inform them that we are in a main build
set(MAIN_BUILD 1)

#Steps
#1. build the super, custom_4coder.so
  #Old steps:
    #- change to build dir
    #- execute build script @ custom/bin/buildsuper_-(win|lnux|mac).(sh|bat)
    #- go to root dir
#So we use our custom cmake file at https://github.com/Borwe/4coder_cmake_builder
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/4coder_cmake_builder/)
  message(FATAL_ERROR "Please make sure you have gotten all submodules: git submodule update --init --recursive")
else()
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/4coder_cmake_builder/)
endif()
#2. build main.
  #Old steps:
    #- Change to build dir
    #- Build 4ed_app.so library
      #- add flags -m64, -DFTECH_64_BIT if 64 bit or
        #use flag -m32, -DFTECH32_BIT if 32 bit
      #- add definitions:
        #if flag SHIP  = -DSHIP_MODE
        #if INTERNAL = -DFRED_INTERNAL
        #if SUPER = -DFRED_SUPER
      #- use GCC options 
          #
          # "-Wno-write-strings "                 \
          #  "-D_GNU_SOURCE -fPIC "                \
          #  "-fno-threadsafe-statics -pthread "   \
          #  "-Wno-unused-result "                 \
          #  "-std=c++11 "                          \
          #  "$(pkg-config --cflags freetype2)"
      #- Add -I path as /code directory
      #- Add code/custom and ../4coder-non-source/ as -I
      #- If DEBUG_INFO set flag -g -O0
      #- If OPTIMIZATION set flag -03
      #- If SHARED_CODE set flag -shared
      #- Add files passed to command
      #- If LIBS set flags if 64 bit:
          #"-lX11 -lpthread -lm -lrt "   \
          #"-lGL -ldl -lXfixes -lfreetype -lfontconfig"
      #- Move to build dir
      #- run command g++ commands and build everything
add_library(4ed_app SHARED ${CMAKE_CURRENT_SOURCE_DIR}/4ed_app_target.cpp)
target_compile_definitions(4ed_app PUBLIC FRED_SUPER FRED_INTERNAL
  -D_GNU_SOURCE)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  target_compile_definitions(4ed_app PUBLIC FTECH_64_BIT)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
  target_compile_definitions(4ed_app PUBLIC FTECH_32_BIT)
endif()
target_include_directories(4ed_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../4coder-non-source/foreign/
  ${CMAKE_CURRENT_SOURCE_DIR}/custom)
target_compile_options(4ed_app PUBLIC -Wno-write-strings
  -fno-threadsafe-statics -pthread -Wno-unused-result)
find_package(Freetype)
target_link_libraries(4ed_app PUBLIC Freetype::Freetype
  -lX11 -lm -lrt -lGL -ldl -lXfixes -lfontconfig)
add_dependencies(4ed_app cleanup-metadata)
add_custom_target(copy-lib-to-4ed-app 
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/lib4ed_app.so
   ${CMAKE_CURRENT_BINARY_DIR}/4ed_app.so)
add_custom_target(remove-lib-4ed-app ALL
  COMMAND ${CMAKE_COMMAND} -E rm ${CMAKE_CURRENT_BINARY_DIR}/lib4ed_app.so)
add_dependencies(copy-lib-to-4ed-app 4ed_app)
add_dependencies(remove-lib-4ed-app copy-lib-to-4ed-app)


    # Build 4ed executable
     # repeat same proccess as for 4ed_app.so but with the source files as
     # LINUX: platform_linux/linux_4ed.cpp
     # MacOS: platform_mac/mac_4ed.mm
     # Windows: platform_win32/win32_4ed.cpp
    # Copy themes folder code/ship_files/themes to build/themes 
      #build being build directory.
