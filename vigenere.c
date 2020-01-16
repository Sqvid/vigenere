/* To Do:
 * 	+ Implement decryption.
 * 	+ Make punctuation preservation optional.
 * 	+ Add help option.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define LOW_CAESER(letter) (letter - 'a' + shift) % 26 + 'a'
#define UP_CAESER(LETTER) (LETTER - 'A' + shift) % 26 + 'A'
#define BUFFER_SIZE 2048
#define MAX_KEY_SIZE 2048

int main(int argc, char* argv[]){
	extern char* optarg;
	int f_optflag = 0, d_optflag = 0, i_optflag = 0, k_optflag = 0, p_optflag = 0;
	int keylen = 0;
	FILE* plaintext = NULL;

	char opt;
	char key[MAX_KEY_SIZE];

	while((opt = getopt(argc, argv, "if:k:dp")) != -1){
		switch(opt){
			// Interactive mode.
			case 'i':
				if(f_optflag == 1){
					fprintf(stderr, "The -i and -f flags cannot be used together.\n");
					exit(1);
				}

				i_optflag = 1;
				plaintext = fopen("/dev/stdin", "r");

				break;
			case 'f':
				if(i_optflag == 1){
					fprintf(stderr, "The -i and -f flags cannot be used together.\n");
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
				strncpy(key, optarg, sizeof(key) - 1);
				keylen = strlen(key);

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
			case '?':
				fprintf(stderr, "Usage -k <key> [-d] (-i | -f <filename>)\n");
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

	char linebuffer[BUFFER_SIZE], cleanbuffer[BUFFER_SIZE];
	int shift = 0, decrypt_shift = 0, keyind = 0;

	while(fgets(linebuffer, sizeof(linebuffer), plaintext) != NULL){
		for(int i=0; i<BUFFER_SIZE; ++i){
			if(linebuffer[i] == '\n' || linebuffer[i] == '\0'){
				break;
			}

			//if(p_optflag == 0){
			//	if(isalpha(linebuffer[i]) && isupper(linebuffer)){
			//		cleanbuffer[i] = tolower(linebuffer[i]);
			//	}

			//	cleanbuffer[i] = linebuffer[i];
			//}

			if(isalpha(linebuffer[i])){
				if(islower(linebuffer[i])){
					if(d_optflag == 0){
						shift = key[keyind % keylen] - 'a';
						linebuffer[i] = LOW_CAESER(linebuffer[i]);
					} else{
						decrypt_shift = linebuffer[i] - key[keyind % keylen] + 'a';
						linebuffer[i] = decrypt_shift < 'a' ? decrypt_shift + 26 : decrypt_shift;
					}
				} else{
					if(d_optflag == 0){
						shift = key[keyind % keylen] - 'a';
						linebuffer[i] = UP_CAESER(linebuffer[i]);
					} else{
						decrypt_shift = linebuffer[i] - toupper(key[keyind % keylen]) + 'A';
						linebuffer[i] = decrypt_shift < 'A' ? decrypt_shift + 26 : decrypt_shift;
					}
				}

				++keyind;
			}
		}

		printf("%s", linebuffer);
	}

	fclose(plaintext);
	return 0;
}
