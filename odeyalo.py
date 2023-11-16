import tkinter as tk
from tkinter import *
from tkinter import messagebox
from tkinter import ttk

from PIL import Image, ImageTk
from PIL.ImageColor import getrgb

import os
from copy import copy
import math


list_of_colors = [
    getrgb('#B22222'),
    getrgb('#6495ED'),
    getrgb('#BDB76B'),
    getrgb('#008000'),
    getrgb('#32CD32'),
    getrgb('#4B0082'),
    getrgb('#FF7F50'),
    getrgb('#FFD700'),
    getrgb('#FFFFF0'),
    getrgb('#00008B'),
    getrgb('#FF8C00'),
    getrgb('#D2691E'),
    getrgb('#E6E6FA'),
    getrgb('#006400'),
    getrgb('#8B008B'),
    getrgb('#1E90FF'),
    getrgb('#3CB371'),
    getrgb('#800000'),
    getrgb('#DDA0DD'),
    getrgb('#3CB371'),
    getrgb('#FAEBD7'),
]

def calc_colors(n: int):
    if n < 20:
        return
    assert(n < 255)
    a = int(n**(1/3)) + 1
    for i in range(a):
        for j in range(a):
            for k in range(a):
                list_of_colors.append((i * 255 // a, j * 255 // a, k * 255 // a))


def calculate_blanket(rows: int, cols: int, n_colors: int, colors: list):
    com_str = f'./Odeyalo {str(rows)} {str(cols)} {str(n_colors)} {" ".join(map(str, colors))}'
    print(com_str)
    text = os.popen(com_str).read()
    print(text)

    def text_gen_f():
        for line in text.split('\n'):
            yield line

    text_gen = text_gen_f()

    blanket0 = []
    stat = ""
    blanket = [] 
    stat0 = ""

    for i in range(rows):
        txt = next(text_gen)
        if txt == "It is impossible to color blanket with such a colors":
            raise ValueError(txt)
        blanket0.append(list(map(int, txt.split())))

    stat0 = f'{next(text_gen)} {next(text_gen)} {next(text_gen)}' 

    for i in range(rows):
        blanket.append(list(map(int, next(text_gen).split())))

    stat = f'{next(text_gen)} {next(text_gen)} {next(text_gen)}'
        
    return blanket0, stat0, blanket, stat


def matrix_to_imag(matrix: list):
    m = len(matrix)
    n = len(matrix[0])
    width = 50 * n
    height = 50 * m
    img = Image.new(mode = "RGB", size = (width, height))
    for w in range(width):
        for h in range(height):
            img.putpixel((w, h), list_of_colors[matrix[w // 50][h // 50]])
    return img


def window():
    window = Tk()
    window.title("Odeyalo")
    window.geometry('400x400')
    frame = Frame(
        window, #Обязательный параметр, который указывает окно для размещения Frame.
        padx = 10, #Задаём отступ по горизонтали.
        pady = 10 #Задаём отступ по вертикали.
    )
    frame.pack(expand=True)

    style = ttk.Style()
    style.configure(
        "BW.TLabel",
        foreground="black",
        font="TimesNewRoman",
        fontsize=20
    )

    rows_lb = ttk.Label(
        frame,
        text="Enter number of rows",
        style="BW.TLabel"
    )
    rows_lb.grid(row=3, column=1)

    rows_tf = Entry(
        frame,
    )
    rows_tf.grid(row=3, column=2)

    cols_lb = ttk.Label(
        frame,
        text="Enter number of cols",
        style="BW.TLabel"
    )
    cols_lb.grid(row=4, column=1)

    cols_tf = Entry(
        frame,
    )
    cols_tf.grid(row=4, column=2)

    cols_lb = ttk.Label(
        frame,
        text="Enter number of colors",
        style="BW.TLabel"
    )
    cols_lb.grid(row=5, column=1)

    colors_num_tf = Entry(
        frame,
    )
    colors_num_tf.grid(row=5, column=2)

    colors_lb = ttk.Label(
        frame,
        text="Enter colors",
        style="BW.TLabel"
    )
    colors_lb.grid(row=6, column=1)

    colors_tf = Entry(
        frame,
    )
    colors_tf.grid(row=6, column=2)

    def get_info():
        try:
            rows = int(rows_tf.get())
            cols = int(cols_tf.get())
            n_colors = int(colors_num_tf.get())
            colors = list(map(int, colors_tf.get().split()))
            assert(len(colors) == n_colors)

            calc_colors(n_colors)

            blanket0, stat0, blanket, stat = calculate_blanket(rows, cols, n_colors, colors)

            new_window_1 = Toplevel(window)
            new_window_1.title("First iter")
            new_window_1.resizable(0, 0)
            new_window_1.wm_attributes("-topmost", 1)
            canvas_1 = Canvas(new_window_1, width=400, height=400, 
                bg="black", highlightthickness=0)
            canvas_1.pack()
            tkimage0 = ImageTk.PhotoImage(matrix_to_imag(blanket0))
            canvas_obj = canvas_1.create_image(
                0, 1, 
                image=tkimage0,
                anchor="nw"
            )
            
            new_window_1 = Toplevel(window)
            new_window_1.title("Last iter")
            new_window_1.resizable(0, 0)
            new_window_1.wm_attributes("-topmost", 1)
            canvas_1 = Canvas(new_window_1, width=400, height=400, 
                bg="black", highlightthickness=0)
            canvas_1.pack()
            tkimage1 = ImageTk.PhotoImage(matrix_to_imag(blanket))
            canvas_obj = canvas_1.create_image(
                0, 0, 
                image=tkimage1,
                anchor="nw"
            )

            messagebox.showinfo('Stat', f'First iter:\n{stat0}\nLastiter:\n{stat}')
        except AssertionError:
            messagebox.showinfo('Error', f'Wrong colors')
        except ValueError as e:
            messagebox.showinfo('Error', f'Input Value Error: {e}')


    cal_btn = Button(
        frame,
        text='Calculate the blanket',
        command=get_info,
    )
    cal_btn.grid(row=7, column=2)

    window.mainloop()


if __name__ == "__main__":
    window()
    
