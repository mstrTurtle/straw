// 绑定func的第n个参数为value（ with a little help from currying 🤣 )
auto bind(int n, auto func, auto value) {
    if (n == 0)
        return func(value);
    else
        return [](auto arg) {return bind(n - 1, func(arg), value); };
}