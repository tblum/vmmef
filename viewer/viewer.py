#!/usr/bin/python

from __future__ import print_function
import sys
import numpy as np
import matplotlib.pyplot as plt
import h5py as h5

class H5file:

    pedestal = {}
    
    def __init__(self, file_name):
        self.file_name = file_name
        try:
            self.file = h5.File(self.file_name, 'r')
            self.images = self.file['images']
        except IOError:
            print ('Could not open: ', self.file_name)
            return False

    def readPedestal(self):
        try:
            self.pedestal['avg'] = self.file['pedestal/average'][:]
            self.pedestal['cut'] = self.file['pedestal/cut-off'][:]
        except KeyError:
            print ('The file ', self.file_name, 'does not contain a pedestal')
            return False

    def readEvent(self, i):
        event_str = str(i).zfill(7)
        try:
            event = self.images[event_str][:]
        except KeyError:
            print ('The file ', self.file_name, 'does not contain event', event_str)
            return False
        return event
        

class Display:

    evno = 0
    ped = None
    zero = False
    surface = False
    
    def __init__(self, h5file):
        self.figure, self.axes = plt.subplots(2)
        plt.subplots_adjust(bottom=0.1, right=0.8, top=0.9)
        self.axes = np.append(self.axes,[plt.axes([0.85, 0.1, 0.075, 0.8])])
        self.figure.canvas.mpl_connect('key_press_event', self.keyEvent)
        self.h5file = h5file
        self.showEvent(0)
        plt.show()
        
    def showEvent(self, i=None):
        if i is None:
            i = self.evno
        event = h5file.readEvent(i).astype(np.int16)
        if not self.ped is None:
            event -= self.ped[:,None,:]
        if self.zero:
            event *= event > 0
            
        self.figure.canvas.set_window_title(str(i).zfill(7))
        min = np.min(event)
        max = np.max(event)
        for a in range(len(self.axes)):
            self.axes[a].cla()
        self.axes[0].title.set_text('X')
        self.axes[1].title.set_text('Y')
        im = self.axes[0].imshow(event[0], aspect='auto', clim=(min,max))
        im = self.axes[1].imshow(event[1], aspect='auto', clim=(min,max))
        plt.colorbar(im, cax=self.axes[2])
        plt.draw()

    def keyEvent(self, event):
        sys.stdout.flush()
        if event.key == 'right':
            self.evno += 1
            self.showEvent()
        if event.key == 'left':
            self.evno -= 1
            self.showEvent()
        if event.key == 'c':
            self.ped = self.h5file.pedestal['cut'].copy()
            self.showEvent()
        if event.key == 'a':
            self.ped = self.h5file.pedestal['avg'].copy()
            self.showEvent()
        if event.key == 'n':
            self.ped = None
            self.showEvent()
        if event.key == 'z':
            self.zero = not self.zero
            self.showEvent()
        if event.key == '+':
            self.ped += 1
            self.showEvent()
        if event.key == '-':
            self.ped -= 1
            self.showEvent()
        
if __name__ == '__main__':
    h5file = H5file(sys.argv[1])
    h5file.readPedestal()
    display = Display(h5file)

    
