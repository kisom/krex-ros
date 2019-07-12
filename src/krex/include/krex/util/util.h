#ifndef __KREX_UTIL_UTIL_H
#define __KREX_UTIL_UTIL_H

#include <chrono>
#include <thread>

using namespace std::chrono;

namespace krex {
namespace util {


static inline void
Sleep(uint32_t _for) {
	std::this_thread::sleep_for(milliseconds(_for));
}


} // namespace util
} // namespace krex


#endif // __KREX_UTIL_UTIL_H
