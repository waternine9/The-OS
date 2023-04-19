import pygame
import sys

# Initialize Pygame
pygame.init()

# Constants
FONT_FILE = "src/fonts/ApercuMonoProLight.ttf"  # Replace with the path to your font file
FONT_SIZE = 32
OUTPUT_FILE = "font.bin"
CHARACTERS = [chr(i) for i in range(32, 127)]  # ASCII characters from 32 to 126

# Load the font
font = pygame.font.Font(FONT_FILE, FONT_SIZE)

# Load sys icons
cmdicon = pygame.image.load("cmdicon.png")
txticon = pygame.image.load("txticon.png")

# Create a surface to render the characters
surface = pygame.Surface((32, 32), pygame.SRCALPHA)

with open("sysicons.bin", "wb") as output:

    # Clear the surface
    surface.fill((0, 0, 0, 0))

    # Draw the icon on the surface
    surface.blit(cmdicon, (0, 0))

    # Write the color values to the output file
    for y in range(32):
        for x in range(32):
            color = surface.get_at((x, y))


            output.write(bytes([color.r, color.g, color.b, color.a]))

        # Clear the surface
    surface.fill((0, 0, 0, 0))

    # Draw the icon on the surface
    surface.blit(txticon, (0, 0))

    # Write the color values to the output file
    for y in range(32):
        for x in range(32):
            color = surface.get_at((x, y))


            output.write(bytes([color.r, color.g, color.b, color.a]))

# Open the output file
with open(OUTPUT_FILE, "wb") as output:

    # Iterate through the ASCII characters
    for char in CHARACTERS:

        # Clear the surface
        surface.fill((0, 0, 0, 0))

        # Render the character with antialiasing
        text = font.render(char, True, (255, 255, 255))

        # Scale the character to 32x32
        scaled_text = pygame.transform.smoothscale(text, (32, 32))

        # Draw the scaled character on the surface
        surface.blit(scaled_text, (0, 0))

        # Write the alpha values to the output file
        for y in range(32):
            for x in range(32):
                alpha = surface.get_at((x, y)).a
                output.write(bytes([alpha]))

with open("background.bmp", "rb") as _bg:

    with open("font.bin", "rb") as _font:
        with open("sysicons.bin", "rb") as _icons:
            with open("resources.bin", "wb") as out:
                out.write(_icons.read() + _font.read() + _bg.read())
        
# Quit Pygame
pygame.quit()