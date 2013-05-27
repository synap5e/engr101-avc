#!/usr/bin/env python 
timeout = 500 
packet_header = [
0x7E, 						# delimiter
0x00, 0x11, 					# length
0x10, 0x00, 					# API ID
0x00, 0x13, 0xA2, 0x00, 0x40, 0x63, 0x47, 0x1C,	# destination 64 bit address - broadcast
0xFF, 0xFE,					# destination 16 bit address - broadcast
0x00,						# number of hops
0x01						# option
]

import sys

if not len(sys.argv) > 1 :
   print "usage %s <serial port>" % (sys.argv[0])
   sys.exit()

import serial
port = serial.Serial(port=sys.argv[1], baudrate=9600)


import pygame, copy, time
from pygame.locals import *


def crc(packet):
   crc = 0;
   for byte in packet[3:]:
      crc = (crc + byte) % 0xFFFF
   crc &= 0xFF
   crc = 0xFF - crc
   return crc
 
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
      packet = packet_header + [direction, power_l, power_r]
      packet += [crc(packet)]
      port.write(bytearray(packet))
      print packet
port.close()

