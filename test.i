int putchar(int c);
static int foo(int x, int y) {
    return x + y;
}
int main() {
    int a = 5;
    int b = 10;
    int c = foo(a, b);
    putchar(65);
    return c;
}
