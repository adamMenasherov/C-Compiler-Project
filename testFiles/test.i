static int foo(int b) {
    static int a = 5;
    a++;
    return a + b;
}
int main() {
    static int a = 4;
    return foo(a);
}
