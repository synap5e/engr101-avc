#!/usr/bin/env python 
timeout = 500 
packet_header = [
0x7E, 						# delimiter
0x00, 0x11, 					# length
0x10, 0x00, 					
# API ID
0x00, 0x13, 0xA2, 0x00, 0x40, 0x7C, 0x73, 0xD2,	# destination 64 bit address - broadcast
0xFF, 0xFE,					# destination 16 bit address - broadcast
0x00,						# number of hops
0x01						# option
]

def crc(packet):
	crc = 0;
	for byte in packet[3:]:
		crc = (crc + byte) % 0xFFFF
	crc &= 0xFF
	crc = 0xFF - crc
	return crc

import sys

if not len(sys.argv) > 1 :
	print "usage %s <serial port>" % (sys.argv[0])
	sys.exit(-1)

import serial
port = serial.Serial(port=sys.argv[1], baudrate=9600)


import pygame, copy, time
from pygame.locals import *
 
pygame.joystick.init()
if not pygame.joystick.get_count():
	print "No joystick found"
	sys.exit(-2)

joystick = pygame.joystick.Joystick(0)
joystick.init()
 
pygame.init()
screen = pygame.display.set_mode((640, 480))
pygame.display.set_caption('AVC')

done = False
lasttime = 0
lastValues = [0, 0]
while not done:
	for event in pygame.event.get():
		if ((event.type == KEYDOWN and event.key == K_ESCAPE) or event.type == pygame.QUIT):
			done = True
	millis = int(round(time.time() * 1000))
	values = [round(joystick.get_axis(1), 1), round(joystick.get_axis(2), 1)]
	if not lastValues == values or millis - lasttime > timeout:
		lasttime = millis
		lastValues = copy.deepcopy(values)
		direction = 0
		power_l = int(255 * abs(values[1]))
		power_r = int(255 * abs(values[0]))
		if values[0] < -0.1:
			# left forward
			direction |= 1 << 3
		elif values[0] > 0.1:
			# left back
			pass
		else:
			power_r = 0
		if values[1] < -0.1:
			# right forward
			direction |= 1 << 4
		elif values[1] > 0.1:
			# right back
			pass
		else:
			power_l = 0
		payload = [direction, power_l, power_r]
		packet = packet_header + payload
		crcVal = crc(packet)
		packet += [crcVal]
		port.write(bytearray(packet))
		print "%s  crc: %d" % (payload, crcVal)
port.close()

