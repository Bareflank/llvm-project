
struct X {
private:
    bool b;
    int arr[3];

public:
    bool operator&&(const X &x)
    {
        return this->b && x.b;
    }

    bool operator||(const X &x)
    {
        return this->b || x.b;
    }

    X& operator,(X &x)
    {
        return x;
    }

    int operator[](int i)
    {
        if (i < 3) {
            return arr[i];
        } else {
            return 0;
        }
    }

    X* operator&()
    // CHECK-MESSAGES: :[[@LINE-1]]:5: warning: overloaded address-of operator is forbidden [bsl-op-forbidden-overload]
    {
        return this;
    }
};

int foo()
{
    class X x0;
    class X x1;

    auto a = x0 && x1;
    // CHECK-MESSAGES: :[[@LINE-1]]:17: warning: overloaded operator&& is forbidden [bsl-op-forbidden-overload]
    auto b = x0 || x1;
    // CHECK-MESSAGES: :[[@LINE-1]]:17: warning: overloaded operator|| is forbidden [bsl-op-forbidden-overload]
    X c = (x0, x1);
    // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: overloaded operator, is forbidden [bsl-op-forbidden-overload]

    int i = c[1];
    // CHECK-MESSAGES: :[[@LINE-1]]:16: warning: overloaded operator[] is forbidden [bsl-op-forbidden-overload]

    return 0;
}

template<typename T>
class Y
{
    T t;

public:
    T* ptr() { return &t; }
};

template<typename T>
T* operator&(T t)
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: overloaded address-of operator is forbidden [bsl-op-forbidden-overload]
{
    return t.ptr();
}

int b;

int *bar()
{
    return &b;
}
