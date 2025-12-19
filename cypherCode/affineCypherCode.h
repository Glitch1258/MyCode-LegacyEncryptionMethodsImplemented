#ifndef  AFFFINE
#define  AFFFINE

// ========== AFFINE CIPHER FUNCTIONS ==========
// Function to get encryption keys from user

// Function to calculate greatest common divisor
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int getKeyFromUserAffineCypher(int *a, int *b) {
    printf("\nEnter key 'a' (must be coprime with 26): ");
    scanf("%d", a);
    getchar();  // Consume newline
    
    printf("Enter key 'b' (0-25): ");
    scanf("%d", b);
    getchar();  // Consume newline
    
    // Validate 'b'
    if (*b < 0 || *b > 25) {
        printf("Error: 'b' must be between 0 and 25.\n");
        return 0;
    }
    
    // Validate 'a' - must be coprime with 26
    if (gcd(*a, 26) != 1) {
        printf("Error: 'a' must be coprime with 26.\n");
        printf("Valid values: 1,3,5,7,9,11,15,17,19,21,23,25\n");
        return 0;
    }
    
    return 1;
}

// Function to calculate modular multiplicative inverse
// Returns a^(-1) mod m, or -1 if it doesn't exist
int modInverse(int a, int m) {
    a = a % m;
    for (int x = 1; x < m; x++) {
        if ((a * x) % m == 1) {
            return x;
        }
    }
    return -1;  // Inverse doesn't exist
}

// Function to encrypt text using Affine cipher
// Encryption formula: E(x) = (ax + b) mod 26
void encryptAffine(char *text, int a, int b, char *result) {
    int i = 0;
    char ch;
    
    while (text[i] != '\0') {
        ch = text[i];
        
        if (isalpha(ch)) {
            if (isupper(ch)) {
                // Convert to 0-25, apply affine transform, convert back
                result[i] = ((a * (ch - 'A') + b) % 26) + 'A';
            } else {
                // Convert to 0-25, apply affine transform, convert back
                result[i] = ((a * (ch - 'a') + b) % 26) + 'a';
            }
        } else {
            result[i] = ch;  // Non-alphabetic characters remain unchanged
        }
        i++;
    }
    result[i] = '\0';
}

// Function to decrypt text using Affine cipher
// Decryption formula: D(x) = a^(-1)(x - b) mod 26
void decryptAffine(char *text, int a, int b, char *result) {
    int i = 0;
    char ch;
    int a_inv = modInverse(a, 26);
    
    if (a_inv == -1) {
        printf("Error: Cannot decrypt - 'a' has no modular inverse!\n");
        result[0] = '\0';
        return;
    }
    
    while (text[i] != '\0') {
        ch = text[i];
        
        if (isalpha(ch)) {
            if (isupper(ch)) {
                // Convert to 0-25, apply inverse affine transform, convert back
                int x = ch - 'A';
                // Handle negative values properly
                int temp = (x - b);
                while (temp < 0) temp += 26;
                result[i] = ((a_inv * temp) % 26) + 'A';
            } else {
                // Convert to 0-25, apply inverse affine transform, convert back
                int x = ch - 'a';
                // Handle negative values properly
                int temp = (x - b);
                while (temp < 0) temp += 26;
                result[i] = ((a_inv * temp) % 26) + 'a';
            }
        } else {
            result[i] = ch;  // Non-alphabetic characters remain unchanged
        }
        i++;
    }
    result[i] = '\0';
}

// Complete encryption function with file handling for Affine cipher
void encryptAffineWithFileHandling() {
    FILE *inputFile, *outputFile, *keyFile;
    char inputPath[MAX_PATH_LENGTH], outputPath[MAX_PATH_LENGTH];
    char keyFileName[MAX_PATH_LENGTH * 2];  // Allow longer filename
    char plaintext[MAX_TEXT_LENGTH], ciphertext[MAX_TEXT_LENGTH];
    size_t bytesRead;
    int a, b;
    
    printf("\n=== Affine Cipher Encryption ===\n");
    
    // Get input file path from user
    printf("Enter the path of the input text file to encrypt: ");
    fgets(inputPath, sizeof(inputPath), stdin);
    inputPath[strcspn(inputPath, "\n")] = '\0';
    
    // Open input file
    inputFile = fopen(inputPath, "r");
    if (inputFile == NULL) {
        printf("Error: Could not open input file: %s\n", inputPath);
        return;
    }
    
    // Read entire file content
    bytesRead = fread(plaintext, 1, MAX_TEXT_LENGTH - 1, inputFile);
    if (bytesRead == 0 && ferror(inputFile)) {
        printf("Error: Could not read from input file\n");
        fclose(inputFile);
        return;
    }
    plaintext[bytesRead] = '\0';
    
    // Remove trailing newline if present
    size_t len = strlen(plaintext);
    if (len > 0 && plaintext[len - 1] == '\n') {
        plaintext[len - 1] = '\0';
    }
    
    fclose(inputFile);
    
    printf("\nText read from file (%zu bytes):\n%s\n", bytesRead, plaintext);
    
    // Get keys from user
    printf("\n--- Affine Cipher Keys ---\n");
    while (!getKeyFromUserAffineCypher(&a, &b)) {
        printf("Please try again.\n");
    }
    
    // Get output file path
    printf("\nEnter the path for the output encrypted file: ");
    fgets(outputPath, sizeof(outputPath), stdin);
    outputPath[strcspn(outputPath, "\n")] = '\0';
    
    // Create key file name
    createKeyFileName(outputPath, keyFileName);
    
    // Perform encryption
    printf("\n--- Encryption Results ---\n");
    printf("Using keys: a=%d, b=%d\n", a, b);
    encryptAffine(plaintext, a, b, ciphertext);
    printf("Plaintext:\n%s\n", plaintext);
    printf("Ciphertext:\n%s\n", ciphertext);
    
    // Open output file and write results
    outputFile = fopen(outputPath, "w");
    if (outputFile == NULL) {
        printf("Error: Could not create output file: %s\n", outputPath);
        return;
    }
    
    fprintf(outputFile, "%s\n", ciphertext);
    fclose(outputFile);
    
    // Create and write to key file
    keyFile = fopen(keyFileName, "w");
    if (keyFile == NULL) {
        printf("Warning: Could not create key file: %s\n", keyFileName);
    } else {
        fprintf(keyFile, "=== Affine Cipher Key ===\n");
        fprintf(keyFile, "Key a: %d\n", a);
        fprintf(keyFile, "Key b: %d\n", b);
        fprintf(keyFile, "Encrypted file: %s\n", outputPath);
        fprintf(keyFile, "Original file: %s\n", inputPath);
        fclose(keyFile);
        printf("Key file created: %s\n", keyFileName);
    }
    
    printf("\nEncrypted text has been written to: %s\n", outputPath);
    printf("=== Encryption Complete ===\n");
}

// Complete decryption function with file handling for Affine cipher
void decryptAffineWithFileHandling() {
    FILE *inputFile, *outputFile;
    char inputPath[MAX_PATH_LENGTH], outputPath[MAX_PATH_LENGTH];
    char ciphertext[MAX_TEXT_LENGTH], decryptedtext[MAX_TEXT_LENGTH];
    size_t bytesRead;
    int a, b;
    
    printf("\n=== Affine Cipher Decryption ===\n");
    
    // Get input file path from user
    printf("Enter the path of the input text file to decrypt: ");
    fgets(inputPath, sizeof(inputPath), stdin);
    inputPath[strcspn(inputPath, "\n")] = '\0';
    
    // Open input file
    inputFile = fopen(inputPath, "r");
    if (inputFile == NULL) {
        printf("Error: Could not open input file: %s\n", inputPath);
        return;
    }
    
    // Read entire file content
    bytesRead = fread(ciphertext, 1, MAX_TEXT_LENGTH - 1, inputFile);
    if (bytesRead == 0 && ferror(inputFile)) {
        printf("Error: Could not read from input file\n");
        fclose(inputFile);
        return;
    }
    ciphertext[bytesRead] = '\0';
    
    // Remove trailing newline if present
    size_t len = strlen(ciphertext);
    if (len > 0 && ciphertext[len - 1] == '\n') {
        ciphertext[len - 1] = '\0';
    }
    
    fclose(inputFile);
    
    printf("\nText read from file (%zu bytes):\n%s\n", bytesRead, ciphertext);
    
    // Get keys from user
    printf("\n--- Affine Cipher Keys ---\n");
    while (!getKeyFromUserAffineCypher(&a, &b)) {
        printf("Please try again.\n");
    }
    
    // Get output file path
    printf("\nEnter the path for the output decrypted file: ");
    fgets(outputPath, sizeof(outputPath), stdin);
    outputPath[strcspn(outputPath, "\n")] = '\0';
    
    // Perform decryption
    printf("\n--- Decryption Results ---\n");
    printf("Using keys: a=%d, b=%d\n", a, b);
    decryptAffine(ciphertext, a, b, decryptedtext);
    printf("Ciphertext:\n%s\n", ciphertext);
    printf("Plaintext:\n%s\n", decryptedtext);
    
    // Open output file and write results
    outputFile = fopen(outputPath, "w");
    if (outputFile == NULL) {
        printf("Error: Could not create output file: %s\n", outputPath);
        return;
    }
    
    fprintf(outputFile, "%s\n", decryptedtext);
    fclose(outputFile);
    
    printf("\nDecrypted text has been written to: %s\n", outputPath);
    printf("=== Decryption Complete ===\n");
}
//</affine encryption>

#endif
