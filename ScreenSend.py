# -*- coding: utf-8 -*-
"""
Created on Sun Aug 30 10:56:03 2020

@author: Sarthak
"""

from tkinter import *
from tkinter.colorchooser import askcolor
import serial
import struct

mySer = ""
class Paint(object):

    DEFAULT_PEN_SIZE = 5.0
    DEFAULT_COLOR = 'black'

    def __init__(self):
        self.root = Tk()

        self.reset_button = Button(self.root, text='Reset', command=self.do_reset)
        self.reset_button.grid(row=0, column=1)

        self.c = Canvas(self.root, bg='white', width=320, height=240)
        self.c.grid(row=1, columnspan=5)

        self.setup()
        self.root.mainloop()

    def setup(self):
        self.old_x = None
        self.old_y = None
        self.line_width = 2
        self.color = self.DEFAULT_COLOR
        self.active_button = self.reset_button
        self.c.bind('<B1-Motion>', self.paint)
        self.c.bind('<ButtonRelease-1>', self.clean)


    def paint(self, event):
        self.line_width = 1
        paint_color = 'black'
        if self.old_x and self.old_y:
            buf = "%d,%d\r\n" % (event.x, event.y)
            mySer.write(buf.encode())
            self.c.create_line(self.old_x, self.old_y, event.x, event.y,
                               width=self.line_width, fill=paint_color,
                               capstyle=ROUND, smooth=TRUE, splinesteps=36)
            
            print("{},{}".format(event.x,event.y))
            
        self.old_x = event.x
        self.old_y = event.y

    def do_reset(self,event):
        self.c.delete("all")
        buf = "1024,1024\r\n"
        mySer.write(buf.encode())

    def clean(self,event):
        self.old_x, self.old_y = None, None


if __name__ == '__main__':
    mySer = serial.Serial('COM6',baudrate=115200)
    
    Paint()