# Bloom Filter

# Description
This script builds a Bloom Filter to check for weak passwords. It loads bad passwords from rockyou.txt, then goes through each word in dictionary.txt to see if it might be in that set. If its a maybe, we write "maybe" to the output, if not, we write "no".
## Requirements
- You need both the murmurhash and bitarray libaries

```bash
pip install bitarray
```

```bash
pip install mmh3
```

## How to Compile

### 1. Connect to Flip server

### 2. Install the dependencies

### 3. Run the python script
- It should print to the terminal
```bash
python bloom.py
```

