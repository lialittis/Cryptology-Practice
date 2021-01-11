"""
From http://aandds.com/blog/password-hashing.html

In this case, the salt and password is "message" and "root":
	md5(salt || password)=md5(messageroot)=f3c36e01c874865bc081e4ae7af037ea
so, $ python login.py root f3c36e01c874865bc081e4ae7af037ea will return "Login Successful!"

If the hacker knows the hash value, it could attack by extended length attack.

For example:
python login.py %80%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%00%38%00%00%00%00%00%00%00admin e53a681a30ff99e3f6522270ca7db244

++NOT WORKING++


"""



import hashlib
import sys
from urllib.request import unquote

def login(password, hash_val):
    m = hashlib.md5()
    secret_key = "message"               # secret_key is a salt
    m.update((secret_key + password).encode("utf-8"))
    if(m.hexdigest() == hash_val):
        print("Login Successful!")
    else:
        print("Login Failed")

if __name__ == "__main__":
    password = unquote(sys.argv[1])
    hash_val = unquote(sys.argv[2])
    login(password, hash_val)
