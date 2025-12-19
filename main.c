#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "./cypherCode/ceaserCypherCode.h"
#include "./cypherCode/affineCypherCode.h"
#include "./cypherCode/playfairCypherCode.h"
#include "./cypherCode/hillCypherCode.h"
#include "./cypherCode/hillCracker.h"
//decryptHillCypher()

int main() {
    int choice = 0;
    
    while(choice != -1) {
        printf("\n===  Cipher Program ===\n");
        printf("1. Encrypt a file using Caesar Encryption\n");
        printf("2. Decrypt Caesar Encryption\n");
        printf("3. Encrypt a file using Affine Encryption\n");
        printf("4. Decrypt Affine Encryption\n");
        printf("5. Encrypt a file using Playfair Encryption\n");
        printf("6. Decrypt Playfair Encryption\n");
        printf("7. Encrypt a file using Hill Encryption\n");
        printf("8. Decrypt Hill Encryption\n");
		printf("9. Crack Hill Cypher\n");
        printf("-1. Exit Program\n");
        printf("Enter your choice (1-8 or -1): ");
        scanf("%d", &choice);
        getchar();  // Consume newline
        
        switch(choice) {
            case 1:
                encryptCaesarWithFileHandling();
                break;
            case 2:
                decryptCaesarWithFileHandling();
                break;
            case 3:
                encryptAffineWithFileHandling();
                break;
            case 4:
                decryptAffineWithFileHandling();
                break;
            case 5:
                encryptPlayfairWithFileHandling();
                break;
            case 6:
                decryptPlayfairWithFileHandling();
                break;
            case 7:
                encryptHillWithFileHandling();
                break;
            case 8:
                decryptHillWithFileHandling();
                break;
			case 9:
				crackHillCypher();
				break;
            case -1: 
                printf("Exiting program...\n");
                return 0;
            default:
                printf("Invalid choice! Please enter 1-8 or -1.\n");
        }
    }
    
    return 0;
}






