// Copyright 2012 The Emscripten Authors.  All rights reserved.
// Emscripten is available under two separate licenses, the MIT license and the
// University of Illinois/NCSA Open Source License.  Both these licenses can be
// found in the LICENSE file.

#include <emscripten/bind.h>
#ifdef USE_CXA_DEMANGLE
#include <../lib/libcxxabi/include/cxxabi.h>
#endif
#include <algorithm>
#include <climits>
#include <emscripten/emscripten.h>
#include <emscripten/wire.h>
#include <limits>
#include <list>
#include <typeinfo>
#include <vector>

using namespace emscripten;

extern "C" {
const char* EMSCRIPTEN_KEEPALIVE __getTypeName(const std::type_info* ti) {
  if (has_unbound_type_names) {
#ifdef USE_CXA_DEMANGLE
    int stat;
    char* demangled = abi::__cxa_demangle(ti->name(), NULL, NULL, &stat);
    if (stat == 0 && demangled) {
      return demangled;
    }

    switch (stat) {
      case -1:
        return strdup("<allocation failure>");
      case -2:
        return strdup("<invalid C++ symbol>");
      case -3:
        return strdup("<invalid argument>");
      default:
        return strdup("<unknown error>");
    }
#else
    return strdup(ti->name());
#endif
  } else {
    char str[80];
    sprintf(str, "%p", reinterpret_cast<const void*>(ti));
    return strdup(str);
  }
}
}

namespace {
template <typename T> static void register_integer(const char* name) {
  using namespace internal;
  _embind_register_integer(TypeID<T>::get(), name, sizeof(T), std::numeric_limits<T>::min(),
    std::numeric_limits<T>::max());
}

template <typename T> static void register_bigint(const char* name) {
  using namespace internal;
  _embind_register_bigint(TypeID<T>::get(), name, sizeof(T));
}

template <typename T> static void register_float(const char* name) {
  using namespace internal;
  _embind_register_float(TypeID<T>::get(), name, sizeof(T));
}

// matches typeMapping in embind.js
enum TypedArrayIndex {
  Int8Array,
  Uint8Array,
  Int16Array,
  Uint16Array,
  Int32Array,
  Uint32Array,
  Float32Array,
  Float64Array,
};

template <typename T> constexpr TypedArrayIndex getTypedArrayIndex() {
  static_assert(internal::typeSupportsMemoryView<T>(), "type does not map to a typed array");
  return std::is_floating_point<T>::value
           ? (sizeof(T) == 4 ? Float32Array : Float64Array)
           : (sizeof(T) == 1
                 ? (std::is_signed<T>::value ? Int8Array : Uint8Array)
                 : (sizeof(T) == 2 ? (std::is_signed<T>::value ? Int16Array : Uint16Array)
                                   : (std::is_signed<T>::value ? Int32Array : Uint32Array)));
}

template <typename T> static void register_memory_view(const char* name) {
  using namespace internal;
  _embind_register_memory_view(TypeID<memory_view<T>>::get(), getTypedArrayIndex<T>(), name);
}
} // namespace

extern "C" {
void EMSCRIPTEN_KEEPALIVE __embind_register_native_and_builtin_types() {
  using namespace emscripten::internal;

  _embind_register_void(TypeID<void>::get(), "void");

  _embind_register_bool(TypeID<bool>::get(), "bool", sizeof(bool), true, false);

  register_integer<char>("char");
  register_integer<signed char>("signed char");
  register_integer<unsigned char>("unsigned char");
  register_integer<signed short>("short");
  register_integer<unsigned short>("unsigned short");
  register_integer<signed int>("int");
  register_integer<unsigned int>("unsigned int");
  register_integer<signed long>("long");
  register_integer<unsigned long>("unsigned long");

  register_bigint<int64_t>("int64_t");
  register_bigint<uint64_t>("uint64_t");

  register_float<float>("float");
  register_float<double>("double");

  _embind_register_std_string(TypeID<std::string>::get(), "std::string");
  _embind_register_std_string(
    TypeID<std::basic_string<unsigned char>>::get(), "std::basic_string<unsigned char>");
  _embind_register_std_wstring(TypeID<std::wstring>::get(), sizeof(wchar_t), "std::wstring");
  _embind_register_std_wstring(TypeID<std::u16string>::get(), sizeof(char16_t), "std::u16string");
  _embind_register_std_wstring(TypeID<std::u32string>::get(), sizeof(char32_t), "std::u32string");
  _embind_register_emval(TypeID<val>::get(), "emscripten::val");

  // Some of these types are aliases for each other. Luckily,
  // embind.js's _embind_register_memory_view ignores duplicate
  // registrations rather than asserting, so the first
  // register_memory_view call for a particular type will take
  // precedence.

  register_memory_view<char>("emscripten::memory_view<char>");
  register_memory_view<signed char>("emscripten::memory_view<signed char>");
  register_memory_view<unsigned char>("emscripten::memory_view<unsigned char>");

  register_memory_view<short>("emscripten::memory_view<short>");
  register_memory_view<unsigned short>("emscripten::memory_view<unsigned short>");
  register_memory_view<int>("emscripten::memory_view<int>");
  register_memory_view<unsigned int>("emscripten::memory_view<unsigned int>");
  register_memory_view<long>("emscripten::memory_view<long>");
  register_memory_view<unsigned long>("emscripten::memory_view<unsigned long>");

  register_memory_view<int8_t>("emscripten::memory_view<int8_t>");
  register_memory_view<uint8_t>("emscripten::memory_view<uint8_t>");
  register_memory_view<int16_t>("emscripten::memory_view<int16_t>");
  register_memory_view<uint16_t>("emscripten::memory_view<uint16_t>");
  register_memory_view<int32_t>("emscripten::memory_view<int32_t>");
  register_memory_view<uint32_t>("emscripten::memory_view<uint32_t>");

  register_memory_view<float>("emscripten::memory_view<float>");
  register_memory_view<double>("emscripten::memory_view<double>");
#if __SIZEOF_LONG_DOUBLE__ == __SIZEOF_DOUBLE__
  register_memory_view<long double>("emscripten::memory_view<long double>");
#endif
}
}

EMSCRIPTEN_BINDINGS(native_and_builtin_types) {
  // Normal initialization, executed through a global constructor. This
  // happens on the main thread; pthreads will call it manually.
  __embind_register_native_and_builtin_types();
}
