# EinKleinFlug
Simple flight simulator... with not-so-simple space ;)
![A screenshot](screenshot.png)

The space is a Klein bottle. It is non-orientable, i.e. there is no global notion of left vs right, or clockwise vs. counterclockwise. For example, the right wing is always red, and while if you fly by particular paths you may find a ship whose *left* wing looks red, from its perspective it’s its *right* wing. That’s somewhat similar to looking in a mirror.

## Controls
 - Move: arrows
 - Shot: space
 - Stabilize (stop rotation): S
 - Self-destruct: X
 
## Dependencies
 - [CMake](https://cmake.org/)
 - [SDL 2](https://www.libsdl.org/)
 - [FreeGLUT](http://freeglut.sourceforge.net/)

### Win32
This program can be cross-compiled to Win32 using MinGW.

## Copyright notices
**EinKleinFlug** - a simple non-orientable-space-flight simulator  
Copyright (C) 2016 Lobachevsky Vitaly <silverunicorn2011@yandex.ru>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer. 

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.

3. The name of the author may not be used to
   endorse or promote products derived from this software without
   specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
