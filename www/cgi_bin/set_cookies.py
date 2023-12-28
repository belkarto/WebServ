#!/usr/bin/env python3
import os
import sys
from urllib.parse import unquote_plus

def parse_query_string(query_string):
    return dict(token.split('=', 1) for token in query_string.split('&'))

def read_post_data(content_length):
    return sys.stdin.buffer.read(content_length).decode('utf-8')

def set_cookies(parsed_values):
    time_value = parsed_values['time']
    for key in ['name', 'email', 'age']:
        print(f"Set-Cookie: {key}={parsed_values[key]}; expires={time_value}; path=/")

def redirect_to_index():
    print("Location: index.php\n\n")

def main():
    content_length = os.getenv("CONTENT_LENGTH")

    if not content_length:
        sys.exit(1)

    length = int(content_length)
    post_data = unquote_plus(read_post_data(length))
    parsed_values = parse_query_string(post_data)

    # Access the values
    set_cookies(parsed_values)

    # Redirect to index.php
    redirect_to_index()

if __name__ == "__main__":
    main()
