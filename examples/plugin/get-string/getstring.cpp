// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2022 Second State INC

#include "plugin/plugin.h"
#include "po/helper.h"
#include "runtime/callingframe.h"
#include "runtime/instance/module.h"
#include <algorithm>
#include <iostream>

namespace {

using namespace std::literals::string_view_literals;
using namespace WasmEdge;

PO::Option<std::string> StringOpt1(PO::Description("string to return"sv),
                                  PO::MetaVar("STRING1"sv),
                                  PO::DefaultValue<std::string>("hello1"));

PO::Option<std::string> StringOpt2(PO::Description("string to return2"sv),
                                  PO::MetaVar("STRIN2G"sv),
                                  PO::DefaultValue<std::string>("world"));

PO::Option<PO::Toggle> UpperOpt(PO::Description("return in upper case"sv));

void addOptions(const Plugin::Plugin::PluginDescriptor *,
                PO::ArgumentParser &Parser) noexcept {
  Parser.add_option("string1"sv, StringOpt1).add_option("string2", StringOpt2).add_option("upper"sv, UpperOpt);
}

class GetString : public Runtime::HostFunction<GetString> {
public:
  GetString(const std::string &String1, const std::string String2, bool Upper)
      : String1(String1), String2(String2), Upper(Upper) {}
  Expect<void> body(const Runtime::CallingFrame &Frame, uint32_t BufPtr,
                    uint32_t BufLen, uint32_t WrittenPtr) {
    // Check memory instance from module.
    auto *MemInst = Frame.getMemoryByIndex(0);
    if (MemInst == nullptr) {
      return Unexpect(ErrCode::Value::HostFuncError);
    }

    // Validate range of the buffer.
    auto Buf = MemInst->getSpan<char>(BufPtr, BufLen);
    if (unlikely(Buf.size() != BufLen)) {
      return Unexpect(ErrCode::Value::HostFuncError);
    }

    // Validate pointer to written count.
    uint32_t *const Written = MemInst->getPointer<uint32_t *>(WrittenPtr);
    if (unlikely(Written == nullptr)) {
      return Unexpect(ErrCode::Value::HostFuncError);
    }
    if(Upper){
      char *const End = std::copy(String1.begin(), String1.end(), Buf.data());
      *Written = End - Buf.data();
    }else{
      char *const End = std::copy(String2.begin(), String2.end(), Buf.data());
      *Written = End - Buf.data();
    }
    //    std::cout << *Written<<std::endl;
    return {};
  }

private:
  std::string_view String1;
  std::string_view  String2;
  bool Upper;
};

class PluginModule : public Runtime::Instance::ModuleInstance {
public:
  PluginModule() : ModuleInstance("module_wasm_name") {
    addHostFunc("get_string", std::make_unique<GetString>(StringOpt1.value(),
                                                          StringOpt2.value(),
                                                          UpperOpt.value()));
  }
};

Runtime::Instance::ModuleInstance *
create(const Plugin::PluginModule::ModuleDescriptor *) noexcept {
  return new PluginModule;
}

Plugin::Plugin::PluginDescriptor Descriptor{
    /* Name */ "plugin_name",
    /* Description */ "Example plugin",
    /* APIVersion */ Plugin::Plugin::CurrentAPIVersion,
    /* Version */ {0, 13, 5, 0},
    /* ModuleCount */ 1,
    /* ModuleDescriptions */
    (Plugin::PluginModule::ModuleDescriptor[]){
        {
            /* Name */ "module_name",
            /* Description */ "Example module",
            /* Create */ create,
        },
    },
    /* AddOptions */ addOptions,
};

EXPORT_GET_DESCRIPTOR(Descriptor)

} // namespace