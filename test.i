int foo(int a, int b,);
int main(void) {
    int x = 5;
    int y = 10;
    {
        if (x < y) {
            x = x + 1;
        } else {
            y = y - 1;
        }
    }
    return foo(x + y, x - y);
}
