import pygame
import sys

# Initialize Pygame
pygame.init()

# Constants
FONT_FILE = "consolas/CONSOLA.TTF"  # Replace with the path to your font file
FONT_SIZE = 128
OUTPUT_FILE = "consolas.bin"
CHARACTERS = [chr(i) for i in range(32, 127)]  # ASCII characters from 32 to 126

# Load the font
font = pygame.font.Font(FONT_FILE, FONT_SIZE)

# Open the output file
with open(OUTPUT_FILE, "wb") as output:

    # Iterate through the ASCII characters
    for char in CHARACTERS:

        # Clear the surface

        # Render the character with antialiasing
        text = font.render(char, True, (255, 255, 255))

        # Scale the character to 32x32
        scaled_text = pygame.transform.smoothscale(text, (32, 32))

        # Write the alpha values to the output file
        for y in range(32):
            for x in range(32):
                alpha = scaled_text.get_at((x, y)).a
                output.write(bytes([alpha]))
# Quit Pygame
pygame.quit()

# File paths
first_file_path = 'consolas.bin'
second_file_path = '../../background.bmp'
output_file_path = '../../resources.img'

# Read the input file
with open(first_file_path, 'rb') as first_file:
    first_data = first_file.read()




# Append the input file data to the output file
with open(second_file_path, 'rb') as second_file:
    second_data = second_file.read()

second_data += first_data

with open(output_file_path, 'wb') as output_file:
    output_file.write(second_data)
