struct func_stub
{
    void *fn;
    char buf[5];
};

void stub_set(struct func_stub *stub, void *fn, void *fn_stub);
void stub_reset(struct func_stub *stub);
