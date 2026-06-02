static int foo(int a, int b, double **c) {
    return *a + (double)b + (int)**c;
}
int main() {
    int (foo)(int * a, int b, double **c);
}
