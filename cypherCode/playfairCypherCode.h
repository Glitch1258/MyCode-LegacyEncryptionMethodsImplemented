//#include "definations.h"
#include <stdbool.h>
#ifndef PLAY_FAIR
#define PLAY_FAIR

// ========== PLAYFAIR CIPHER FUNCTIONS ==========

// Prepare Playfair key by removing duplicates and 'J' (treat 'I' and 'J' as same)
void preparePlayfairKey(char *key, char *preparedKey) {
    int used[26] = {0};
    int i, j = 0;
    
    // Mark 'J' as used (we'll use 'I' instead)
    used['J' - 'A'] = 1;
    
    for (i = 0; key[i] != '\0'; i++) {
        if (isalpha(key[i])) {
            char ch = toupper(key[i]);
            if (ch == 'J') ch = 'I';  // Treat 'J' as 'I'
            
            if (!used[ch - 'A']) {
                preparedKey[j++] = ch;
                used[ch - 'A'] = 1;
            }
        }
    }
    
    // Fill remaining letters
    for (char ch = 'A'; ch <= 'Z'; ch++) {
        if (ch == 'J') continue;  // Skip 'J'
        if (!used[ch - 'A']) {
            preparedKey[j++] = ch;
        }
    }
    
    preparedKey[j] = '\0';
}

// Generate 5x5 Playfair matrix from prepared key
void generatePlayfairMatrix(char *key, char matrix[PLAYFAIR_SIZE][PLAYFAIR_SIZE]) {
    int k = 0;
    for (int i = 0; i < PLAYFAIR_SIZE; i++) {
        for (int j = 0; j < PLAYFAIR_SIZE; j++) {
            matrix[i][j] = key[k++];
        }
    }
}

// Prepare text for Playfair encryption (add 'X' between double letters, make even length)
// Also track space positions
void preparePlayfairText(char *text, char *result, int *spacePositions, int *spaceCount) {
    int i = 0, j = 0;
    char prev = '\0';
    int charCount = 0;
    *spaceCount = 0;
    
    while (text[i] != '\0') {
        if (isalpha(text[i])) {
            char ch = toupper(text[i]);
            if (ch == 'J') ch = 'I';  // Treat 'J' as 'I'
            
            // If same as previous, insert 'X'
            if (ch == prev) {
                result[j++] = 'X';
                prev = 'X';
            }
            
            result[j++] = ch;
            prev = ch;
            charCount++;
        } else if (text[i] == ' ') {
            // Record space position (number of characters before this space)
            if (charCount > 0) {
                spacePositions[(*spaceCount)++] = charCount;
                charCount = 0;  // Reset character count for next word
            }
        } else if (text[i] == '\n' || text[i] == '\t') {
            // Treat newline and tab as spaces
            if (charCount > 0) {
                spacePositions[(*spaceCount)++] = charCount;
                charCount = 0;
            }
        }
        i++;
    }
    
    // Record any remaining characters as the last word
    if (charCount > 0) {
        spacePositions[(*spaceCount)++] = charCount;
    }
    
    // If odd length, add 'X' at the end
    if (j % 2 != 0) {
        result[j++] = 'X';
    }
    
    result[j] = '\0';
}

// Find position of a character in the Playfair matrix
void findPosition(char matrix[PLAYFAIR_SIZE][PLAYFAIR_SIZE], char ch, int *row, int *col) {
    for (int i = 0; i < PLAYFAIR_SIZE; i++) {
        for (int j = 0; j < PLAYFAIR_SIZE; j++) {
            if (matrix[i][j] == ch) {
                *row = i;
                *col = j;
                return;
            }
        }
    }
}

// Encrypt text using Playfair cipher
void encryptPlayfair(char *text, char *key, char *result, int *spacePositions, int *spaceCount) {
    char preparedKey[26];
    char matrix[PLAYFAIR_SIZE][PLAYFAIR_SIZE];
    char preparedText[MAX_TEXT_LENGTH * 2];
    
    // Prepare key and matrix
    preparePlayfairKey(key, preparedKey);
    generatePlayfairMatrix(preparedKey, matrix);
    
    // Prepare text and track space positions
    preparePlayfairText(text, preparedText, spacePositions, spaceCount);
    
    // Encrypt pairs of letters
    int i = 0, j = 0;
    while (preparedText[i] != '\0') {
        char a = preparedText[i];
        char b = preparedText[i + 1];
        
        int row1, col1, row2, col2;
        findPosition(matrix, a, &row1, &col1);
        findPosition(matrix, b, &row2, &col2);
        
        if (row1 == row2) {
            // Same row: shift right
            result[j++] = matrix[row1][(col1 + 1) % PLAYFAIR_SIZE];
            result[j++] = matrix[row2][(col2 + 1) % PLAYFAIR_SIZE];
        } else if (col1 == col2) {
            // Same column: shift down
            result[j++] = matrix[(row1 + 1) % PLAYFAIR_SIZE][col1];
            result[j++] = matrix[(row2 + 1) % PLAYFAIR_SIZE][col2];
        } else {
            // Rectangle: swap columns
            result[j++] = matrix[row1][col2];
            result[j++] = matrix[row2][col1];
        }
        
        i += 2;
    }
    
    result[j] = '\0';
}

// Decrypt text using Playfair cipher and restore spaces
void decryptPlayfair(char *text, char *key, char *result, int *spacePositions, int spaceCount) {
    char preparedKey[26];
    char matrix[PLAYFAIR_SIZE][PLAYFAIR_SIZE];
    
    // Prepare key and matrix
    preparePlayfairKey(key, preparedKey);
    generatePlayfairMatrix(preparedKey, matrix);
    
    // Decrypt pairs of letters
    int i = 0, j = 0;
    char decryptedWithoutSpaces[MAX_TEXT_LENGTH];
    
    while (text[i] != '\0') {
        char a = text[i];
        char b = text[i + 1];
        
        int row1, col1, row2, col2;
        findPosition(matrix, a, &row1, &col1);
        findPosition(matrix, b, &row2, &col2);
        
        if (row1 == row2) {
            // Same row: shift left
            decryptedWithoutSpaces[j++] = matrix[row1][(col1 - 1 + PLAYFAIR_SIZE) % PLAYFAIR_SIZE];
            decryptedWithoutSpaces[j++] = matrix[row2][(col2 - 1 + PLAYFAIR_SIZE) % PLAYFAIR_SIZE];
        } else if (col1 == col2) {
            // Same column: shift up
            decryptedWithoutSpaces[j++] = matrix[(row1 - 1 + PLAYFAIR_SIZE) % PLAYFAIR_SIZE][col1];
            decryptedWithoutSpaces[j++] = matrix[(row2 - 1 + PLAYFAIR_SIZE) % PLAYFAIR_SIZE][col2];
        } else {
            // Rectangle: swap columns
            decryptedWithoutSpaces[j++] = matrix[row1][col2];
            decryptedWithoutSpaces[j++] = matrix[row2][col1];
        }
        
        i += 2;
    }
    
    decryptedWithoutSpaces[j] = '\0';
    
    // Process the decrypted text to handle 'X' removal and space restoration
    int k = 0; // Index for final result
    int currentWordChars = 0;
    int spaceIndex = 0;
    
    for (int m = 0; m < j; m++) {
        // Check if current character is 'X'
        if (decryptedWithoutSpaces[m] == 'X') {
            // Check if 'X' should be removed based on your rules:
            // 1. If 'X' is between two identical letters
            // 2. If 'X' is at the end of text (trailing)
            // 3. If 'X' is at the beginning of text (leading)
            // 4. If 'X' appears where we need to insert a space
            
            bool shouldRemoveX = false;
            
            // Rule 1: Check if 'X' is between two identical letters
            if (m > 0 && m < j - 1) {
                if (decryptedWithoutSpaces[m-1] == decryptedWithoutSpaces[m+1]) {
                    shouldRemoveX = true;
                }
            }
            
            // Rule 2: Check if 'X' is trailing (at the end of text)
            if (m == j - 1) {
                shouldRemoveX = true;
            }
            
            // Rule 3: Check if 'X' is leading (at the beginning of text)
            if (m == 0) {
                shouldRemoveX = true;
            }
            
            // Rule 4: Check if 'X' appears where we need to insert a space
            if (spaceIndex < spaceCount && currentWordChars == spacePositions[spaceIndex]) {
                // We need to insert a space here instead of the 'X'
                result[k++] = ' ';
                currentWordChars = 0;
                spaceIndex++;
                shouldRemoveX = true; // Don't add the 'X'
            }
            
            // If we shouldn't remove this 'X', add it to result
            if (!shouldRemoveX) {
                result[k++] = decryptedWithoutSpaces[m];
                currentWordChars++;
            }
            // If we should remove it, just skip it (don't increment currentWordChars)
        } 
        else {
            // Not an 'X', add the character
            result[k++] = decryptedWithoutSpaces[m];
            currentWordChars++;
            
            // Check if we need to insert a space after this character
            if (spaceIndex < spaceCount && currentWordChars == spacePositions[spaceIndex]) {
                result[k++] = ' ';
                currentWordChars = 0;
                spaceIndex++;
            }
        }
    }
    
    result[k] = '\0';
    
    // Additional cleanup: Remove any trailing 'X' that might have been missed
    if (k > 0 && result[k-1] == 'X') {
        result[k-1] = '\0';
        k--;
    }
    
    // Also remove any 'X' at the beginning if present
    if (k > 0 && result[0] == 'X') {
        // Shift everything left by 1
        for (int i = 0; i < k; i++) {
            result[i] = result[i+1];
        }
        k--;
    }
}

// Function to format space positions as a string like "[3,5,4]"
void formatSpacePositions(int *spacePositions, int spaceCount, char *formatted) {
    if (spaceCount == 0) {
        strcpy(formatted, "[]");
        return;
    }
    
    strcpy(formatted, "[");
    char buffer[20];
    
    for (int i = 0; i < spaceCount; i++) {
        sprintf(buffer, "%d", spacePositions[i]);
        strcat(formatted, buffer);
        
        if (i < spaceCount - 1) {
            strcat(formatted, ",");
        }
    }
    
    strcat(formatted, "]");
}

// Function to parse space positions from string like "[3,5,4]"
int parseSpacePositions(char *formatted, int *spacePositions) {
    int count = 0;
    char *token;
    
    // Remove brackets
    if (formatted[0] == '[') {
        formatted++;
    }
    if (formatted[strlen(formatted)-1] == ']') {
        formatted[strlen(formatted)-1] = '\0';
    }
    
    // Split by commas
    token = strtok(formatted, ",");
    while (token != NULL && count < MAX_TEXT_LENGTH) {
        spacePositions[count++] = atoi(token);
        token = strtok(NULL, ",");
    }
    
    return count;
}

// Complete encryption function with file handling for Playfair cipher
void encryptPlayfairWithFileHandling() {
    FILE *inputFile, *outputFile, *keyFile;
    char inputPath[MAX_PATH_LENGTH], outputPath[MAX_PATH_LENGTH];
    char keyFileName[MAX_PATH_LENGTH * 2];
    char plaintext[MAX_TEXT_LENGTH], ciphertext[MAX_TEXT_LENGTH];
    char playfairKey[100];
    size_t bytesRead;
    int spacePositions[MAX_TEXT_LENGTH];
    int spaceCount = 0;
    char spacePositionsStr[500];
    
    printf("\n=== Playfair Cipher Encryption ===\n");
    
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
    
    // Get Playfair key from user
    printf("\nEnter the Playfair cipher key (letters only): ");
    fgets(playfairKey, sizeof(playfairKey), stdin);
    playfairKey[strcspn(playfairKey, "\n")] = '\0';
    
    // Validate key contains at least some letters
    int hasLetters = 0;
    for (int i = 0; playfairKey[i] != '\0'; i++) {
        if (isalpha(playfairKey[i])) {
            hasLetters = 1;
            break;
        }
    }
    
    if (!hasLetters) {
        printf("Error: Key must contain at least one letter.\n");
        return;
    }
    
    // Get output file path
    printf("\nEnter the path for the output encrypted file: ");
    fgets(outputPath, sizeof(outputPath), stdin);
    outputPath[strcspn(outputPath, "\n")] = '\0';
    
    // Create key file name
    createKeyFileName(outputPath, keyFileName);
    
    // Perform encryption
    printf("\n--- Encryption Results ---\n");
    encryptPlayfair(plaintext, playfairKey, ciphertext, spacePositions, &spaceCount);
    printf("Plaintext:\n%s\n", plaintext);
    printf("Ciphertext:\n%s\n", ciphertext);
    printf("Key: %s\n", playfairKey);
    
    // Format space positions for storage
    formatSpacePositions(spacePositions, spaceCount, spacePositionsStr);
    printf("Space positions: %s\n", spacePositionsStr);
    
    // Show Playfair matrix (for information)
    char preparedKey[26];
    char matrix[PLAYFAIR_SIZE][PLAYFAIR_SIZE];
    preparePlayfairKey(playfairKey, preparedKey);
    generatePlayfairMatrix(preparedKey, matrix);
    
    printf("\nPlayfair Matrix:\n");
    for (int i = 0; i < PLAYFAIR_SIZE; i++) {
        for (int j = 0; j < PLAYFAIR_SIZE; j++) {
            printf("%c ", matrix[i][j]);
        }
        printf("\n");
    }
    
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
        fprintf(keyFile, "=== Playfair Cipher Key ===\n");
        fprintf(keyFile, "Key: %s\n", playfairKey);
        fprintf(keyFile, "Prepared Key: %s\n", preparedKey);
        fprintf(keyFile, "Encrypted file: %s\n", outputPath);
        fprintf(keyFile, "Original file: %s\n", inputPath);
        fprintf(keyFile, "Space positions: %s\n", spacePositionsStr);
        fprintf(keyFile, "\nPlayfair Matrix:\n");
        for (int i = 0; i < PLAYFAIR_SIZE; i++) {
            for (int j = 0; j < PLAYFAIR_SIZE; j++) {
                fprintf(keyFile, "%c ", matrix[i][j]);
            }
            fprintf(keyFile, "\n");
        }
        fclose(keyFile);
        printf("Key file created: %s\n", keyFileName);
    }
    
    printf("\nEncrypted text has been written to: %s\n", outputPath);
    printf("=== Encryption Complete ===\n");
}

// Complete decryption function with file handling for Playfair cipher
void decryptPlayfairWithFileHandling() {
    FILE *inputFile, *outputFile, *keyFile;
    char inputPath[MAX_PATH_LENGTH], outputPath[MAX_PATH_LENGTH];
    char keyFileName[MAX_PATH_LENGTH * 2];
    char ciphertext[MAX_TEXT_LENGTH], decryptedtext[MAX_TEXT_LENGTH];
    char playfairKey[100];
    size_t bytesRead;
    int spacePositions[MAX_TEXT_LENGTH];
    int spaceCount = 0;
    char keyFileContent[MAX_TEXT_LENGTH * 2];
    char *spacePositionsStr = NULL;
    
    printf("\n=== Playfair Cipher Decryption ===\n");
    
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
    
    // Get Playfair key from user
    printf("\nEnter the Playfair cipher key used for encryption: ");
    fgets(playfairKey, sizeof(playfairKey), stdin);
    playfairKey[strcspn(playfairKey, "\n")] = '\0';
    
    // Try to find and read the key file for space positions
    createKeyFileName(inputPath, keyFileName);
    keyFile = fopen(keyFileName, "r");
    
    if (keyFile != NULL) {
        printf("Found key file: %s\n", keyFileName);
        // Read the key file to find space positions
        while (fgets(keyFileContent, sizeof(keyFileContent), keyFile) != NULL) {
            if (strstr(keyFileContent, "Space positions: ") != NULL) {
                spacePositionsStr = strstr(keyFileContent, ": ") + 2;
                // Remove newline if present
                spacePositionsStr[strcspn(spacePositionsStr, "\n")] = '\0';
                spaceCount = parseSpacePositions(spacePositionsStr, spacePositions);
                printf("Space positions restored from key file: %s\n", spacePositionsStr);
                break;
            }
        }
        fclose(keyFile);
    } else {
        printf("Key file not found: %s\n", keyFileName);
        printf("Space positions will not be restored.\n");
    }
    
    // Get output file path
    printf("\nEnter the path for the output decrypted file: ");
    fgets(outputPath, sizeof(outputPath), stdin);
    outputPath[strcspn(outputPath, "\n")] = '\0';
    
    // Perform decryption
    printf("\n--- Decryption Results ---\n");
    decryptPlayfair(ciphertext, playfairKey, decryptedtext, spacePositions, spaceCount);
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
#endif
