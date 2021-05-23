#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <openssl/aes.h>

#define MAX_DIRECTORY 64
#define MAX_KEY 128

int searchDirectory(char* dir_name, int en_de);
void encryptFile(char* filename);
void decryptFile(char* filename);

char key[AES_BLOCK_SIZE];
AES_KEY aes_key;

int main(void) {

  char e_d;
  char y_n;

  char directory[MAX_DIRECTORY];

  int completion_indicator = 0;
  int c;

  while(1) {

    printf("Encrypt/Decrypt (E/D) (Q to quit): ");
    scanf("%c", &e_d);
    while ((c = getchar()) != '\n' && c != EOF) { }

    if(e_d == 'E' || e_d == 'e') {

      while(1) {

        printf("Enter directory to encrypt (Q to quit): ");
        scanf("%[^\n]", directory);
        while ((c = getchar()) != '\n' && c != EOF) { }

        if(directory == "Q" || directory == "q") {

          break;

        }

        printf("Enter key: ");
        scanf("%[^\n]", key);
        while ((c = getchar()) != '\n' && c != EOF) { }

        printf("Ensure this is the directory you want to encrypt:\n%s (Y/N): ", directory);
        scanf("%c", &y_n);
        while ((c = getchar()) != '\n' && c != EOF) { }

        if(y_n == 'Y' || y_n == 'y') {

          completion_indicator = searchDirectory(directory, 0);

          if(completion_indicator) {

            printf("Directory encryption completed successfully, exiting.\n");

          } else {

            printf("Directory could not be encrypted, incorrect path or insufficient permissions.\n");

          }

          break;

        } else if(y_n == 'N' || y_n == 'n') {

          printf("Incorrect directory please try again.\n");

        } else {

          printf("Invalid option please try again.\n");

        }

      }

      break;

    } else if(e_d == 'D' || e_d == 'd') {

      while(1) {

        printf("Enter directory to decrypt (Q to quit): ");
        scanf("%[^\n]", directory);
        while ((c = getchar()) != '\n' && c != EOF) { }

        if(directory == "Q" || directory == "q") {

          break;

        }

        printf("Enter key: ");
        scanf("%[^\n]", key);
        while ((c = getchar()) != '\n' && c != EOF) { }

        printf("Ensure this is the directory you want to decrypt:\n%s (Y/N): ", directory);
        scanf("%c", &y_n);

        if(y_n == 'Y' || y_n == 'y') {

          completion_indicator = searchDirectory(directory, 1);

          if(completion_indicator) {

            printf("Directory decryption completed successfully, exiting.\n");

          } else {

            printf("Directory could not be decrypted, incorrect path or insufficient permissions.\n");

          }

          break;

        } else if(y_n == 'N' || y_n == 'n') {

          printf("Incorrect directory please try again.\n");

        } else {

          printf("Invalid option please try again.\n");

        }

      }

      break;

    } else if(e_d == 'Q' || e_d == 'q') {

      break;

    } else {

      printf("Invalid option, please try again.\n");

    }

  }

  exit(1);

}

int searchDirectory(char* dir_name, int en_de) {

  FILE* file;
  DIR* dir;
  DIR* temp_dir;
  char temp_dir_name[MAX_DIRECTORY];
  bzero(temp_dir_name, MAX_DIRECTORY);
  struct dirent* ent;

  if ((dir = opendir(dir_name)) != NULL) {

    while ((ent = readdir(dir)) != NULL) {

      if(ent->d_name[0] != '.') {

        strcpy(temp_dir_name, dir_name);
        strcat(temp_dir_name, "/");
        strcat(temp_dir_name, ent->d_name);
        if((temp_dir = opendir(temp_dir_name)) != NULL) {

          int i = searchDirectory(temp_dir_name, en_de);
          closedir(temp_dir);

        } else {

          if(en_de == 0) {

            printf("Encrypting %s\n", temp_dir_name);
            encryptFile(temp_dir_name);

          } else if(en_de == 1) {

            printf("Decrypting %s\n", temp_dir_name);
            decryptFile(temp_dir_name);

          }

        }

        bzero(temp_dir_name, MAX_DIRECTORY);

      }

    }

    closedir (dir);
    return 1;

  } else {

    perror ("");

  }

  return 0;

}

void encryptFile(char* filename) {

  FILE* file;
  char enc_filename[MAX_DIRECTORY];
  int filesize;
  int padding;
  char unencrypted_block[AES_BLOCK_SIZE + 1] = {'\0'};
  char encrypted_block[AES_BLOCK_SIZE + 1] = {'\0'};
  AES_set_encrypt_key(key, MAX_KEY, &aes_key);

  file = fopen(filename, "r");
  if(file) {

    fseek(file, 0, SEEK_END);
    filesize = ftell(file);
    fclose(file);

  }

  padding = -((filesize % AES_BLOCK_SIZE) - AES_BLOCK_SIZE);
  char unencrypted_file_text[filesize + 1];
  memset(unencrypted_file_text, '\0', filesize + 1);
  char encrypted_file_text[filesize + padding + 1];
  memset(encrypted_file_text, '\0', filesize + padding + 1);

  file = fopen(filename, "r");
  if(file) {

    fread(unencrypted_file_text, 1, filesize, file);
    fclose(file);

  }

  //TODO: split incoming data into 16 byte chunks

  for(int i = 0; i < (filesize + padding)/AES_BLOCK_SIZE; i++) {

    for(int j = 0; j < AES_BLOCK_SIZE; j++) {

      if(i == ((filesize + padding)/AES_BLOCK_SIZE) - 1) {

        if(j >= (AES_BLOCK_SIZE - padding)) {

          unencrypted_block[j] = (char) padding;

        } else {

          unencrypted_block[j] = unencrypted_file_text[AES_BLOCK_SIZE*i + j];

        }

      } else {

        unencrypted_block[j] = unencrypted_file_text[AES_BLOCK_SIZE*i + j];

      }

    }

    AES_encrypt(unencrypted_block, encrypted_block, &aes_key);

    for(int j = 0; j < AES_BLOCK_SIZE; j++) {

      encrypted_file_text[i*AES_BLOCK_SIZE + j] = encrypted_block[j];

    }

    memset(unencrypted_block, '\0', AES_BLOCK_SIZE);
    memset(encrypted_block, '\0', AES_BLOCK_SIZE);

  }

  strcpy(enc_filename, filename);
  strcat(enc_filename, ".enc");
  FILE* enc_file = fopen(enc_filename, "w+");
  for(int i = 0; i < (filesize + padding); i++) {

    fprintf(enc_file, "%c", encrypted_file_text[i]);

  }
  fclose(enc_file);

  remove(filename);

}

void decryptFile(char* filename) {

  char end[5] = {'\0'};
  for(int i = 0; i < 4; i++) {

    end[i] = filename[strlen(filename) - (4-i)];

  }

  //ill be honest, asking end == ".enc" wouldn't work and this does
  //I try really hard not to question it
  if(end[0] == '.' && end[1] == 'e' && end[2] == 'n' && end[3] == 'c') {

    FILE* file;
    char dec_filename[MAX_DIRECTORY] = {'\0'};
    int filesize;
    int padding;
    char unencrypted_block[AES_BLOCK_SIZE + 1] = {'\0'};
    char encrypted_block[AES_BLOCK_SIZE + 1] = {'\0'};
    AES_set_decrypt_key(key, MAX_KEY, &aes_key);

    file = fopen(filename, "r");
    if(file) {

      fseek(file, 0, SEEK_END);
      filesize = ftell(file);
      fclose(file);

    }

    char unencrypted_file_text[filesize + 1];
    memset(unencrypted_file_text, '\0', filesize + 1);
    char encrypted_file_text[filesize + 1];
    memset(encrypted_file_text, '\0', filesize + 1);

    file = fopen(filename, "r");
    if(file) {

      fread(encrypted_file_text, 1, filesize, file);
      fclose(file);

    }

    //TODO: split incoming data into 16 byte chunks

    for(int i = 0; i < filesize/AES_BLOCK_SIZE; i++) {

      for(int j = 0; j < AES_BLOCK_SIZE; j++) {

        encrypted_block[j] = encrypted_file_text[AES_BLOCK_SIZE*i + j];

      }

      AES_decrypt(encrypted_block, unencrypted_block, &aes_key);

      for(int j = 0; j < AES_BLOCK_SIZE; j++) {

        unencrypted_file_text[i*AES_BLOCK_SIZE + j] = unencrypted_block[j];

      }

      memset(unencrypted_block, '\0', AES_BLOCK_SIZE);
      memset(encrypted_block, '\0', AES_BLOCK_SIZE);

    }

    for(int i = 0; i < strlen(filename) - 4; i++) {

      dec_filename[i] = filename[i];

    }

    FILE* dec_file = fopen(dec_filename, "w+");
    padding = unencrypted_file_text[strlen(unencrypted_file_text) - 1];

    if(padding < 0) {

      padding *= -1;

    }

    for(int i = 0; i < filesize - padding; i++) {

      fprintf(dec_file, "%c", unencrypted_file_text[i]);

    }
    fclose(dec_file);

    remove(filename);

  } else {

  }

}
