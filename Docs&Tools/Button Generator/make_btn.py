import sys
import wand
from wand.color import Color
from wand.image import Image
from wand.drawing import Drawing
from wand.compat import nested
from wand.display import display

text_colors = {"On": "187, 215, 228", "Off": "180, 210, 220", "Disabled": "215, 215, 215"}
border_colors = {"On": "28, 56, 72", "Off": "28, 50, 60"}


def generate_image(text, mode):
    text_col = text_colors[mode]
    with Drawing() as draw:
        with Image(width=320, height=64) as img:
            draw.font = '../../HoboStd.otf'

            draw.text_alignment = 'center'
            draw.font_size = 32
            draw.fill_color = Color(f'rgb({text_col})')
            draw.text(img.width // 2, img.height // 2 + 15, text)
            draw(img)
            img.save(filename='template/.main_text.png')


    if mode != "Disabled":
        border_col = border_colors[mode]
        with Drawing() as draw:
            with Image(width=320, height=64) as img:
                draw.font = '../../HoboStd.otf'

                draw.text_alignment = 'center'
                draw.font_size = 32
                draw.fill_color = Color(f'rgb({border_col})')
                draw.text(img.width // 2, img.height // 2 + 15, text)
                draw(img)
                img.blur(radius=3, sigma=2)
                img.save(filename='template/.highlight_text.png')

    fg = Image(filename='template/.main_text.png')
    hl = Image(filename='template/.highlight_text.png')
    bg = Image(filename=f'template/{mode}.png')

    filename = text.replace(" ", "")

    with Drawing() as draw:
        if mode != "Disabled":
            for _ in range(3):
                draw.composite(operator='atop', left=0, top=0, width=hl.width, height=hl.height, image=hl)
                draw(bg)
        draw.composite(operator='atop', left=0, top=0,
                       width=hl.width, height=hl.height, image=fg)
        draw(bg)
        bg.save(filename=f'{filename}{mode}.png')


text = sys.argv[1]
generate_image(text, "On")
generate_image(text, "Off")
generate_image(text, "Disabled")
