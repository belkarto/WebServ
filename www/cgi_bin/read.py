import os
import sys

# Get the content length from the environment variables
content_length_str = os.environ.get('CONTENT_LENGTH')

print("this is test for post cgi in python")

if content_length_str is not None:
    try:
        # Convert content length to an integer
        content_length = int(content_length_str)
        
        # Read the specified number of bytes from stdin
        content = sys.stdin.read(content_length)

        # Print the content
        print("readed from stdin", content)

    except ValueError:
        print("Invalid CONTENT_LENGTH value")
else:
    print("CONTENT_LENGTH not set in environment variables")
