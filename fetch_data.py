#!/usr/bin/env python3
"""Fetch text data for Melvin from HuggingFace."""

from datasets import load_dataset
import os

print("Fetching wikitext-2 dataset...")

# Create data directory
os.makedirs("data", exist_ok=True)

try:
    # Try wikitext which is popular and simple
    ds = load_dataset("wikitext", "wikitext-2-raw-v1", split="train")
    
    # Save the text
    with open("data/wikitext.txt", "w", encoding="utf-8") as f:
        for example in ds[:1000]:  # First 1000 examples
            if example['text'] and len(example['text'].strip()) > 0:
                f.write(example['text'] + "\n")
    
    print("Saved to data/wikitext.txt")
    print(f"Examples processed: 1000")
except Exception as e:
    print(f"Error: {e}")
    print("Creating a simple test file instead...")
    
    # Create a simple test file
    with open("data/simple_test.txt", "w") as f:
        f.write("This is a test of Melvin's cognitive architecture.\n")
        f.write("Melvin processes text by creating graph nodes for each character.\n")
        f.write("Connections form when nodes appear together in time.\n")
        f.write("Patterns emerge through activation and coherence.\n")
    
    print("Created data/simple_test.txt")
