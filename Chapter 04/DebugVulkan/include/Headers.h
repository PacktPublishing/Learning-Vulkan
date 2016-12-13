/*
* Learning Vulkan - ISBN: 9781786469809
*
* Author: Parminder Singh, parminder.vulkan@gmail.com
* Linkedin: https://www.linkedin.com/in/parmindersingh18
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/

#pragma once

/*********** COMPILER SPECIFIC PREPROCESSORS ***********/
#ifdef _WIN32
#pragma comment(linker, "/subsystem:console")
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define APP_NAME_STR_LEN 80
#define _CRT_SECURE_NO_WARNINGS
#else  // _WIN32
#define VK_USE_PLATFORM_XCB_KHR
#include <unistd.h>
#endif // _WIN32

/*********** C/C++ HEADER FILES ***********/
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <assert.h>

// Header files for Singleton
#include <memory>
#include <mutex>

/*********** VULKAN HEADER FILES ***********/
#include <vulkan/vulkan.h>