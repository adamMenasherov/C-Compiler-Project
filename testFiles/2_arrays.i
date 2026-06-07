int main(void) {
    int arr[3][2] = {{2, 4}, {6, 8}, {10, 12}};
    int first = arr[0][0];
    int last = arr[1][1];
    arr[1][0] = first + last;
    return arr[1][0];
}
