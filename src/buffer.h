struct Buffer
{
    void *buf;
    size_t sz;
    size_t pos;
    Buffer();
    void set_at(size_t ix, double val);
    void push(double val);
};

