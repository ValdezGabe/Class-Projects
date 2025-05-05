#!/bin/bash

total_weak=0
total_strong=0
trials=100

echo "Trial #  Weak Attempts    Strong Attempts"
echo "-----------------------------------------"

for ((i = 1; i <= trials; i++)); do
    output=$(./main)
    
    weak=$(echo "$output" | awk '{print $2}')
    strong=$(echo "$output" | awk '{print $4}')

    echo "$i           $weak           $strong"

    total_weak=$((total_weak + weak))
    total_strong=$((total_strong + strong))
done

avg_weak=$(echo "scale=2; $total_weak / $trials" | bc)
avg_strong=$(echo "scale=2; $total_strong / $trials" | bc)

echo
echo "================== Averages =================="
echo "Average Weak Attempts:   $avg_weak"
echo "Average Strong Attempts: $avg_strong"