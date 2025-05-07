'''
File Name: bloom.py
Date: 5/03/25
Description: 
    This script builds a Bloom Filter to check for weak passwords.
    It loads bad passwords from rockyou.txt, then goes through each word in dictionary.txt to see if it might be in that set.
    If its a maybe, we write "maybe" to the output, if not, we write "no".
    Everything gets saved to output.txt.
'''

import math
import mmh3
from bitarray import bitarray


# Bloom filter class
class BloomFilter(object):
    def __init__(self, items_count, fp_prob):
        '''
        items_count : int
            Number of items expected to be stored in bloom filter
        fp_prob : float
            False Positive probability in decimal
        '''
        # False possible
        self.fp_prob = fp_prob

        # Size of array
        self.size = self.get_size(items_count, fp_prob)

        # Number of hashes
        self.hash_count = self.get_hash_count(self.size, items_count)

        # Bit array
        self.bit_array = bitarray(self.size)

        # Set all bits to 0
        self.bit_array.setall(0)

    '''
    Name: init_hash
    Description:
        Adds an item to the filter
    '''

    def init_hash(self, item):
        '''

        '''
        # create hashes for given item.
        hashes = []
        for i in range(self.hash_count):
            # Seed each hash with i to create different output
            hash = mmh3.hash(item, i) % self.size
            hashes.append(hash)
        return hashes

    '''
    Name: insert_word
    Description:
        Inserts a word to Filter.
    Parameters:
        - word (str): Word to insert.
    '''
    def insert_item(self, item):
        for index in self.init_hash(item):
            self.bit_array[index] = 1

    '''
        Check for existence of an item in filter
        '''
        # if any of bit is 0 then, no
        # else there is probability that it exist (0)
    def check_item(self, item):
        for i in range(self.hash_count):
            hash = mmh3.hash(item, i) % self.size
            if self.bit_array[hash] == 0:
                return "no"
        return "maybe"
    

    '''
    Name: get_size
    Description:
        Computes the hash count based on the bloom filter calculator
    Parameters:
        n : int
            number of items expected to be stored in filter
        p : float
            False Positive probability in decimal
    '''
    @classmethod
    def get_size(self, n, p):
        m = -(n * math.log(p))/(math.log(2)**2)
        return int(m)

    '''
    Name: get_hash_count
    Description:
        Computes the hash count based on the bloom filter calculator
    Parameters:
        m : int
            size of bit array
        n : int
            number of items expected to be stored in filter
    '''
    @classmethod
    def get_hash_count(self, m, n):
        k = (m/n) * math.log(2)
        return int(k)


'''
Name: count_lines
Description:
    Loads the rock you, then tests the dictionary and prints to console
Parameters:
    None
'''
def get_input_size(path):
    count = 0
    with open(path, encoding='latin-1') as f:
        for line in f:
            if line.strip():
                count += 1
    return count

'''
Name: load_input
Description:
    
Parameters:
    None
'''
def load_input(bloom, path):
    with open(path, encoding='latin-1') as f:
        for line in f:
            word = line.strip()
            if word:
                bloom.insert_item(word)

'''
Name: test_input
Description:
    
Parameters:
    None
'''
def test_input(bloom, path):
    results = []
    with open(path, encoding='latin-1') as f:
        for line in f:
            word = line.strip()
            if word:
                results.append(bloom.check_item(word))
    return results


def compute_stats(bloom, dictionary_path, rockyou_path):
    bad_passwords = set()
    with open(rockyou_path, encoding='latin-1') as f:
        for line in f:
            bad_passwords.add(line.strip())

    tp = tn = fp = fn = 0

    with open(dictionary_path, encoding='latin-1') as f:
        for line in f:
            word = line.strip()
            if not word:
                continue

            in_rockyou = word in bad_passwords
            result = bloom.check_item(word)

            if result == "maybe" and in_rockyou:
                tp += 1
            elif result == "no" and not in_rockyou:
                tn += 1
            elif result == "maybe" and not in_rockyou:
                fp += 1
            elif result == "no" and in_rockyou:
                fn += 1

    print("\n===== Statistics =====")
    print(f"True Positives:  {tp}")
    print(f"True Negatives:  {tn}")
    print(f"False Positives: {fp}")
    print(f"False Negatives: {fn}")
    
'''
Name: main
Description:
    Loads the rock you, then tests the dictionary and prints to console
Parameters:
    None
'''
def main():
    # Input paths
    load_file = "rockyou.ISO-8859-1.txt"
    test_file = "dictionary.txt"

    # Get amount of items to store in filter
    expected_items = get_input_size(load_file)

    # False positive probability rate
    fp_rate = 0.05

    bloom = BloomFilter(expected_items, fp_rate)

    
    load_input(bloom, load_file)
    results = test_input(bloom, test_file)
    # Print results
    # print(bloom.size) # To get size of bit array
    # print(expected_items) # To get amount of items
    # print(bloom.hash_count) # To get amount of hashes
    for result in results:
        print(result)
    compute_stats(bloom, "dictionary.txt", "rockyou.ISO-8859-1.txt")

if __name__ == "__main__":
    main()
