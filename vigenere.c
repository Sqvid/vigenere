#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define CLEAN_CAESAR(letter) (tolower(letter) - 'a' + shift) % 26 + 'a'
#define LOW_CAESAR(letter) (letter - 'a' + shift) % 26 + 'a'
#define UP_CAESAR(LETTER) (LETTER - 'A' + shift) % 26 + 'A'
#define MAX_KEY_SIZE 4096

char helpmsg[] =
"This program enciphers and deciphers text using the \
Vigenere cipher.\n\
Usage: [-h] [-p] [-d] -k <key> (-i | -f <file>)\n\n\
Options:\n\
-h\t\t-- Shows this help page.\n\
-i\t\t-- Accept input from stdin. (Cannot be used with -f)\n\
-f <file>\t-- Specifies the input file. (Cannot be used with -i)\n\
-k <key>\t-- Specifies the cipher key.\n\
-d\t\t-- Sets the program to decryption mode.\n\
-p\t\t-- Preserves case, punctuation, and spacing. (Less secure)\n";


int main(int argc, char* argv[]){
	extern char* optarg;
	int f_optflag = 0, d_optflag = 0, i_optflag = 0, k_optflag = 0, p_optflag = 0;
	int keylen = 0;
	FILE* plaintext = NULL;

	char opt;
	char key[MAX_KEY_SIZE];

	while((opt = getopt(argc, argv, "if:k:dph")) != -1){
		switch(opt){
			// Stdin mode.
			case 'i':
				if(f_optflag == 1){
					fprintf(stderr, "The -i and -f flags cannot be used together.\n");
					printf("For help use the -h option.\n");
					exit(1);
				}

				i_optflag = 1;
				plaintext = fopen("/dev/stdin", "r");

				break;
			case 'f':
				if(i_optflag == 1){
					fprintf(stderr, "The -i and -f flags cannot be used together.\n");
					printf("For help use the -h option.\n");
					exit(1);
				}

				plaintext = fopen(optarg, "r");

				if(! plaintext){
					perror("Error");
					exit(1);
				}

				f_optflag = 1;
				break;
			// Encryption/decryption key.
			case 'k':
				snprintf(key, sizeof(key), optarg);
				keylen = strnlen(key, sizeof(key));

				for(int i=0; i<keylen; ++i){
					if(! isalpha(key[i])){
						fprintf(stderr, "Key must contain letters only.\n");
						exit(1);
					}

					key[i] = tolower(key[i]);
				}

				k_optflag = 1;
				break;
			// Sets mode to decrypt.
			case 'd':
				d_optflag = 1;
				break;
			// Preserve formatting.
			case 'p':
				p_optflag = 1;
				break;
			case 'h':
				if(i_optflag == 1 || f_optflag == 1 || k_optflag == 1\
						|| d_optflag == 1 || p_optflag == 1){

					fprintf(stderr, "Do not run the -h option with any other arguments.\n");
					exit(1);
				}

				printf("%s", helpmsg);
				return 0;
			case '?':
				printf("For help use the -h option.\n");
				exit(1);
				break;
		}
	}

	if(f_optflag == 0 && i_optflag == 0){
		fprintf(stderr, "Please provide a filename or use interactive mode.\n");
		exit(1);
	}

	if(k_optflag == 0){
		fprintf(stderr, "Please provide an encryption/decryption key.\n");
		exit(1);
	}

	if(d_optflag == 1){
		//shift = 26 - shift;
	}

	char rawletter;
	int shift = 0, decrypt_shift = 0, key_ind = 0;

	while((rawletter = fgetc(plaintext)) != EOF){
		// Like rawletter but will only hold alphanumeric characters.
		char cleanletter = '\0';
		int j = 0;

		if((isdigit(rawletter) || rawletter == '\n') && p_optflag == 0){
			cleanletter = rawletter;
			++j;
		} else if(isalpha(rawletter)){
			if(p_optflag == 0){
				// Encryption:
				if(d_optflag == 0){
					shift = key[key_ind % keylen] - 'a';

					cleanletter = CLEAN_CAESAR(rawletter);
					++j;
				// Decryption:
				} else{
					decrypt_shift = tolower(rawletter) \
							- key[key_ind % keylen];

					if(decrypt_shift < 0){
						cleanletter = decrypt_shift + 26 + 'a';
					} else{
						cleanletter = decrypt_shift + 'a';
					}

					++j;
				}
			} else{
				// Encryption:
				if(d_optflag == 0){
					shift = key[key_ind % keylen] - 'a';

					if(islower(rawletter)){
						rawletter = LOW_CAESAR(rawletter);
					} else{
						rawletter = UP_CAESAR(rawletter);
					}
				// Decryption:
				} else{
					decrypt_shift = tolower(rawletter) \
							- key[key_ind % keylen];

					if(islower(rawletter)){
						if(decrypt_shift < 0){
							rawletter = decrypt_shift + 26 + 'a';
						} else{
							rawletter = decrypt_shift + 'a';
						}
					} else{
						if(decrypt_shift < 0){
							rawletter = decrypt_shift + 26 + 'A';
						} else{
							rawletter = decrypt_shift + 'A';
						}
					}
				}
			}

			++key_ind;
		} else{
			if(p_optflag == 0){
				continue;
			}
		}

		if(p_optflag == 0){
			printf("%c", cleanletter);
		} else{
			printf("%c", rawletter);
		}
	}

	fclose(plaintext);
	return 0;
}
