# 🔐 AES-CBC-Key-Brue-Forcer

## Description

This is a Python script that brute-forces AES-128 encryption keys in CBC mode using a known plaintext and ciphertext pair.  
It reads candidate keys from a wordlist, pads them to 16 bytes with spaces, and checks if encrypting the plaintext with each key produces the target ciphertext.  
Useful for cryptanalysis demos, CTF challenges, and educational exploration of weak key practices.

## Requirements

- Python 3.x  
- [`pycryptodome`](https://pypi.org/project/pycryptodome/)
- `words.txt` — file containing potential keys (one per line)

Install dependencies with:

```bash
pip install pycryptodome
