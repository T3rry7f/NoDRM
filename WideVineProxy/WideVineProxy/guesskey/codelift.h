#include <stdint.h>
extern "C" {
    const char* guessInput(const char* input);
    const char* getOutput(const char* input);
    const char* getDeoaep(const char* input);
	const char* tryUsingDecoder(const char* input);
    void freeStr(void* str);
}