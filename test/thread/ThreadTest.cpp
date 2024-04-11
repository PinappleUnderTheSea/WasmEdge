// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2022 Second State INC

//===-- wasmedge/test/thread/ThreadTest.cpp - Multithread test ------------===//
//
// Part of the WasmEdge Project.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains tests for running Executor in parallel multithread way.
///
//===----------------------------------------------------------------------===//

#include "common/log.h"
#include "vm/vm.h"

#ifdef WASMEDGE_USE_LLVM
#include "llvm/codegen.h"
#include "llvm/compiler.h"
#endif

#include "gtest/gtest.h"

#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace {

// See mt19937.c for source of this webassembly data.
std::array<WasmEdge::Byte, 1481> MersenneTwister19937{
    0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00, 0x01, 0x08, 0x01, 0x60,
    0x03, 0x7f, 0x7e, 0x7e, 0x01, 0x7e, 0x03, 0x02, 0x01, 0x00, 0x05, 0x03,
    0x01, 0x00, 0x01, 0x07, 0x0b, 0x01, 0x07, 0x6d, 0x74, 0x31, 0x39, 0x39,
    0x33, 0x37, 0x00, 0x00, 0x0a, 0x9e, 0x0b, 0x01, 0x9b, 0x0b, 0x03, 0x02,
    0x7f, 0x03, 0x7e, 0x01, 0x7b, 0x20, 0x00, 0x20, 0x01, 0x37, 0x03, 0x00,
    0x20, 0x00, 0x41, 0x20, 0x6a, 0x21, 0x03, 0x42, 0x01, 0x21, 0x06, 0x42,
    0x04, 0x21, 0x05, 0x03, 0x7e, 0x20, 0x03, 0x41, 0x18, 0x6b, 0x20, 0x05,
    0x20, 0x01, 0x42, 0x3e, 0x88, 0x20, 0x01, 0x85, 0x42, 0xad, 0xfe, 0xd5,
    0xe4, 0xd4, 0x85, 0xfd, 0xa8, 0xd8, 0x00, 0x7e, 0x7c, 0x42, 0x03, 0x7d,
    0x22, 0x01, 0x37, 0x03, 0x00, 0x20, 0x03, 0x41, 0x10, 0x6b, 0x20, 0x05,
    0x20, 0x01, 0x42, 0x3e, 0x88, 0x20, 0x01, 0x85, 0x42, 0xad, 0xfe, 0xd5,
    0xe4, 0xd4, 0x85, 0xfd, 0xa8, 0xd8, 0x00, 0x7e, 0x7c, 0x42, 0x02, 0x7d,
    0x22, 0x01, 0x37, 0x03, 0x00, 0x20, 0x03, 0x41, 0x08, 0x6b, 0x20, 0x05,
    0x20, 0x01, 0x42, 0x3e, 0x88, 0x20, 0x01, 0x85, 0x42, 0xad, 0xfe, 0xd5,
    0xe4, 0xd4, 0x85, 0xfd, 0xa8, 0xd8, 0x00, 0x7e, 0x7c, 0x42, 0x01, 0x7d,
    0x22, 0x01, 0x37, 0x03, 0x00, 0x20, 0x05, 0x42, 0xb8, 0x02, 0x51, 0x04,
    0x7e, 0x20, 0x00, 0x42, 0xb8, 0x02, 0x37, 0x03, 0xc0, 0x13, 0x02, 0x40,
    0x20, 0x02, 0x50, 0x04, 0x40, 0x42, 0x00, 0x21, 0x02, 0x42, 0xb8, 0x02,
    0x21, 0x05, 0x0c, 0x01, 0x0b, 0x42, 0x00, 0x21, 0x01, 0x03, 0x40, 0x42,
    0x9c, 0x01, 0x21, 0x05, 0x20, 0x00, 0x21, 0x03, 0x03, 0x40, 0x20, 0x03,
    0x20, 0x03, 0xfd, 0x00, 0x03, 0x08, 0x22, 0x08, 0xfd, 0x0c, 0xfe, 0xff,
    0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x7f, 0x00, 0x00,
    0x00, 0x00, 0xfd, 0x4e, 0x20, 0x03, 0xfd, 0x00, 0x03, 0x00, 0xfd, 0x0c,
    0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x80,
    0xff, 0xff, 0xff, 0xff, 0xfd, 0x4e, 0xfd, 0x50, 0x41, 0x01, 0xfd, 0xcd,
    0x01, 0x20, 0x03, 0x41, 0xe0, 0x09, 0x6a, 0xfd, 0x00, 0x03, 0x00, 0xfd,
    0x51, 0xfd, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfd, 0x0c, 0xe9, 0x19, 0x66,
    0xa9, 0x5a, 0x6f, 0x02, 0xb5, 0xe9, 0x19, 0x66, 0xa9, 0x5a, 0x6f, 0x02,
    0xb5, 0xfd, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x08, 0xfd, 0x0c, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xfd, 0x4e, 0xfd, 0xd6, 0x01, 0xfd, 0x52, 0xfd, 0x51,
    0xfd, 0x0b, 0x03, 0x00, 0x20, 0x03, 0x41, 0x10, 0x6a, 0x21, 0x03, 0x20,
    0x05, 0x42, 0x02, 0x7d, 0x22, 0x05, 0x42, 0x00, 0x52, 0x0d, 0x00, 0x0b,
    0x42, 0x9a, 0x01, 0x21, 0x05, 0x20, 0x00, 0x21, 0x03, 0x03, 0x40, 0x20,
    0x03, 0x41, 0xe0, 0x09, 0x6a, 0x22, 0x04, 0x20, 0x03, 0x41, 0xe8, 0x09,
    0x6a, 0xfd, 0x00, 0x03, 0x00, 0x22, 0x08, 0xfd, 0x0c, 0xfe, 0xff, 0xff,
    0x7f, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00,
    0x00, 0xfd, 0x4e, 0x20, 0x04, 0xfd, 0x00, 0x03, 0x00, 0xfd, 0x0c, 0x00,
    0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x80, 0xff,
    0xff, 0xff, 0xff, 0xfd, 0x4e, 0xfd, 0x50, 0x41, 0x01, 0xfd, 0xcd, 0x01,
    0x20, 0x03, 0xfd, 0x00, 0x03, 0x00, 0xfd, 0x51, 0xfd, 0x0c, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xfd, 0x0c, 0xe9, 0x19, 0x66, 0xa9, 0x5a, 0x6f, 0x02, 0xb5,
    0xe9, 0x19, 0x66, 0xa9, 0x5a, 0x6f, 0x02, 0xb5, 0x20, 0x08, 0xfd, 0x0c,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xfd, 0x4e, 0xfd, 0x0c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xfd, 0xd6, 0x01, 0xfd, 0x52, 0xfd, 0x51, 0xfd, 0x0b, 0x03, 0x00, 0x20,
    0x03, 0x41, 0x10, 0x6a, 0x21, 0x03, 0x20, 0x05, 0x42, 0x02, 0x7d, 0x22,
    0x05, 0x42, 0x00, 0x52, 0x0d, 0x00, 0x0b, 0x42, 0x00, 0x21, 0x05, 0x20,
    0x00, 0x42, 0x00, 0x37, 0x03, 0xc0, 0x13, 0x20, 0x00, 0x42, 0x00, 0x20,
    0x00, 0x29, 0x03, 0xb8, 0x13, 0x22, 0x06, 0x42, 0x01, 0x83, 0x7d, 0x42,
    0xe9, 0xb3, 0x98, 0xcb, 0xaa, 0xeb, 0x9b, 0x81, 0xb5, 0x7f, 0x83, 0x20,
    0x00, 0x29, 0x03, 0xd0, 0x09, 0x20, 0x06, 0x42, 0xfe, 0xff, 0xff, 0xff,
    0x07, 0x83, 0x20, 0x00, 0x29, 0x03, 0xb0, 0x13, 0x42, 0x80, 0x80, 0x80,
    0x80, 0x78, 0x83, 0x84, 0x42, 0x01, 0x88, 0x85, 0x85, 0x37, 0x03, 0xb0,
    0x13, 0x20, 0x00, 0x42, 0x00, 0x20, 0x00, 0x29, 0x03, 0x00, 0x22, 0x07,
    0x42, 0x01, 0x83, 0x7d, 0x42, 0xe9, 0xb3, 0x98, 0xcb, 0xaa, 0xeb, 0x9b,
    0x81, 0xb5, 0x7f, 0x83, 0x20, 0x00, 0x29, 0x03, 0xd8, 0x09, 0x20, 0x07,
    0x42, 0xfe, 0xff, 0xff, 0xff, 0x07, 0x83, 0x20, 0x06, 0x42, 0x80, 0x80,
    0x80, 0x80, 0x78, 0x83, 0x84, 0x42, 0x01, 0x88, 0x85, 0x85, 0x37, 0x03,
    0xb8, 0x13, 0x20, 0x01, 0x20, 0x02, 0x7c, 0x21, 0x02, 0x42, 0xc8, 0x7d,
    0x21, 0x01, 0x20, 0x02, 0x42, 0xb8, 0x02, 0x56, 0x0d, 0x00, 0x0b, 0x0b,
    0x20, 0x00, 0x20, 0x02, 0x20, 0x05, 0x7c, 0x22, 0x05, 0x37, 0x03, 0xc0,
    0x13, 0x20, 0x05, 0x42, 0xb8, 0x02, 0x5a, 0x04, 0x40, 0x42, 0x9c, 0x01,
    0x21, 0x05, 0x20, 0x00, 0x21, 0x03, 0x03, 0x40, 0x20, 0x03, 0x20, 0x03,
    0xfd, 0x00, 0x03, 0x08, 0x22, 0x08, 0xfd, 0x0c, 0xfe, 0xff, 0xff, 0x7f,
    0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00,
    0xfd, 0x4e, 0x20, 0x03, 0xfd, 0x00, 0x03, 0x00, 0xfd, 0x0c, 0x00, 0x00,
    0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff,
    0xff, 0xff, 0xfd, 0x4e, 0xfd, 0x50, 0x41, 0x01, 0xfd, 0xcd, 0x01, 0x20,
    0x03, 0x41, 0xe0, 0x09, 0x6a, 0xfd, 0x00, 0x03, 0x00, 0xfd, 0x51, 0xfd,
    0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xfd, 0x0c, 0xe9, 0x19, 0x66, 0xa9, 0x5a,
    0x6f, 0x02, 0xb5, 0xe9, 0x19, 0x66, 0xa9, 0x5a, 0x6f, 0x02, 0xb5, 0xfd,
    0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x08, 0xfd, 0x0c, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xfd, 0x4e, 0xfd, 0xd6, 0x01, 0xfd, 0x52, 0xfd, 0x51, 0xfd, 0x0b,
    0x03, 0x00, 0x20, 0x03, 0x41, 0x10, 0x6a, 0x21, 0x03, 0x20, 0x05, 0x42,
    0x02, 0x7d, 0x22, 0x05, 0x42, 0x00, 0x52, 0x0d, 0x00, 0x0b, 0x42, 0x9a,
    0x01, 0x21, 0x05, 0x20, 0x00, 0x21, 0x03, 0x03, 0x40, 0x20, 0x03, 0x41,
    0xe0, 0x09, 0x6a, 0x22, 0x04, 0x20, 0x03, 0x41, 0xe8, 0x09, 0x6a, 0xfd,
    0x00, 0x03, 0x00, 0x22, 0x08, 0xfd, 0x0c, 0xfe, 0xff, 0xff, 0x7f, 0x00,
    0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xfd,
    0x4e, 0x20, 0x04, 0xfd, 0x00, 0x03, 0x00, 0xfd, 0x0c, 0x00, 0x00, 0x00,
    0x80, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff,
    0xff, 0xfd, 0x4e, 0xfd, 0x50, 0x41, 0x01, 0xfd, 0xcd, 0x01, 0x20, 0x03,
    0xfd, 0x00, 0x03, 0x00, 0xfd, 0x51, 0xfd, 0x0c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xfd, 0x0c, 0xe9, 0x19, 0x66, 0xa9, 0x5a, 0x6f, 0x02, 0xb5, 0xe9, 0x19,
    0x66, 0xa9, 0x5a, 0x6f, 0x02, 0xb5, 0x20, 0x08, 0xfd, 0x0c, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xfd, 0x4e, 0xfd, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfd, 0xd6,
    0x01, 0xfd, 0x52, 0xfd, 0x51, 0xfd, 0x0b, 0x03, 0x00, 0x20, 0x03, 0x41,
    0x10, 0x6a, 0x21, 0x03, 0x20, 0x05, 0x42, 0x02, 0x7d, 0x22, 0x05, 0x42,
    0x00, 0x52, 0x0d, 0x00, 0x0b, 0x20, 0x00, 0x42, 0x00, 0x20, 0x00, 0x29,
    0x03, 0xb8, 0x13, 0x22, 0x01, 0x42, 0x01, 0x83, 0x7d, 0x42, 0xe9, 0xb3,
    0x98, 0xcb, 0xaa, 0xeb, 0x9b, 0x81, 0xb5, 0x7f, 0x83, 0x20, 0x00, 0x29,
    0x03, 0xd0, 0x09, 0x20, 0x01, 0x42, 0xfe, 0xff, 0xff, 0xff, 0x07, 0x83,
    0x20, 0x00, 0x29, 0x03, 0xb0, 0x13, 0x42, 0x80, 0x80, 0x80, 0x80, 0x78,
    0x83, 0x84, 0x42, 0x01, 0x88, 0x85, 0x85, 0x37, 0x03, 0xb0, 0x13, 0x20,
    0x00, 0x42, 0x00, 0x20, 0x00, 0x29, 0x03, 0x00, 0x22, 0x02, 0x42, 0x01,
    0x83, 0x7d, 0x42, 0xe9, 0xb3, 0x98, 0xcb, 0xaa, 0xeb, 0x9b, 0x81, 0xb5,
    0x7f, 0x83, 0x20, 0x00, 0x29, 0x03, 0xd8, 0x09, 0x20, 0x02, 0x42, 0xfe,
    0xff, 0xff, 0xff, 0x07, 0x83, 0x20, 0x01, 0x42, 0x80, 0x80, 0x80, 0x80,
    0x78, 0x83, 0x84, 0x42, 0x01, 0x88, 0x85, 0x85, 0x37, 0x03, 0xb8, 0x13,
    0x42, 0x00, 0x21, 0x05, 0x0b, 0x20, 0x00, 0x20, 0x05, 0x42, 0x01, 0x7c,
    0x37, 0x03, 0xc0, 0x13, 0x20, 0x00, 0x20, 0x05, 0xa7, 0x41, 0x03, 0x74,
    0x6a, 0x29, 0x03, 0x00, 0x22, 0x01, 0x42, 0x1d, 0x88, 0x42, 0xd5, 0xaa,
    0xd5, 0xaa, 0xd5, 0x00, 0x83, 0x20, 0x01, 0x85, 0x22, 0x01, 0x42, 0x11,
    0x86, 0x42, 0x80, 0x80, 0x98, 0xed, 0xfe, 0xff, 0x9f, 0xeb, 0xf1, 0x00,
    0x83, 0x20, 0x01, 0x85, 0x22, 0x01, 0x42, 0x25, 0x86, 0x42, 0x80, 0x80,
    0x80, 0x80, 0x80, 0xdc, 0xfb, 0x7b, 0x83, 0x20, 0x01, 0x85, 0x22, 0x01,
    0x42, 0x2b, 0x88, 0x20, 0x01, 0x85, 0x05, 0x20, 0x06, 0x42, 0x03, 0x7c,
    0x21, 0x07, 0x20, 0x03, 0x20, 0x01, 0x42, 0x3e, 0x88, 0x20, 0x01, 0x85,
    0x42, 0xad, 0xfe, 0xd5, 0xe4, 0xd4, 0x85, 0xfd, 0xa8, 0xd8, 0x00, 0x7e,
    0x22, 0x01, 0x20, 0x05, 0x7c, 0x37, 0x03, 0x00, 0x20, 0x03, 0x41, 0x20,
    0x6a, 0x21, 0x03, 0x20, 0x05, 0x42, 0x04, 0x7c, 0x21, 0x05, 0x20, 0x06,
    0x42, 0x04, 0x7c, 0x21, 0x06, 0x20, 0x01, 0x20, 0x07, 0x7c, 0x21, 0x01,
    0x0c, 0x01, 0x0b, 0x0b, 0x0b,
};
std::array<uint64_t, 4> Answers{
    UINT64_C(7605900683918645917),
    UINT64_C(9082641531226583590),
    UINT64_C(4446454406775736720),
    UINT64_C(9019442596657776185),
};

using namespace std::literals;

TEST(AsyncExecute, ThreadTest) {
  WasmEdge::Configure Conf;
  WasmEdge::VM::VM VM(Conf);
  ASSERT_TRUE(VM.loadWasm(MersenneTwister19937));
  ASSERT_TRUE(VM.validate());
  ASSERT_TRUE(VM.instantiate());
  {
    std::array<WasmEdge::Async<WasmEdge::Expect<std::vector<
                   std::pair<WasmEdge::ValVariant, WasmEdge::ValType>>>>,
               4>
        AsyncResults;
    for (uint64_t Index = 0; Index < Answers.size(); ++Index) {
      AsyncResults[Index] = VM.asyncExecute(
          "mt19937",
          std::initializer_list<WasmEdge::ValVariant>{
              UINT32_C(2504) * Index, UINT64_C(5489), UINT64_C(100000) + Index},
          {WasmEdge::ValType(WasmEdge::TypeCode::I32),
           WasmEdge::ValType(WasmEdge::TypeCode::I64),
           WasmEdge::ValType(WasmEdge::TypeCode::I64)});
    }
    for (uint64_t Index = 0; Index < Answers.size(); ++Index) {
      auto Result = AsyncResults[Index].get();
      ASSERT_TRUE(Result);
      ASSERT_EQ((*Result)[0].second.getCode(), WasmEdge::TypeCode::I64);
      EXPECT_EQ((*Result)[0].first.get<uint64_t>(), Answers[Index]);
    }
  }
}

TEST(AsyncExecute, GasThreadTest) {
  WasmEdge::Configure Conf;
  Conf.getStatisticsConfigure().setInstructionCounting(true);
  Conf.getStatisticsConfigure().setCostMeasuring(true);
  Conf.getStatisticsConfigure().setTimeMeasuring(true);
  WasmEdge::VM::VM VM(Conf);
  ASSERT_TRUE(VM.loadWasm(MersenneTwister19937));
  ASSERT_TRUE(VM.validate());
  ASSERT_TRUE(VM.instantiate());
  {
    std::array<WasmEdge::Async<WasmEdge::Expect<std::vector<
                   std::pair<WasmEdge::ValVariant, WasmEdge::ValType>>>>,
               4>
        AsyncResults;
    for (uint64_t Index = 0; Index < Answers.size(); ++Index) {
      AsyncResults[Index] = VM.asyncExecute(
          "mt19937",
          std::initializer_list<WasmEdge::ValVariant>{
              UINT32_C(2504) * Index, UINT64_C(5489), UINT64_C(100000) + Index},
          {WasmEdge::ValType(WasmEdge::TypeCode::I32),
           WasmEdge::ValType(WasmEdge::TypeCode::I64),
           WasmEdge::ValType(WasmEdge::TypeCode::I64)});
    }
    for (uint64_t Index = 0; Index < Answers.size(); ++Index) {
      auto Result = AsyncResults[Index].get();
      ASSERT_TRUE(Result);
      ASSERT_EQ((*Result)[0].second.getCode(), WasmEdge::TypeCode::I64);
      EXPECT_EQ((*Result)[0].first.get<uint64_t>(), Answers[Index]);
    }
  }
}

#ifdef WASMEDGE_USE_LLVM

TEST(AOTAsyncExecute, ThreadTest) {
  WasmEdge::Configure Conf;
  Conf.getCompilerConfigure().setInterruptible(true);
  Conf.getCompilerConfigure().setOutputFormat(
      WasmEdge::CompilerConfigure::OutputFormat::Native);
  const auto Path =
      std::filesystem::temp_directory_path() /
      std::filesystem::u8path("ThreadTest" WASMEDGE_LIB_EXTENSION);
  {
    WasmEdge::Loader::Loader Loader(Conf);
    WasmEdge::Validator::Validator ValidatorEngine(Conf);
    WasmEdge::LLVM::Compiler Compiler(Conf);
    WasmEdge::LLVM::CodeGen CodeGen(Conf);
    auto Module = *Loader.parseModule(MersenneTwister19937);
    ASSERT_TRUE(ValidatorEngine.validate(*Module));
    auto Data = Compiler.compile(*Module);
    ASSERT_TRUE(Data);
    ASSERT_TRUE(CodeGen.codegen(MersenneTwister19937, std::move(*Data), Path));
  }

  WasmEdge::VM::VM VM(Conf);
  ASSERT_TRUE(VM.loadWasm(Path));
  ASSERT_TRUE(VM.validate());
  ASSERT_TRUE(VM.instantiate());
  {
    std::array<WasmEdge::Async<WasmEdge::Expect<std::vector<
                   std::pair<WasmEdge::ValVariant, WasmEdge::ValType>>>>,
               4>
        AsyncResults;
    for (uint64_t Index = 0; Index < Answers.size(); ++Index) {
      AsyncResults[Index] = VM.asyncExecute(
          "mt19937",
          std::initializer_list<WasmEdge::ValVariant>{
              UINT32_C(2504) * Index, UINT64_C(5489), UINT64_C(100000) + Index},
          {WasmEdge::ValType(WasmEdge::TypeCode::I32),
           WasmEdge::ValType(WasmEdge::TypeCode::I64),
           WasmEdge::ValType(WasmEdge::TypeCode::I64)});
    }
    for (uint64_t Index = 0; Index < Answers.size(); ++Index) {
      auto Result = AsyncResults[Index].get();
      ASSERT_TRUE(Result);
      ASSERT_EQ((*Result)[0].second.getCode(), WasmEdge::TypeCode::I64);
      EXPECT_EQ((*Result)[0].first.get<uint64_t>(), Answers[Index]);
    }
  }
  VM.cleanup();
  EXPECT_NO_THROW(std::filesystem::remove(Path));
}

TEST(AOTAsyncExecute, GasThreadTest) {
  WasmEdge::Configure Conf;
  Conf.getCompilerConfigure().setInterruptible(true);
  Conf.getStatisticsConfigure().setInstructionCounting(true);
  Conf.getStatisticsConfigure().setCostMeasuring(true);
  Conf.getStatisticsConfigure().setTimeMeasuring(true);
  Conf.getCompilerConfigure().setOutputFormat(
      WasmEdge::CompilerConfigure::OutputFormat::Native);
  auto Path = std::filesystem::temp_directory_path() /
              std::filesystem::u8path("AOTGasTest" WASMEDGE_LIB_EXTENSION);

  {
    WasmEdge::Loader::Loader Loader(Conf);
    WasmEdge::Validator::Validator ValidatorEngine(Conf);
    WasmEdge::LLVM::Compiler Compiler(Conf);
    WasmEdge::LLVM::CodeGen CodeGen(Conf);
    auto Module = *Loader.parseModule(MersenneTwister19937);
    ASSERT_TRUE(ValidatorEngine.validate(*Module));
    auto Data = Compiler.compile(*Module);
    ASSERT_TRUE(Data);
    ASSERT_TRUE(CodeGen.codegen(MersenneTwister19937, std::move(*Data), Path));
  }

  WasmEdge::VM::VM VM(Conf);
  ASSERT_TRUE(VM.loadWasm(Path));
  ASSERT_TRUE(VM.validate());
  ASSERT_TRUE(VM.instantiate());
  {
    std::array<WasmEdge::Async<WasmEdge::Expect<std::vector<
                   std::pair<WasmEdge::ValVariant, WasmEdge::ValType>>>>,
               4>
        AsyncResults;
    for (uint64_t Index = 0; Index < Answers.size(); ++Index) {
      AsyncResults[Index] = VM.asyncExecute(
          "mt19937",
          std::initializer_list<WasmEdge::ValVariant>{
              UINT32_C(2504) * Index, UINT64_C(5489), UINT64_C(100000) + Index},
          {WasmEdge::ValType(WasmEdge::TypeCode::I32),
           WasmEdge::ValType(WasmEdge::TypeCode::I64),
           WasmEdge::ValType(WasmEdge::TypeCode::I64)});
    }
    for (uint64_t Index = 0; Index < Answers.size(); ++Index) {
      auto Result = AsyncResults[Index].get();
      ASSERT_TRUE(Result);
      ASSERT_EQ((*Result)[0].second.getCode(), WasmEdge::TypeCode::I64);
      EXPECT_EQ((*Result)[0].first.get<uint64_t>(), Answers[Index]);
    }
  }
  VM.cleanup();
  EXPECT_NO_THROW(std::filesystem::remove(Path));
}

#endif

} // namespace

GTEST_API_ int main(int argc, char **argv) {
  WasmEdge::Log::setErrorLoggingLevel();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
