
  #include <stdio.h>
  #include <openssl/md5.h>

  int main(int argc, const char *argv[])
  {
    MD5_CTX c;
    unsigned char buffer[MD5_DIGEST_LENGTH];
    int i;

    MD5_Init(&c);
    MD5_Update(&c, "secret", 6);
    MD5_Update(&c, "data"
                   "\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x00\x00\x00\x00"
                   "\x50\x00\x00\x00\x00\x00\x00\x00"
                   "append", 64);
    MD5_Final(buffer, &c);

    for (i = 0; i < 16; i++) {
      printf("%02x", buffer[i]);
    }
    printf("\n");
    return 0;
  }
