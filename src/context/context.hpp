#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <stdint.h>
#include <string>

namespace Context {

bool SetupContext (int scale);
void DestroyContext ();
void HandleEvents ();

void RenderDebugText ();
void RenderDisplay ();

void SetDisplayBuffer (uint8_t* buffer);
void CopyDisplayBuffer (uint8_t* buffer);

void SetTitle (std::string title);
bool IsOpen ();

bool ShouldHalt ();
bool ShouldStep ();

void SetDebugText (std::string text);

float GetSpeedInput ();

};

#endif
