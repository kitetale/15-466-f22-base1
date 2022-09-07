# https://stackoverflow.com/questions/65645044/get-hex-color-code-coordinate-of-the-pixel-in-a-image

from PIL import Image

def getPatterns (file):
    img = Image.open(file)
    pixels = img.load()
    width, height = img.size
    colors = [(0xff, 0xab, 0x3a), # orange
              (0xff, 0xde, 0xb2), # light orange
              (0x20, 0x19, 0x19), # black
              (0x00, 0x00, 0x00)] # clear
    colors_body = [(0xff, 0xab, 0x3a), # orange
                   (0xff, 0xde, 0xb2), # light orange
                   (0x81, 0x4a, 0x00), # brown
                   (0x00, 0x00, 0x00)] #clear
    pattern = []

    for y in range(height):
        for x in range(width):
            r, g, b = pixels.getpixel((x,y))
            i, j = 0
            if (x>=8): i=1
            if (y>=8): j=1

            color = []
            if (i==0 and j==0): 
                color = colors #cat head
            else: 
                color = colors_body #cat body

            try:
                index = color.index((r,g,b))
            except:
                print("shouldn't happen but color not found")
                return []
            pattern.append(index)
    
    return pattern

def getBit0 (pattern):
    bit0s = ""
    for i in range(256):
        if (i%2==0):
            bit0s += "0"
        else:
            bit0s += "1"
    return bit0s


def getBit1 (pattern):
    bit1s = ""
    for i in range(256):
        if (i<2): 
            bits1s += "0"
        else: 
            bits1s += "1"
    return bit1s


def getBits (pattern):
    bit0s = getBit0(pattern)
    bit1s = getBit1(pattern)

    pattern_file = open("cat_pattern.txt", "w")
    n = pattern_file.write(bit0s)
    if (n<=0): print("just wrote 0 bit0s?")
    pattern_file.close()

    pattern_file = open("cat_pattern.txt", "a")
    n = pattern_file.write(bit1s)
    if (n<=0): print("just wrote 0 bit1s?")
    pattern_file.close()

getPatterns("catto.png")