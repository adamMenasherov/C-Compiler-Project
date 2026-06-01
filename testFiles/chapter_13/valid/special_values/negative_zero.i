double copysign(double x, double y);
double zero = 0.0;
int main(void) {
    double negative_zero = -zero;
    if (negative_zero != 0)
        return 1;
    if ( 1/negative_zero != -10e308 )
        return 2;
    if ( (-10)/negative_zero != 10e308)
        return 3;
    int fail = 0;
    negative_zero && (fail = 1);
    if (fail)
        return 4;
    if (negative_zero) {
        return 5;
    }
    if (zero != -0.0) {
        return 6;
    }
    double negated = copysign(4.0, -0.0);
    double positive = copysign(-5.0, 0.0);
    if (negated != -4.0) {
        return 7;
    }
    if (positive != 5.0) {
        return 8;
    }
    return 0;
}
