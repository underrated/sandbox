#ifndef __MEMPOOL__

#include <cstdlib>
#include <iostream>
#include <new>

class MemPool {
	public:
		MemPool();
		~MemPool();
		
		void* allocate(size_t s);
		int deallocate(void* ptr);

	private:
		// List of allocated blocks
		// List of unallocated blocks
};

#endif // __MEMPOOL__

