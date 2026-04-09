int foo(int a, int b);
int main(void) {
    int c = 5;
    int d = 10;
    int foo(int c, int d);
    return foo(c, d);
}
int foo(int a, int b) {
    return a * b;
}
