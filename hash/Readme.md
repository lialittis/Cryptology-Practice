# REAdME

## example: hash function for login and signature

from https://blog.skullsecurity.org/2012/everything-you-need-to-know-about-hash-length-extension-attacks

let secret = "secret"
let data = "data"
let H = md5()
let signature = hash(secret || data) = 6036708eba0d11f6ef52ad44e8b74d5b
let append = "append"

第一种方法是服务器将执行的操作，第二种方法是攻击者将执行的操作【攻击者也没办法使用第一种方法进行计算，因为不知道secret的值】

  0000  73 65 63 72 65 74 64 61 74 61 80 00 00 00 00 00  secretdata......
  0010  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
  0020  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
  0030  00 00 00 00 00 00 00 00 50 00 00 00 00 00 00 00  ........P.......


Breaking down the string, we have:

"secret" = secret
"data" = data
80 00 00 ... — The 46 bytes of padding, starting with 0x80
50 00 00 00 00 00 00 00 — The bit length in little endian

### The attack

Now that we have the data that H hashes, let's look at how to perform the actual attack.

First, let's just append append to the string. Easy enough! Here's what it looks like:

  0000  73 65 63 72 65 74 64 61 74 61 80 00 00 00 00 00  secretdata......
  0010  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
  0020  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
  0030  00 00 00 00 00 00 00 00 50 00 00 00 00 00 00 00  ........P.......
  0040  61 70 70 65 6e 64                                append

Server's calculation
We know the server will prepend secret to the string, so we send it the string minus the secret value:

  0000  64 61 74 61 80 00 00 00 00 00 00 00 00 00 00 00  data............
  0010  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
  0020  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
  0030  00 00 50 00 00 00 00 00 00 00 61 70 70 65 6e 64  ..P.......append

The server will prepend secret to that string, creating:

  0000  73 65 63 72 65 74 64 61 74 61 80 00 00 00 00 00  secretdata......
  0010  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
  0020  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
  0030  00 00 00 00 00 00 00 00 50 00 00 00 00 00 00 00  ........P.......
  0040  61 70 70 65 6e 64                                append


And hashes it to the following value:

  6ee582a1669ce442f3719c47430dadee

For those of you playing along at home, you can prove this works by copying and pasting this into a terminal:

`shell
  echo '
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
  }' > hash_extension_1.c

  gcc -o hash_extension_1 hash_extension_1.c -lssl -lcrypto

  ./hash_extension_1
`

All right, so the server is going to be checking the data we send against the signature 6ee582a1669ce442f3719c47430dadee. Now, as the attacker, we need to figure out how to generate that signature!

Client's calculation
So, how do we calculate the hash of the data shown above without actually having access to secret?

Well, first, we need to look at what we have to work with: data, append, H, and H(secret || data).

We need to define a new function, H′, which uses the same hashing algorithm as H, but whose starting state is the final state of H(secret || data), i.e., signature. Once we have that, we simply calculate H′(append) and the output of that function is our hash. It sounds easy (and is!); have a look at this code:
`shell
  echo '
  #include <stdio.h>
  #include <openssl/md5.h>

  int main(int argc, const char *argv[])
  {
    int i;
    unsigned char buffer[MD5_DIGEST_LENGTH];
    MD5_CTX c;

    MD5_Init(&c);
    MD5_Update(&c, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", 64);

    c.A = htonl(0x6036708e); /* <-- This is the hash we already had */
    c.B = htonl(0xba0d11f6);
    c.C = htonl(0xef52ad44);
    c.D = htonl(0xe8b74d5b);

    MD5_Update(&c, "append", 6); /* This is the appended data. */
    MD5_Final(buffer, &c);
    for (i = 0; i < 16; i++) {
      printf("%02x", buffer[i]);
    }
    printf("\n");
    return 0;
  }' > hash_extension_2.c

  gcc -o hash_extension_2 hash_extension_2.c -lssl -lcrypto

  ./hash_extension_2
`


So we know the signature is right. The difference is, we didn't use secret at all! What's happening!?

Well, we create a MD5_CTX structure from scratch, just like normal. Then we take the MD5 of 64 'A's. We take the MD5 of a full (64-byte) block of 'A's to ensure that any internal values — other than the state of the hash itself — are set to what we expect.

Then, after that is done, we replace c.A, c.B, c.C, and c.D with the values that were found in signature: 6036708eba0d11f6ef52ad44e8b74d5b. This puts the MD5_CTX structure in the same state as it finished in originally, and means that anything else we hash — in this case append — will produce the same output as it would have had we hashed it the usual way.

We use htonl() on the values before setting the state variables because MD5 — being little-endian — outputs its values in little-endian as well.

Result
So, now we have this string:

  0000  64 61 74 61 80 00 00 00 00 00 00 00 00 00 00 00  data............
  0010  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
  0020  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
  0030  00 00 50 00 00 00 00 00 00 00 61 70 70 65 6e 64  ..P.......append
And this signature for H(secret || data || append):

  6ee582a1669ce442f3719c47430dadee




[!](https://github.com/iagox86/hash_extender）







