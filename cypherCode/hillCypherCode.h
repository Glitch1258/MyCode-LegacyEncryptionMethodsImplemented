//#include "definations.h"
#ifndef HILL
#define HILL
// ========== HILL CIPHER FUNCTIONS ==========

// Modular inverse for Hill cipher (different implementation for clarity)
int modInverseHill(int a, int m) {
    a = a % m;
    for (int x = 1; x < m; x++) {
        if ((a * x) % m == 1) {
            return x;
        }
    }
    return -1;
}
// Calculate determinant of 3x3 matrix
int determinant3x3(int matrix[HILL_SIZE][HILL_SIZE]) {
    int det = 0;
    
    det = matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1])
        - matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0])
        + matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
    
    return det;
}
// Get Hill cipher key (3x3 matrix) from user
int getHillKeyFromUser(int key[HILL_SIZE][HILL_SIZE]) {
    printf("\nEnter the 3x3 Hill cipher key matrix (9 numbers, 0-25):\n");
    
    for (int i = 0; i < HILL_SIZE; i++) {
        for (int j = 0; j < HILL_SIZE; j++) {
            printf("Enter element [%d][%d]: ", i, j);
            scanf("%d", &key[i][j]);
            
            // Validate input (0-25)
            if (key[i][j] < 0 || key[i][j] > 25) {
                printf("Error: All elements must be between 0 and 25.\n");
                return 0;
            }
        }
    }
    
    // Check if matrix is invertible (determinant must have modular inverse modulo 26)
    int det = determinant3x3(key);
    det = ((det % 26) + 26) % 26;  // Ensure positive modulo 26
    
    if (det == 0) {
        printf("Error: Matrix determinant is 0 (not invertible).\n");
        return 0;
    }
    
    int det_inv = modInverseHill(det, 26);
    if (det_inv == -1) {
        printf("Error: Matrix determinant %d has no modular inverse modulo 26.\n", det);
        printf("Matrix is not suitable for Hill cipher.\n");
        return 0;
    }
    
    return 1;
}

void prepareHillText(char *text, char *result, int *spacePositions, int *spaceCount) {
    int i = 0, j = 0;
    int charCount = 0;  // Count of characters since last space
    *spaceCount = 0;
    
    while (text[i] != '\0') {
        if (isalpha(text[i])) {
            result[j++] = toupper(text[i]);
            charCount++;
        } else if (isspace(text[i])) {
            // Record the position where space occurred
            if (charCount > 0 && *spaceCount < MAX_TEXT_LENGTH) {
                spacePositions[(*spaceCount)++] = charCount;
            }
            charCount = 0;  // Reset counter for next word
        }
        i++;
    }
    
    // If there were trailing characters after last space, record them
    if (charCount > 0 && *spaceCount < MAX_TEXT_LENGTH) {
        spacePositions[(*spaceCount)++] = charCount;
    }
    
    // Pad with 'X' to make length multiple of 3
    while (j % HILL_SIZE != 0) {
        result[j++] = 'X';
    }
    
    result[j] = '\0';
}



// Calculate adjoint of 3x3 matrix
void adjoint3x3(int matrix[HILL_SIZE][HILL_SIZE], int adj[HILL_SIZE][HILL_SIZE]) {
    // Calculate cofactors
    adj[0][0] = (matrix[1][1] * matrix[2][2] - matrix[2][1] * matrix[1][2]);
    adj[0][1] = -(matrix[1][0] * matrix[2][2] - matrix[2][0] * matrix[1][2]);
    adj[0][2] = (matrix[1][0] * matrix[2][1] - matrix[2][0] * matrix[1][1]);
    
    adj[1][0] = -(matrix[0][1] * matrix[2][2] - matrix[2][1] * matrix[0][2]);
    adj[1][1] = (matrix[0][0] * matrix[2][2] - matrix[2][0] * matrix[0][2]);
    adj[1][2] = -(matrix[0][0] * matrix[2][1] - matrix[2][0] * matrix[0][1]);
    
    adj[2][0] = (matrix[0][1] * matrix[1][2] - matrix[1][1] * matrix[0][2]);
    adj[2][1] = -(matrix[0][0] * matrix[1][2] - matrix[1][0] * matrix[0][2]);
    adj[2][2] = (matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1]);
}

// Calculate inverse of 3x3 matrix modulo 26
void inverse3x3(int matrix[HILL_SIZE][HILL_SIZE], int inv[HILL_SIZE][HILL_SIZE]) {
    int det = determinant3x3(matrix);
    det = ((det % 26) + 26) % 26;  // Ensure positive modulo 26
    
    int det_inv = modInverseHill(det, 26);
    
    int adj[HILL_SIZE][HILL_SIZE];
    adjoint3x3(matrix, adj);
    
    // Calculate inverse = (det_inv * adj) mod 26
    for (int i = 0; i < HILL_SIZE; i++) {
        for (int j = 0; j < HILL_SIZE; j++) {
            // Take modulo 26 of adjoint element
            int adj_val = adj[i][j] % 26;
            if (adj_val < 0) adj_val += 26;
            
            // Multiply by modular inverse of determinant
            inv[i][j] = (det_inv * adj_val) % 26;
            if (inv[i][j] < 0) inv[i][j] += 26;
        }
    }
}

// Modified encryptHill function to track spaces
void encryptHill(char *text, int key[HILL_SIZE][HILL_SIZE], char *result, int *spacePositions, int *spaceCount) {
    char preparedText[MAX_TEXT_LENGTH];
    prepareHillText(text, preparedText, spacePositions, spaceCount);
    
    int len = strlen(preparedText);
    int k = 0;
    
    // Process text in blocks of 3
    for (int i = 0; i < len; i += HILL_SIZE) {
        // Convert block to vector
        int vector[HILL_SIZE];
        for (int j = 0; j < HILL_SIZE; j++) {
            vector[j] = preparedText[i + j] - 'A';
        }
        
        // Multiply key matrix with vector
        int encrypted[HILL_SIZE] = {0};
        for (int row = 0; row < HILL_SIZE; row++) {
            for (int col = 0; col < HILL_SIZE; col++) {
                encrypted[row] += key[row][col] * vector[col];
            }
            encrypted[row] = ((encrypted[row] % 26) + 26) % 26;
            result[k++] = encrypted[row] + 'A';
        }
    }
    
    result[k] = '\0';
}

// Modified decryptHill function to restore spaces
void decryptHill(char *text, int key[HILL_SIZE][HILL_SIZE], char *result, int *spacePositions, int spaceCount) {
    int invKey[HILL_SIZE][HILL_SIZE];
    inverse3x3(key, invKey);
    
    int len = strlen(text);
    int k = 0;
    int spaceIndex = 0;
    int charsSinceLastSpace = 0;
    int totalCharsProcessed = 0;
    
    // Process text in blocks of 3
    for (int i = 0; i < len; i += HILL_SIZE) {
        // Convert block to vector
        int vector[HILL_SIZE];
        for (int j = 0; j < HILL_SIZE; j++) {
            vector[j] = text[i + j] - 'A';
        }
        
        // Multiply inverse key matrix with vector
        int decrypted[HILL_SIZE] = {0};
        for (int row = 0; row < HILL_SIZE; row++) {
            for (int col = 0; col < HILL_SIZE; col++) {
                decrypted[row] += invKey[row][col] * vector[col];
            }
            decrypted[row] = ((decrypted[row] % 26) + 26) % 26;
            
            // Check if this character should be a space
            if (spaceIndex < spaceCount) {
                charsSinceLastSpace++;
                if (charsSinceLastSpace > spacePositions[spaceIndex]) {
                    // Insert space before current character
                    result[k++] = ' ';
                    spaceIndex++;
                    charsSinceLastSpace = 1;  // Reset for new word
                }
            }
            
            result[k++] = decrypted[row] + 'A';
            totalCharsProcessed++;
        }
    }
    
    result[k] = '\0';
    
    // Remove padding 'X's at the end
    k = strlen(result);
    while (k > 0 && result[k-1] == 'X') {
        result[--k] = '\0';
    }
    
    // Also remove trailing spaces
    while (k > 0 && result[k-1] == ' ') {
        result[--k] = '\0';
    }
}

// Complete encryption function with file handling for Hill cipher
void encryptHillWithFileHandling() {
    FILE *inputFile, *outputFile, *keyFile;
    char inputPath[MAX_PATH_LENGTH], outputPath[MAX_PATH_LENGTH];
    char keyFileName[MAX_PATH_LENGTH * 2];
    char plaintext[MAX_TEXT_LENGTH], ciphertext[MAX_TEXT_LENGTH];
    size_t bytesRead;
    int hillKey[HILL_SIZE][HILL_SIZE];
    int spacePositions[MAX_TEXT_LENGTH];
    int spaceCount = 0;
    
    // prompt user for input and output path here
    printf("\n=== Hill Cipher Encryption ===\n");
    
    // Get input file path from user
    printf("Enter the path of the input text file to encrypt: ");
    fgets(inputPath, sizeof(inputPath), stdin);
    inputPath[strcspn(inputPath, "\n")] = '\0';
    
    // Get output file path from user
    printf("Enter the path for the output encrypted file: ");
    fgets(outputPath, sizeof(outputPath), stdin);
    outputPath[strcspn(outputPath, "\n")] = '\0';
    
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
    
    // Get Hill key matrix from user
    printf("\n--- Hill Cipher Key Matrix ---\n");
    while (!getHillKeyFromUser(hillKey)) {
        printf("Please try again.\n");
    }
    
    // Create key file name
    createKeyFileName(outputPath, keyFileName);
    
    // Perform encryption
    printf("\n--- Encryption Results ---\n");
    encryptHill(plaintext, hillKey, ciphertext, spacePositions, &spaceCount);
    printf("Plaintext:\n%s\n", plaintext);
    printf("Ciphertext:\n%s\n", ciphertext);
    
    // Display key matrix
    printf("\nHill Cipher Key Matrix (3x3):\n");
    for (int i = 0; i < HILL_SIZE; i++) {
        for (int j = 0; j < HILL_SIZE; j++) {
            printf("%3d ", hillKey[i][j]);
        }
        printf("\n");
    }
    
    // Display space information
    printf("\nSpace positions (word lengths): [");
    for (int i = 0; i < spaceCount; i++) {
        printf("%d", spacePositions[i]);
        if (i < spaceCount - 1) printf(", ");
    }
    printf("]\n");
    printf("Total words: %d\n", spaceCount);
    
    // Calculate and display determinant
    int det = determinant3x3(hillKey);
    det = ((det % 26) + 26) % 26;
    printf("Determinant: %d (mod 26)\n", det);
    
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
        fprintf(keyFile, "=== Hill Cipher Key ===\n");
        fprintf(keyFile, "Key Matrix (3x3):\n");
        for (int i = 0; i < HILL_SIZE; i++) {
            for (int j = 0; j < HILL_SIZE; j++) {
                fprintf(keyFile, "%3d ", hillKey[i][j]);
            }
            fprintf(keyFile, "\n");
        }
        fprintf(keyFile, "Determinant: %d (mod 26)\n", det);
        
        // Save space information
        fprintf(keyFile, "Space positions (word lengths): [");
        for (int i = 0; i < spaceCount; i++) {
            fprintf(keyFile, "%d", spacePositions[i]);
            if (i < spaceCount - 1) fprintf(keyFile, ", ");
        }
        fprintf(keyFile, "]\n");
        fprintf(keyFile, "Total words: %d\n", spaceCount);
        
        fprintf(keyFile, "Encrypted file: %s\n", outputPath);
        fprintf(keyFile, "Original file: %s\n", inputPath);
        
        // Also calculate and save inverse matrix for reference
        int invKey[HILL_SIZE][HILL_SIZE];
        inverse3x3(hillKey, invKey);
        fprintf(keyFile, "\nInverse Matrix (for decryption):\n");
        for (int i = 0; i < HILL_SIZE; i++) {
            for (int j = 0; j < HILL_SIZE; j++) {
                fprintf(keyFile, "%3d ", invKey[i][j]);
            }
            fprintf(keyFile, "\n");
        }
        
        fclose(keyFile);
        printf("Key file created: %s\n", keyFileName);
    }
    
    printf("\nEncrypted text has been written to: %s\n", outputPath);
    printf("=== Encryption Complete ===\n");
}

// Complete decryption function with file handling for Hill cipher
void decryptHillWithFileHandling() {
    FILE *inputFile, *outputFile, *keyFile;
    char inputPath[MAX_PATH_LENGTH], outputPath[MAX_PATH_LENGTH];
    char keyFileName[MAX_PATH_LENGTH * 2];
    char ciphertext[MAX_TEXT_LENGTH], decryptedtext[MAX_TEXT_LENGTH];
    size_t bytesRead;
    int hillKey[HILL_SIZE][HILL_SIZE];
    int spacePositions[MAX_TEXT_LENGTH];
    int spaceCount = 0;
    char line[256];
    
    // prompt user for input and output path here
    printf("\n=== Hill Cipher Decryption ===\n");
    
    // Get input file path from user
    printf("Enter the path of the input text file to decrypt: ");
    fgets(inputPath, sizeof(inputPath), stdin);
    inputPath[strcspn(inputPath, "\n")] = '\0';
    
    // Get output file path from user
    printf("Enter the path for the output decrypted file: ");
    fgets(outputPath, sizeof(outputPath), stdin);
    outputPath[strcspn(outputPath, "\n")] = '\0';
    
    // Try to find key file
    createKeyFileName(inputPath, keyFileName);
    
    // Open key file to read space information if it exists
    keyFile = fopen(keyFileName, "r");
    if (keyFile != NULL) {
        printf("Found key file: %s\n", keyFileName);
        
        // Read space information from key file
        while (fgets(line, sizeof(line), keyFile)) {
            if (strstr(line, "Space positions (word lengths):") != NULL) {
                // Extract space positions from line like: "Space positions (word lengths): [3, 5, 4]"
                char *start = strchr(line, '[');
                char *end = strchr(line, ']');
                if (start && end) {
                    *end = '\0';
                    start++;  // Skip '['
                    
                    // Parse comma-separated values
                    char *token = strtok(start, ",");
                    while (token != NULL && spaceCount < MAX_TEXT_LENGTH) {
                        // Remove whitespace
                        while (*token == ' ') token++;
                        spacePositions[spaceCount++] = atoi(token);
                        token = strtok(NULL, ",");
                    }
                }
            }
        }
        fclose(keyFile);
        
        if (spaceCount > 0) {
            printf("Read space positions from key file: [");
            for (int i = 0; i < spaceCount; i++) {
                printf("%d", spacePositions[i]);
                if (i < spaceCount - 1) printf(", ");
            }
            printf("]\n");
        }
    } else {
        printf("No key file found. Space restoration will not be available.\n");
        printf("Please enter the space positions manually (comma-separated, e.g., '3,5,4'): ");
        fgets(line, sizeof(line), stdin);
        line[strcspn(line, "\n")] = '\0';
        
        if (strlen(line) > 0) {
            // Parse comma-separated values
            char *token = strtok(line, ",");
            while (token != NULL && spaceCount < MAX_TEXT_LENGTH) {
                // Remove whitespace
                while (*token == ' ') token++;
                spacePositions[spaceCount++] = atoi(token);
                token = strtok(NULL, ",");
            }
        }
    }
    
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
    
    // Get Hill key matrix from user
    printf("\n--- Hill Cipher Key Matrix ---\n");
    while (!getHillKeyFromUser(hillKey)) {
        printf("Please try again.\n");
    }
    
    // Perform decryption
    printf("\n--- Decryption Results ---\n");
    decryptHill(ciphertext, hillKey, decryptedtext, spacePositions, spaceCount);
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
}//</hill encryption>
#endif


