#ifndef CrackHillCypherHeader
#define CrackHillCypherHeader
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MOD 26
#define PAIR_SIZE 2

// Function to compute modulo (handles negative numbers)
int mod(int a, int m) {
    return (a % m + m) % m;
}

// Function to compute the determinant of a 2x2 matrix mod MOD
int det2x2(int a, int b, int c, int d) {
    return mod(a * d - b * c, MOD);
}

// Extended Euclidean Algorithm to find modular inverse
int modInverseHillCracker(int a, int m) {
    a = mod(a, m);
    for (int x = 1; x < m; x++) {
        if (mod(a * x, m) == 1) {
            return x;
        }
    }
    return -1; // No inverse
}


// Function to compute inverse of a 2x2 matrix mod MOD
int inverse2x2(int A[2][2], int A_inv[2][2]) {
    int det = det2x2(A[0][0], A[0][1], A[1][0], A[1][1]);
    int det_inv = modInverseHillCracker(det, MOD);
    
    if (det_inv == -1) {
        printf("Matrix is not invertible mod 26 (determinant = %d).\n", det);
        return 0;
    }
    
    A_inv[0][0] = mod(det_inv * A[1][1], MOD);
    A_inv[0][1] = mod(det_inv * -A[0][1], MOD);
    A_inv[1][0] = mod(det_inv * -A[1][0], MOD);
    A_inv[1][1] = mod(det_inv * A[0][0], MOD);
    
    return 1;
}

// Function to multiply two 2x2 matrices mod MOD
void multiply2x2(int A[2][2], int B[2][2], int result[2][2]) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            result[i][j] = 0;
            for (int k = 0; k < 2; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
            result[i][j] = mod(result[i][j], MOD);
        }
    }
}

// Convert letter to number (A=0, B=1, ..., Z=25)
int letterToNumber(char ch) {
    ch = toupper(ch);
    if (ch < 'A' || ch > 'Z') {
        return -1; // Invalid
    }
    return ch - 'A';
}

// Get valid 2-letter pair from user
void getPair(const char* prompt, int pair[2]) {
    char input[3];
    int valid = 0;
    
    while (!valid) {
        printf("%s (2 letters): ", prompt);
        scanf("%2s", input);
        getchar(); // clear newline
        
        if (strlen(input) != 2) {
            printf("Please enter exactly 2 letters.\n");
            continue;
        }
        
        int n1 = letterToNumber(input[0]);
        int n2 = letterToNumber(input[1]);
        
        if (n1 == -1 || n2 == -1) {
            printf("Invalid characters. Use A-Z or a-z.\n");
            continue;
        }
        
        pair[0] = n1;
        pair[1] = n2;
        valid = 1;
    }
}

int crackHillCypher() {
    int plain[2][2];  // Plaintext matrix
    int cipher[2][2]; // Ciphertext matrix
    int plain_inv[2][2];
    int K[2][2];
    
    printf("=== Hill Cipher Key Recovery Attack ===\n");
    printf("We need 2 known plaintext-ciphertext pairs (2 letters each).\n\n");
    
    // Get first pair
    printf("Pair 1:\n");
    getPair("Plaintext", plain[0]);
    getPair("Ciphertext", cipher[0]);
    
    // Get second pair
    printf("\nPair 2:\n");
    getPair("Plaintext", plain[1]);
    getPair("Ciphertext", cipher[1]);
    
    printf("\nPlaintext matrix P:\n");
    printf("[%2d %2d]\n", plain[0][0], plain[0][1]);
    printf("[%2d %2d]\n", plain[1][0], plain[1][1]);
    
    printf("\nCiphertext matrix C:\n");
    printf("[%2d %2d]\n", cipher[0][0], cipher[0][1]);
    printf("[%2d %2d]\n", cipher[1][0], cipher[1][1]);
    
    // Step 1: Compute inverse of plaintext matrix
    if (!inverse2x2(plain, plain_inv)) {
        printf("Cannot proceed: plaintext matrix is not invertible mod 26.\n");
        return 1;
    }
    
    printf("\nInverse of plaintext matrix P^-1:\n");
    printf("[%2d %2d]\n", plain_inv[0][0], plain_inv[0][1]);
    printf("[%2d %2d]\n", plain_inv[1][0], plain_inv[1][1]);
    
    // Step 2: Compute K = P_inv * C mod 26
    multiply2x2(plain_inv, cipher, K);
    
    printf("\nRecovered encryption matrix K:\n");
    printf("[%2d %2d]\n", K[0][0], K[0][1]);
    printf("[%2d %2d]\n", K[1][0], K[1][1]);
    
    printf("\nIn letter form:\n");
    printf("%c%c  %c%c\n", 
           K[0][0] + 'A', K[0][1] + 'A', 
           K[1][0] + 'A', K[1][1] + 'A');
    
    return 0;
}

#endif
