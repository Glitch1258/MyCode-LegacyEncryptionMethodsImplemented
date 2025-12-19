#ifndef DEFINATIONS
#define DEFINATIONS

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

#endif
