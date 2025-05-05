# 🔁 Hash Collision Simulator

## Description

**Hash Collision Simulator** is a C program that demonstrates how many attempts it takes to find weak and strong collisions using truncated SHA-256 hashes (first 3 bytes).  
It simulates two types of collisions:
- **Weak collision**: Find a message that collides with a fixed target.
- **Strong collision**: Find any two distinct messages that collide.
---

## Requirements

- GCC or another C compiler  
- OpenSSL development libraries

### Install dependencies (Ubuntu/Debian):

```bash
sudo apt update
sudo apt install build-essential libssl-dev
```
## How To Run
1. Compile the program using the provided Makefile:
```bash
make
```
2. Run a single trial:
```bash
python main.py
```
3. Run 100 trials and compute average attempts:
```bash
bash script.bash
```

