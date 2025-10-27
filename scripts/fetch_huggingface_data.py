#!/usr/bin/env python3
"""
Fetch text datasets from Hugging Face and convert them for Melvin's intake.
"""

import datasets
from datasets import load_dataset
import json
import os

def fetch_text_dataset(dataset_name, split="train", max_examples=10000):
    """
    Fetch a text dataset from HuggingFace and save as JSON lines.
    
    Args:
        dataset_name: Name of the HuggingFace dataset
        split: Which split to use (train, validation, test)
        max_examples: Maximum number of examples to fetch
    """
    print(f"Loading dataset: {dataset_name}")
    
    try:
        dataset = load_dataset(dataset_name, split=split)
    except Exception as e:
        print(f"Error loading dataset: {e}")
        return None
    
    # Get text field (may be called 'text', 'sentence', 'content', etc.)
    text_fields = ['text', 'sentence', 'content', 'document', 'inputs', 'input']
    
    text_field = None
    for field in text_fields:
        if field in dataset.column_names and isinstance(dataset[0][field], str):
            text_field = field
            break
    
    if not text_field:
        # Try first column that's a string
        for col in dataset.column_names:
            if isinstance(dataset[0][col], str) and len(dataset[0][col]) > 10:
                text_field = col
                break
    
    if not text_field:
        print("Could not find text field in dataset")
        return None
    
    print(f"Using field: {text_field}")
    
    # Create output directory
    output_dir = "data/huggingface"
    os.makedirs(output_dir, exist_ok=True)
    
    # Save as JSON lines
    output_file = os.path.join(output_dir, f"{dataset_name.replace('/', '_')}_{split}.jsonl")
    
    print(f"Writing to: {output_file}")
    
    with open(output_file, 'w') as f:
        count = 0
        for item in dataset:
            if count >= max_examples:
                break
            
            text = item[text_field]
            if text and len(text.strip()) > 0:
                f.write(json.dumps({"text": text}) + '\n')
                count += 1
            
            if count % 1000 == 0:
                print(f"  Processed {count} examples")
    
    print(f"Saved {count} examples to {output_file}")
    return output_file

def main():
    """Fetch multiple popular text datasets."""
    
    datasets_to_fetch = [
        ("wikitext", "wikitext-103-v1"),  # Wikipedia text
        ("openwebtext",),  # Open source web text
        ("c4", "en"),  # Colossal Clean Crawled Corpus
        ("bookcorpus",),  # Book corpus
        ("imdb",),  # Movie reviews
        ("squad"),  # Question answering
        ("ag_news"),  # News articles
    ]
    
    print("Fetching text datasets from HuggingFace...")
    print("This may take a while for large datasets.\n")
    
    for ds_info in datasets_to_fetch:
        dataset_name = ds_info[0]
        config = ds_info[1] if len(ds_info) > 1 else None
        
        try:
            if config:
                full_name = f"{dataset_name}_{config}"
                fetch_text_dataset(dataset_name, config_name=config)
            else:
                fetch_text_dataset(dataset_name)
        except Exception as e:
            print(f"Failed to fetch {dataset_name}: {e}")
            continue
        
        print()
    
    print("Done!")

if __name__ == "__main__":
    main()

