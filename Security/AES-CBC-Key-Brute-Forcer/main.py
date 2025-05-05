from Crypto.Cipher import AES
from Crypto.Util.Padding import pad

# Plaintext
pt = b"This is a top secret."
padded_pt = pad(pt, 16)

# Ciphertext (in hex)
ct_hex = "8d20e5056a8d24d0462ce74e4904c1b513e10d1df4a2ef2ad4540fae1ca0aaf9"

# Convert to bytes
ct = bytes.fromhex(ct_hex)

# IV initialized to all zeros
iv = bytes([0] * 16)


with open("words.txt", "r") as f:
    for line in f:
        word = line.strip()
        if len(word) > 16 or not word:
            continue

        # Pad key to length 16 using spaces
        key = (word + ' ' * (16 - len(word))).encode()

        # Create new CBJ object using AES
        cipher = AES.new(key=key, mode=AES.MODE_CBC, iv=iv)
        guess = cipher.encrypt(padded_pt)
        if guess == ct:
            print("Key found:",key.decode())
            exit(0)

print("Key not found!")
