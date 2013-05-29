import pygame, copy, time
from pygame.locals import *

pygame.init()

pygame.joystick.init()
if not pygame.joystick.get_count():
   print "nPlease connect a joystick and run again.n"
   quit()

screen = pygame.display.set_mode((640, 480))
pygame.display.set_caption('AVC')

done = False
joystick_count = pygame.joystick.get_count()
while not done:
	for stick in range(joystick_count):
		joystick = pygame.joystick.Joystick(stick)
		axes = joystick.get_numaxes()
		for axis in range( axes ):
			value = joystick.get_axis( axis )
			print "Joystick " + stick + ", axis " + axis + "\t\t" + value

