# Copyright 2026 Maxim Pavlov (MaxEELabs)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.25)

# Standalone production build for CI and terminals. MPLAB generates its own
# build under cmake/PICnUSB/default from .vscode/PICnUSB.mplab.json.
set(CMAKE_SYSTEM_NAME Generic)
find_program(XC8_CC NAMES xc8-cc REQUIRED)
set(CMAKE_C_COMPILER "${XC8_CC}" CACHE FILEPATH "XC8 compiler")
set(CMAKE_C_COMPILER_WORKS TRUE CACHE BOOL "Cross compiler cannot run on host")
set(CMAKE_C_ABI_COMPILED YES)
set(CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_CURRENT_LIST_DIR}/cmake/xc8-rules.cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(PICnUSB LANGUAGES C)

set(DFP_PATH "" CACHE PATH "Path to PIC12-16F1xxx_DFP 1.10.270's xc8 directory")
if(NOT EXISTS "${DFP_PATH}/pic/dat/ini/16f1455.ini")
  message(FATAL_ERROR "Set DFP_PATH to the xc8 directory of PIC12-16F1xxx_DFP 1.10.270")
endif()

# Match the main.c and src/**/*.c file set in the MPLAB project.
file(GLOB_RECURSE application_sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
add_executable(firmware main.c ${application_sources})
set_target_properties(firmware PROPERTIES
  OUTPUT_NAME default
  SUFFIX .elf
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/out/ci/PICnUSB")

set(xc8_options -mcpu=16F1455 "-mdfp=${DFP_PATH}" -O0 -std=c99 -mno-default-config-bits)
target_compile_options(firmware PRIVATE ${xc8_options})
target_link_options(firmware PRIVATE ${xc8_options})
