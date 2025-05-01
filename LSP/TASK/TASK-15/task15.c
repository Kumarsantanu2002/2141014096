#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int reverse(const char *input, char *output) {
    int len = strlen(input);
    for (int i = 0; i < len; i++) {
        output[i] = input[len - i - 1];
    }
    output[len] = '\0';
    return 0;
}
int main() {
    char *input, *output;
    size_t size = 256;

    input = (char *)malloc(size);
    output = (char *)malloc(size);

    if (input == NULL || output == NULL) {
        perror("Memory allocation failed");
        return 1;
    }

    printf("Enter a string: ");
    if (fgets(input, size, stdin) == NULL) {
        perror("Input error");
        free(input);
        free(output);
        return 1;
    }
    input[strcspn(input, "\n")] = '\0';

    if (reverse(input, output) == 0) {
        printf("Reversed string: %s\n", output);
    } else {
        printf("Error reversing string.\n");
    }

    free(input);
    free(output);

    return 0;
}


