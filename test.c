static int foo = 5;

int main() {
    int a = 5;

    switch (a) {
        case 5: 
            return a + foo;
            break;
        default:
            return foo;
    }
}