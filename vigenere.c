#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define LOW_CAESER(letter) (letter - 'a' + shift) % 26 + 'a'
#define UP_CAESER(LETTER) (LETTER - 'A' + shift) % 26 + 'A'
#define BUFFER_SIZE 4096
#define MAX_KEY_SIZE BUFFER_SIZE

char helpmsg[] =
"This program enciphers and deciphers text using the \
Vigenere cipher.\n\
Usage: [-h] [-p] [-d] -k <key> (-i | -f <file>)\n\n\
Options:\n\
-h\t\t-- Shows this help page.\n\
-i\t\t-- Sets the program to interactive mode. (Cannot be used with -f)\n\
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
			// Interactive mode.
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
				strncpy(key, optarg, sizeof(key));
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

	char linebuffer[BUFFER_SIZE];
	int shift = 0, decrypt_shift = 0, key_ind = 0;

	while(fgets(linebuffer, sizeof(linebuffer), plaintext) != NULL){
		char cleanbuffer[BUFFER_SIZE] = "\0";
		int j = 0;

		for(int i=0; i<BUFFER_SIZE; ++i){
			if(isdigit(linebuffer[i]) || linebuffer[i] == '\n' || linebuffer[i] == '\0'){
				if(p_optflag == 0){
					cleanbuffer[j] = linebuffer[i];
					++j;
				}

				// Keep going when numbers are encountered.
				if(isdigit(linebuffer[i])){
					continue;
				} else{
					break;
				}
			}

			if(isalpha(linebuffer[i])){
				// Lowercase:
				if(islower(linebuffer[i])){
					// Lowercase encryption:
					if(d_optflag == 0){
						shift = key[key_ind % keylen] - 'a';

						// Without format preservation:
						if(p_optflag == 0){
							cleanbuffer[j] = LOW_CAESER(linebuffer[i]);
							++j;
						// With format preservation:
						} else{
							linebuffer[i] = LOW_CAESER(linebuffer[i]);
						}
					// Lowercase decryption:
					} else{
						decrypt_shift = linebuffer[i] - key[key_ind % keylen] + 'a';

						// Without format preservation:
						if(p_optflag == 0){
							cleanbuffer[j] = decrypt_shift < 'a' ? decrypt_shift + 26 : decrypt_shift;
							++j;
						// With format preservation:
						} else{
							linebuffer[i] = decrypt_shift < 'a' ? decrypt_shift + 26 : decrypt_shift;
						}
					}
				// Uppercase:
				} else{
					// Uppercase encryption:
					if(d_optflag == 0){
						shift = key[key_ind % keylen] - 'a';

						if(p_optflag == 0){
							cleanbuffer[j] = tolower(UP_CAESER(linebuffer[i]));
							++j;
						// With format preservation:
						} else{
							linebuffer[i] = UP_CAESER(linebuffer[i]);
						}
					// Uppercase decryption:
					} else{
						// Without format preservation:
						if(p_optflag == 0){
							decrypt_shift = tolower(linebuffer[i]) - key[key_ind % keylen] + 'a';
							cleanbuffer[j] = decrypt_shift < 'a' ? decrypt_shift + 26 : decrypt_shift;
							++j;
						// With format preservation:
						} else{
							decrypt_shift = linebuffer[i] - toupper(key[key_ind % keylen]) + 'A';
							linebuffer[i] = decrypt_shift < 'A' ? decrypt_shift + 26 : decrypt_shift;
						}
					}
				}

				++key_ind;
			}
		}

		if(p_optflag == 1){
			printf("%s", linebuffer);
		} else{
			printf("%s", cleanbuffer);
		}
	}

	fclose(plaintext);
	return 0;
}
