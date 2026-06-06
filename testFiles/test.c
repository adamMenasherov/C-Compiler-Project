int bar(int x) {
    static int y = x + 1;
    return y;
}

int main(void) {
    return bar(42);
}
