//#include "definations.h"
#ifndef CEASER
#define CEASER
#define MAX_TEXT_LENGTH 1000
#define MAX_PATH_LENGTH 256
#define PLAYFAIR_SIZE 5
#define HILL_SIZE 3

// ========== HELPER FUNCTIONS ==========
// Helper function to create key file name
void createKeyFileName(char *outputPath, char *keyFileName) {
    // Copy output path to key file name
    strcpy(keyFileName, outputPath);
    
    // Find the last dot in the filename (for extension)
    char *dot = strrchr(keyFileName, '.');
    char *slash = strrchr(keyFileName, '/');
    
    #ifdef _WIN32
    char *backslash = strrchr(keyFileName, '\\');
    if (backslash && (!slash || backslash > slash)) {
        slash = backslash;
    }
    #endif
    
    // If there's a dot after the last slash (or no slash at all)
    if (dot && (!slash || dot > slash)) {
        // Insert ".key" before the extension
        int dotPos = dot - keyFileName;
        memmove(keyFileName + dotPos + 4, keyFileName + dotPos, strlen(keyFileName + dotPos) + 1);
        memcpy(keyFileName + dotPos, ".key", 4);
    } else {
        // No extension found, just append ".key.txt"
        strcat(keyFileName, ".key.txt");
    }
}
// ========== CAESAR CIPHER FUNCTIONS ==========
// Function to get encryption key from user
int getKeyFromUserCeaserCypher() {
    int key;
    printf("Enter the encryption key (1-25): ");
    scanf("%d", &key);
    getchar();  // Consume newline
    
    // Validate key
    while (key < 1 || key > 25) {
        printf("Invalid key! Please enter a value between 1 and 25: ");
        scanf("%d", &key);
        getchar();  // Consume newline
    }
    
    return key;
}

// Function to encrypt text using Caesar cipher (pure encryption logic)
void encryptTextWithCeaserCypher(char *text, int key, char *result) {
    int i = 0;
    char ch;
    
    while (text[i] != '\0') {
        ch = text[i];
        
        if (isalpha(ch)) {
            if (isupper(ch)) {
                result[i] = ((ch - 'A' + key) % 26) + 'A';
            } else {
                result[i] = ((ch - 'a' + key) % 26) + 'a';
            }
        } else {
            result[i] = ch;  // Non-alphabetic characters remain unchanged
        }
        i++;
    }
    result[i] = '\0';
}

// Function to decrypt text using Caesar cipher (pure decryption logic)
void decryptTextWithCeaserCypher(char *text, int key, char *result) {
    int i = 0;
    char ch;
    
    while (text[i] != '\0') {
        ch = text[i];
        
        if (isalpha(ch)) {
            if (isupper(ch)) {
                result[i] = ((ch - 'A' - key + 26) % 26) + 'A';
            } else {
                result[i] = ((ch - 'a' - key + 26) % 26) + 'a';
            }
        } else {
            result[i] = ch;  // Non-alphabetic characters remain unchanged
        }
        i++;
    }
    result[i] = '\0';
}

//<CEASER ENCRYPTION>
// Complete encryption function with file handling and key input
void encryptCaesarWithFileHandling() {
    FILE *inputFile, *outputFile, *keyFile;
    char inputPath[MAX_PATH_LENGTH], outputPath[MAX_PATH_LENGTH];
    char keyFileName[MAX_PATH_LENGTH * 2];  // Allow longer filename
    char plaintext[MAX_TEXT_LENGTH], ciphertext[MAX_TEXT_LENGTH];
    size_t bytesRead;
    
    printf("\n=== Caesar Cipher Encryption ===\n");
    
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
    
    // Get key from user
    int key = getKeyFromUserCeaserCypher();
    
    // Get output file path
    printf("\nEnter the path for the output encrypted file: ");
    fgets(outputPath, sizeof(outputPath), stdin);
    outputPath[strcspn(outputPath, "\n")] = '\0';
    
    // Create key file name
    createKeyFileName(outputPath, keyFileName);
    
    // Perform encryption
    printf("\n--- Encryption Results ---\n");
    encryptTextWithCeaserCypher(plaintext, key, ciphertext);
    printf("Plaintext:\n%s\n", plaintext);
    printf("Ciphertext:\n%s\n", ciphertext);
    printf("Key: %d\n", key);
    
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
        fprintf(keyFile, "=== Caesar Cipher Key ===\n");
        fprintf(keyFile, "Key: %d\n", key);
        fprintf(keyFile, "Encrypted file: %s\n", outputPath);
        fprintf(keyFile, "Original file: %s\n", inputPath);
        fclose(keyFile);
        printf("Key file created: %s\n", keyFileName);
    }
    
    printf("\nEncrypted text has been written to: %s\n", outputPath);
    printf("=== Encryption Complete ===\n");
}

// Complete decryption function with file handling and key input
void decryptCaesarWithFileHandling() {
    FILE *inputFile, *outputFile;
    char inputPath[MAX_PATH_LENGTH], outputPath[MAX_PATH_LENGTH];
    char ciphertext[MAX_TEXT_LENGTH], decryptedtext[MAX_TEXT_LENGTH];
    size_t bytesRead;
    
    printf("\n=== Caesar Cipher Decryption ===\n");
    
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
    
    // Get key from user
    int key = getKeyFromUserCeaserCypher();
    
    // Get output file path
    printf("\nEnter the path for the output decrypted file: ");
    fgets(outputPath, sizeof(outputPath), stdin);
    outputPath[strcspn(outputPath, "\n")] = '\0';
    
    // Perform decryption
    printf("\n--- Decryption Results ---\n");
    decryptTextWithCeaserCypher(ciphertext, key, decryptedtext);
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
//</ceaser encryption>

#endif
