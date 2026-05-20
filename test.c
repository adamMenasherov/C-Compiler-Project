extern int a = 10;
extern int a = 3;
static int b = 10;

static int foo() {
    return 42;
}

int main() {
    int a = 5;

    switch(a) {
       case 1:
            return foo() * a;            
            break;
        case 6:
            return foo() + a;
            break;
        default:
            return foo() * 2 + a;
    }
}