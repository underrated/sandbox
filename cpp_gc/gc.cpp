#include <iostream>
#include <list>
#include <cassert>

using namespace std;

struct config_object {
    unsigned char x[1024];.
};

struct gc_table_entry_base {
	int get_ref_count() {
		return ref_count;
	}

	void inc_ref_count() {
		ref_count++;
	}

	void dec_ref_count() {
		ref_count--;
	}
	
	private:
	int ref_count;

}

template<typename T=int>
struct gc_table_entry:gc_table_entry_base {
	private:
	T* content;
}


struct gc_manager {
    private:

    static gc_manager* m_self;
    long int heap_size_limit;
    long int heap_size;

    list<gc_table_entry_base> table;

    gc_manager(); 

    public: 

    ~gc_manager() {}

    static gc_manager* self();

    void set_limit(long int limit);

    long int get_limit();

    long int get_heap_size();

    void collect();


};


gc_manager* gc_manager::m_self=NULL;

gc_manager::gc_manager() {
    heap_size = 0;
}

gc_manager* gc_manager::self() {
    if(NULL==m_self)
        m_self = new gc_manager();
    return m_self;
}

void gc_manager::set_limit(long int limit) {
    heap_size_limit = limit;
}

long int gc_manager::get_limit() {
    return heap_size_limit;
}

long int gc_manager::get_heap_size() {
    return heap_size;
}

void gc_manager::collect() {

}


struct gc_pointer_base {
}

template <typename T=int>
struct gc_pointer:gc_pointer_base {
    
    gc_pointer() {}
    ~gc_pointer() {}

    T* operator =(T* other) {

    }

    private:
        T* content;
};

int main() {
    gc_manager* gc = gc_manager::self();
    gc->set_limit(5*1024*1024);// 5 MB
    
    gc_pointer<config_object> cobj,other_cobj,x;;

    cobj = new config_object();
    other_cobj = new config_object();
    x = new config_object;
    
    cobj = other_cobj;

    // Test that the allocated heap size is correct
    assert(gc->get_heap_size()==3*1024);

    gc->collect();
    
    // Test that cobj's initial instance has been deallocated
    assert(gc->get_heap_size()==2*1024);

    cout<<"Test passed :-)"<<endl;

    delete gc;
    return 0;
}
