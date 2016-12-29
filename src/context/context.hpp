#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <stdint.h>
#include <string>

namespace Context {

bool SetupContext (int scale);
void DestroyContext ();
void HandleEvents ();
void RenderDisplay ();
void SetTitle (std::string title);
bool IsOpen ();
void SetDisplayBuffer (uint8_t* buffer);
void CopyDisplayBuffer (uint8_t* buffer);

};

#endif
