#!/usr/bin/env python3
"""Fetch a simple text dataset for Melvin."""

from datasets import load_dataset

# Load a small, simple dataset
print("Loading tiny_shakespeare dataset...")
ds = load_dataset("tiny_shakespeare", split="train")

# Save just the text to a file
with open("data/william_shakespeare.txt", "w") as f:
    for example in ds:
        f.write(example['text'])

print(f"Saved dataset to data/william_shakespeare.txt")
print(f"Total examples: {len(ds)}")

