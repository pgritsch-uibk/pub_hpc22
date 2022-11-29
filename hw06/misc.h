#ifndef HPC22_MISC_H
#define HPC22_MISC_H

#include <sys/resource.h>
#include <exception>

void increaseStackSize() {
	const rlim_t kStackSize = 1024 * 1024 * 1024; // min stack size = 16 MB
	struct rlimit rl {};
	int result;

	result = getrlimit(RLIMIT_STACK, &rl);
	if(result == 0) {
		if(rl.rlim_cur < kStackSize) {
			rl.rlim_cur = kStackSize;
			result = setrlimit(RLIMIT_STACK, &rl);
			if(result != 0) {
				throw std::bad_alloc();
			}
		}
	}
}

#endif // HPC22_MISC_H
