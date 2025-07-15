
class A {
    
};

struct B {
public:

    virtual void Method()=0;

    virtual A* Get() =0;
    
};

template<typename T>
struct D : B {

    void Method() override {
        T val;
    }

    A * Get() {
        return nullptr;
    }
    
};

struct C;

using CD = D<C>;


struct C : public A {    
};

int main(int argc, char* argv[])
{
    CD cd;

    A * ptr = cd.Get();

	return 0;
}
