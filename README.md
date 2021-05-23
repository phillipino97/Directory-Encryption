# Directory-Encryption
A C program for encrypting a directory and all subdirectories using AES.

This implementation takes a folder name as input and proceeds to encrypt the entire directory and all subdirectories within it. The way it does this is by recursively navigating through the file system looking for any files and then taking them as input and breaking them into 16 bit chunks so that they can be processed by AES. The most complex portion of this implementation is the method of being able to encrypt and decrypt files that have file sizes that result in the necessity of padding. Our implementation takes the encrypted file and adds padded 0 bits to it and at the end it adds the amount of padded data involved in the encryption.

The decryption uses the same method of converting the files into 16 bit chunks, but at the end when it reaches the padding it decrypts it, checks the last bits for the amount of padding, and removes the padding from the data and rewrites the file decrypted.
