#ifndef COMMON_H
#define COMMON_H

#include <chrono>

using Seconds = std::chrono::duration< double, std::ratio<1, 1> >;
using Milliseconds = std::chrono::milliseconds;
using Microseconds = std::chrono::microseconds;
#endif // COMMON_H
