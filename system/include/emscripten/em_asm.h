#ifndef __em_asm_h__
#define __em_asm_h__

#ifndef __cplusplus
void emscripten_asm_const();
int emscripten_asm_const_int();
double emscripten_asm_const_double();
#else
#ifdef __asmjs
extern "C" {
    void emscripten_asm_const(const char* code);
    int emscripten_asm_const_int(const char* code, ...);
    double emscripten_asm_const_double(const char* code, ...);
}
#else
template <typename... Args> void emscripten_asm_const(const char* code, Args...);
template <typename... Args> int emscripten_asm_const_int(const char* code, Args...);
template <typename... Args> double emscripten_asm_const_double(const char* code, Args...);
#endif
#endif

void emscripten_asm_const(const char* code);

#define EM_ASM(code) emscripten_asm_const(#code)
#define EM_ASM_(code, ...) emscripten_asm_const_int(#code, __VA_ARGS__)
#define EM_ASM_ARGS(code, ...) emscripten_asm_const_int(#code, __VA_ARGS__)
#define EM_ASM_INT(code, ...) emscripten_asm_const_int(#code, __VA_ARGS__)
#define EM_ASM_DOUBLE(code, ...) emscripten_asm_const_double(#code, __VA_ARGS__)
#define EM_ASM_INT_V(code) emscripten_asm_const_int(#code)
#define EM_ASM_DOUBLE_V(code) emscripten_asm_const_double(#code)

#endif // __em_asm_h__
