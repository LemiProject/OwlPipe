#pragma once

#include <exception>
#include <string>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define OWLPIPEERROR(text) std::exception(std::string(__FILENAME__).append(":").append(std::to_string(__LINE__)).append(": ").append(text).c_str())