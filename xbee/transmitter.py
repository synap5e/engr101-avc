#!/usr/bin/env python 
timeout = 500 

import sys

if not len(sys.argv) > 1 :
   print "usage %s <serial port>" % (sys.argv[0])
   sys.exit()

import serial
port = serial.Serial(port=sys.argv[1], baudrate=9600)


import pygame, copy, time
from pygame.locals import *
 
pygame.init()
screen = pygame.display.set_mode((640, 480))
pygame.display.set_caption('AVC')
 
done = False
keys_held = set()
last_keys_held = set()
lasttime = 0
while not done:
   for event in pygame.event.get():
      if (event.type == KEYDOWN):
         if (event.key == K_ESCAPE):
            done = True
         keys_held.add(event.key)
      elif (event.type == KEYUP):
         keys_held.remove(event.key)
   millis = int(round(time.time() * 1000))
   if not keys_held == last_keys_held or millis - lasttime > timeout:
      lasttime = millis
      last_keys_held = copy.deepcopy(keys_held)
      direction = 0;
      power_l = 255
      power_r = 255
      if K_q in keys_held:
         # left forward
         direction |= 1 << 1
      elif K_a in keys_held:
         # left back
         pass
      else:
         power_l = 0
      if K_e in keys_held:
         # right forward
         direction |= 1
      elif K_d in keys_held:
         # right back
         pass
      else:
         power_r = 0
      port.write(bytearray([direction, power_l, power_r]))
      print "%x %x %x" % (direction, power_l, power_r)
port.close()

