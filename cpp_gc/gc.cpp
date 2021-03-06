#include <iostream>
#include <list>
#include <cassert>

using namespace std;


struct gc_table_entry_base {

	gc_table_entry_base() { }
	~gc_table_entry_base() { }

	virtual void free() { } 
	virtual void print() {
		cout<<"ref_count="<<ref_count<<"size="<<size<<endl;
	}

	int get_ref_count() {
		return ref_count;
	}

	void inc_ref_count() {
		ref_count++;
	}

	void dec_ref_count() {
		ref_count--;
	}

	void set_size(size_t s) {
		size=s;
	}

	size_t get_size() {
		return size;
	}
	
	protected:
	int ref_count;
	size_t size;

};

template<typename T=int>
struct gc_table_entry:gc_table_entry_base {

	gc_table_entry(T* c) {
		ref_count=0;
		size = sizeof(T);
		content = c;
	}

	~gc_table_entry() {}

	virtual void print() {
		cout<<"location="<<content<<" ref_count="<<ref_count<<" size="<<size<<endl;
	}
	
	virtual void free() {
		delete content;
	}
	private:
	T* content;
};


struct gc_manager {
	private:
	
	static gc_manager* m_self;
	long int heap_size_limit;
	long int heap_size;
	
	list<gc_table_entry_base*> table;
	
	gc_manager(); 
	
	public: 
	
	~gc_manager() {}
	
	static gc_manager* self();
	
	void set_limit(long int limit);
	
	long int get_limit();
	
	long int get_heap_size();
	
	void collect();
	
	void add_table_entry(gc_table_entry_base*,list<gc_table_entry_base*>::iterator&);

	void remove_table_entry(list<gc_table_entry_base*>::iterator);

	void print_table();

	void clear_heap();
};


gc_manager* gc_manager::m_self=NULL;

gc_manager::gc_manager():table() {
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
	list<gc_table_entry_base*>::iterator it;
	it = table.begin();
	while(it!=table.end()) {
		if((*it)->get_ref_count()>0)
			++it;
		else {
			heap_size-=(*it)->get_size();
			(*it)->free();
			delete (*it);
			it=table.erase(it);
		}
	}
}

void gc_manager::clear_heap() {
	list<gc_table_entry_base*>::iterator it;
	it = table.begin();
	while(it!=table.end()) {
		heap_size-=(*it)->get_size();
		(*it)->free();
		delete (*it);
		it=table.erase(it);
	}
}

void gc_manager::add_table_entry(gc_table_entry_base* entry, list<gc_table_entry_base*>::iterator& it) {
	table.push_back(entry);
	it=--table.end();
	heap_size+=(*it)->get_size();
}

void gc_manager::remove_table_entry(list<gc_table_entry_base*>::iterator it) {

}

void gc_manager::print_table() {
	list<gc_table_entry_base*>::iterator it;
	cout<<"Heap contents:"<<endl;
	cout<<"==============="<<endl;
	for(it=table.begin();it!=table.end();++it)
		(*it)->print();
	cout<<"==============="<<endl<<endl;
}


struct gc_pointer_base {
	gc_pointer_base() {}
	virtual void* get_content(){ };
	virtual size_t get_size() { return 0; }
	virtual void set_size(size_t s) { }
	virtual list<gc_table_entry_base*>::iterator get_table_entry() { }
	virtual int get_ref_count() {}
};

template <typename T=int>
struct gc_pointer:gc_pointer_base {
    
    gc_pointer() {
	    content = NULL;
	    gcm = gc_manager::self();
	    size = sizeof(T);
    }
    ~gc_pointer(){
	    (*table_entry)->dec_ref_count();
    }

    // NOTE : when other is of a type derived from T
    // it must be casted to T*
    void assign_raw_pointer(T* other, bool new_entry) {
	if(content!=NULL) {
	        (*table_entry)->dec_ref_count();
		if(other=NULL) {
			content = NULL;
			return;
		}
	} 
	if(new_entry && other!=NULL) {
	        // Create new table entry
		gc_table_entry<T>* entry = new gc_table_entry<T>(other);
		entry->inc_ref_count();
		gcm->add_table_entry(entry, table_entry);
	}
	content = other;
    }

    void operator =(T* other) {
	    assign_raw_pointer(other,true);
    }


    void operator =(gc_pointer<T>& other) {
	assign_raw_pointer(other.get_typed_content(),false);
	table_entry = other.get_table_entry();
	(*table_entry)->inc_ref_count();
    }

    void operator =(gc_pointer_base& other) {
	void* ptr = other.get_content();
	T* other_ptr = (T*)ptr;
	assign_raw_pointer(other_ptr,false);
	size = other.get_size();
	table_entry = other.get_table_entry();
	(*table_entry)->inc_ref_count();
	(*table_entry)->set_size(size);
    }

    T* operator ->() {
	    return content;
    }

    bool operator ==(T* other) {
	    return other==content;
    }

    bool operator ==(gc_pointer<T> other) {
	    return other.get_typed_content()==content;
    }

    bool operator ==(gc_pointer_base other) {
	    return other.get_content()==(void*)content;
    }

    virtual size_t get_size() { return size; }
    virtual void set_size(size_t s) { size=s; }

    virtual void* get_content() {
	    return (void*)content;
    }

    T* get_typed_content() {
	    return content;
    }

    virtual list<gc_table_entry_base*>::iterator get_table_entry(){
	    return table_entry;
    }
    
    virtual int get_ref_count() {
	    return (*table_entry)->get_ref_count();
    }

    private:
    	list<gc_table_entry_base*>::iterator table_entry;
        T* content;
	gc_manager* gcm;
	size_t size;
};


struct config_object {
    unsigned char x[1024];
};

void testAllocSizes() {

    gc_manager* gc = gc_manager::self();
    gc->set_limit(5*1024*1024);// 5 MB
    
    gc_pointer<config_object> cobj,other_cobj,x;;

    cout<<"*Creating object*"<<endl;
    cobj = new config_object();
    gc->print_table();

    cout<<"*Creating object*"<<endl;
    other_cobj = new config_object();
    gc->print_table();

    cout<<"*Creating object*"<<endl;
    x = new config_object();
    gc->print_table();
    
    cout<<"*Overwriting object with other object*"<<endl;
    cobj = other_cobj;
    other_cobj= x;
    gc->print_table();

    // Test that the allocated heap size is correct
    assert(gc->get_heap_size()==3*1024);

    cout<<"*Collecting garbage*"<<endl;
    gc->collect();
    gc->print_table();
    
    // Test that cobj's initial instance has been deallocated
    assert(gc->get_heap_size()==2*1024);

    cout<<"*Deleting all allocated objects*"<<endl;
    gc->clear_heap();
    gc->print_table();

    // Check that the heap got cleared
    assert(gc->get_heap_size()==0);


}

// Test that the ref_counts update correctly
struct wheel {
	int diameter;
};
struct gear {
	int nof_steps;
};
struct car {
	gc_pointer<wheel> w;
	gc_pointer<gear> g;

	car():w(),g() {
		w = new wheel();
		g = new gear();
	}
};

void testRefCount() {
	gc_manager* gc = gc_manager::self();
	gc_pointer<car> my_car,other_car;
	gc_pointer<wheel> my_wheel;
	
	cout<<"==Running test testRefCount=="<<endl;

	cout<<"Allocating some objects"<<endl;
	my_car = new car();
	other_car = new car();
	my_wheel = my_car->w;

	cout<<"my_car : ";(*my_car.get_table_entry())->print();
	cout<<"my_car->w : ";(*my_car->w.get_table_entry())->print();
	cout<<"my_car->g : ";(*my_car->g.get_table_entry())->print();
	cout<<"other_car : ";(*other_car.get_table_entry())->print();
	cout<<"other_car->w : ";(*other_car->w.get_table_entry())->print();
	cout<<"other_car->g : ";(*other_car->g.get_table_entry())->print();
	cout<<"my_wheel : ";(*my_wheel.get_table_entry())->print();

	gc->print_table();
	
	cout << "Overwriting an object" << endl;
	my_car = other_car;
	assert( other_car.get_ref_count() == 2  );
	assert( my_wheel.get_ref_count()  == 2  );

	gc->print_table();

	cout << "Collecting garbage" << endl;
	gc->collect();
	assert(my_wheel.get_ref_count() == 1);

	gc->print_table();

}

// Assign object of a derivative class
// Check that size updates correctly
// Check that virtual methods are called correctly
void testInheritance() {
}

// Assign a null raw pointer
void testRawNullAssign() {
}

// Assign a null smart pointer
void testNullSPAssign() {
}

// Create collections of smart pointers and check that they are deallocated correctly
void testSPointerCollection() {
}


int main() {
	testAllocSizes();
	testRefCount();
	testInheritance();
	testRawNullAssign();
	testNullSPAssign();
	testSPointerCollection();
	
    	cout<<"All tests passed :-)"<<endl;

	delete gc_manager::self();

	return 0;
}

