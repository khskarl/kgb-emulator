#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <stdint.h>

namespace Context {
void SetKeymap (uint8_t* argKeypad);
bool SetupContext ();
void DestroyContext ();
void UpdateKeypad ();
void HandleEvents ();
void Draw (uint8_t displayBuffer[64 * 32]);
void Sleep (float ms);
bool ShouldQuit ();
}

#endif
