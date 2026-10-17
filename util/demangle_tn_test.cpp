// Checks that __cxa_demangle in util/decode.c accepts Tn <type> <value> template arguments (a non-type template
// argument whose type is dependent, such as an enable_if result). Build with the decoder:
//   clang++ -x c++ -std=c++17 -c -I src -I lib_src -I util util/decode.c -o decode.o
//   clang++ -std=c++17 util/demangle_tn_test.cpp decode.o -o demangle_tn_test && ./demangle_tn_test
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" char *__cxa_demangle(const char *, char *, size_t *, int *);

static int check(const char *mangled, const char *expect) {
  int status = -1;
  char *d = __cxa_demangle(mangled, nullptr, nullptr, &status);
  bool ok = status == 0 && d && strstr(d, expect);
  std::printf("%s %s -> %s\n", ok ? "ok  " : "FAIL", mangled, d ? d : "(null)");
  std::free(d);
  return ok ? 0 : 1;
}

int main() {
  int bad = 0;
  // A template argument list with a Tn <type> argument, from a function template with an enable_if-typed parameter.
  bad += check("_ZN2ns3dspIiTnNS_9enable_ifIX2spIT_EEiE4typeELi0EEEvPS2_", "dsp<int, (int)0>");
  // The same shape as libc++'s __sort_dispatch instance.
  bad += check("_ZNSt3__115__sort_dispatchB8ne180100INS_17_ClassicAlgPolicyEiTnNS_9enable_ifIXsr32__sort_is_specialized_"
               "in_libraryIT0_EE5valueEiE4typeELi0EEEvPS3_S6_RNS_6__lessIvvEE",
               "__sort_dispatch");
  // Names without Tn must keep decoding as before.
  bad += check("_ZN2ns3litIiLi0EEEvPT_", "lit<int, (int)0>");
  bad += check("_ZN2ns4nospIiEEvPT_", "nosp<int>");
  std::printf("%s\n", bad ? "demangle_tn_test: FAILED" : "demangle_tn_test: all passed");
  return bad ? 1 : 0;
}
