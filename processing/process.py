#!/usr/bin/env python3

from PIL import Image

# From: https://stackoverflow.com/questions/17778258/image-does-not-open-with-python-imaging-library
import os.path
script_dir = os.path.dirname(os.path.abspath(__file__))

def print_bits(bit0, bit1):
    print("Print bits")
    print("bit0:")
    for i in range(len(bit0)):
        print("{0:08b}".format(bit0[i]))

    print("\n\nbit1:")
    for i in range(len(bit1)):
        print("{0:08b}".format(bit1[i]))

def process(name):
    image_path = os.path.join(script_dir, name + '.png')
    if not os.path.exists(image_path):
        print("Asset {} does not exists".format(name))
        return
    img = Image.open(image_path)
    pixels = img.load()
    color_to_idx = {}
    bit0 = [0] * 8
    bit1 = [0] * 8
    num_colors = 1
    for i in range(img.width):
        for j in range(img.height):
            color = pixels[i, j]
            index = 0
            if color == (255, 255, 255):
                index = 0
            elif color in color_to_idx:
                index = color_to_idx[color]
            else:
                index = num_colors
                color_to_idx[color] = index
                num_colors += 1
            if index & 1:
                bit0[j] = (bit0[j]) | (1 << i)
            if (index >> 1) & 1:
                bit1[j] = (bit1[j]) | (1 << i)
            # print(pixels[i, j])
            # print('Index: {}'.format(index))
            # print("{0:b}".format(bit0[j]))
            # print('\n')

    print_bits(bit0, bit1)

    if num_colors > 4:
        print("[ERROR] More than 3 colors")

    # From: https://stackoverflow.com/questions/21220916/writing-bits-to-a-binary-file
    output_path = os.path.join(script_dir, '../dist/',name + '.tile')
    with open(output_path, 'wb') as f:
        for bitarray in bit0[::-1]:
            f.write(int(bitarray).to_bytes(1, 'little'))
        for bitarray in bit1[::-1]:
            f.write(int(bitarray).to_bytes(1, 'little'))
    print("Output {} to {}".format(name, output_path))

if __name__ == "__main__":
    print("Processing...")
    assets = []
    assets.append('player2')
    assets.append('death')
    assets.append('ball')
    assets.append('back')
    assets.append('brick')
    for asset in assets:
        process(asset)
