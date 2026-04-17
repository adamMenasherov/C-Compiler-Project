static int a;
static int foo() {
    return 42;
}
extern int foo();
int main() {
    int static foo();
    return 1;
}
