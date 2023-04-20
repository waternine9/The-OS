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
filemanicon = pygame.image.load("filemanicon.png")

# Load the background
backgroundsurf = pygame.image.load("background.bmp")

# Create a surface to render the characters
surface = pygame.Surface((1920, 1080), pygame.SRCALPHA)

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

        # Clear the surface
    surface.fill((0, 0, 0, 0))

    # Draw the icon on the surface
    surface.blit(filemanicon, (0, 0))

    # Write the color values to the output file
    for y in range(32):
        for x in range(32):
            color = surface.get_at((x, y))


            output.write(bytes([color.r, color.g, color.b, color.a]))

with open("font.bin", "wb") as output:

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

with open("background.bin", "wb") as output:
        # Clear the surface
        surface.fill((0, 0, 0, 0))

        scaled_bg = pygame.transform.smoothscale(backgroundsurf, (1920, 1080))
        scaled_bg = pygame.transform.flip(scaled_bg, False, True)
        # Draw the scaled character on the surface
        surface.blit(backgroundsurf, (0, 0))

        # Write the alpha values to the output file
        for y in range(1080):
            for x in range(1920):
                color = surface.get_at((x, y))
                output.write(bytes([color.b, color.g, color.r, color.a]))

with open("background.bin", "rb") as _bg:
    
    with open("font.bin", "rb") as _font:
        with open("sysicons.bin", "rb") as _icons:
            with open("resources.bin", "wb") as out:
                out.write(_icons.read() + _font.read() + _bg.read())
        
# Quit Pygame
pygame.quit()
