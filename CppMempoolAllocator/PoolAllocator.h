#ifndef __POOLALLOCATOR__

#include <cstdlib>
#include <iostream>
#include <new>

template <class T>
class PoolAllocator {
    private:

    public:
        // Useful typedefs
        typedef T                 value_type;
        typedef value_type*       pointer;
        typedef const value_type* const_pointer;
        typedef value_type&       reference;
        typedef const value_type& const_reference;
        typedef std::size_t       size_type;
        typedef std::ptrdiff_t    difference_type;

        // Just another spelling for operator&
        pointer address(reference x) const { return &x; }
        const_pointer address(const_reference x) const { 
            return &x; 
        }

        // Allocate memory in the pool
        pointer allocate(size_type n, const_pointer = 0) {
            void* p;
            p = std::malloc(n * sizeof(T));
            // TODO : delete the line above and add allocation logic below
            if (!p)
                throw std::bad_alloc();
            return static_cast<pointer>(p);
        }

        // Deallocate memory in the pool
        void deallocate(pointer p, size_type) {
            std::free(p);
            // TODO : delete the line above and add deallocation logic below
        }

        // Returns the largest number of allocated objects that might succeed
        size_type max_size() const { 
            return static_cast<size_type>(-1) / sizeof(value_type);
        }

        // Performs placement new (places x at address p)
        void construct(pointer p, const value_type& x) { 
            new(p) value_type(x); 
        }
        // Invokes the destructor
        void destroy(pointer p) { p->~value_type(); }

        // Constructors
        PoolAllocator() {}
        PoolAllocator(const PoolAllocator&) {}
        template <class U> 
            PoolAllocator(const PoolAllocator<U>&) {}

        template <class U> 
            struct rebind { typedef PoolAllocator<U> other; };

        // Destructor
        ~PoolAllocator();

    private:
        void operator=(const PoolAllocator&);
};

template <class T>
inline bool operator==(const PoolAllocator<T>&, 
        const PoolAllocator<T>&) {
    return true;
}

template <class T>
inline bool operator!=(const PoolAllocator<T>&, 
        const PoolAllocator<T>&) {
    return false;
}

// Specialize it for void
template<> class PoolAllocator<void>
{
    typedef void        value_type;
    typedef void*       pointer;
    typedef const void* const_pointer;

    template <class U> 
        struct rebind { typedef PoolAllocator<U> other;};
};

#endif // __POOLALLOCATOR__

