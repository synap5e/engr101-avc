import pygame

pygame.init()
 
# Set the width and height of the screen [width,height]
size = [500, 700]
screen = pygame.display.set_mode(size)

pygame.display.set_caption("My Game")

#Loop until the user clicks the close button.
done = False

# Used to manage how fast the screen updates
clock = pygame.time.Clock()

# Initialize the joysticks
pygame.joystick.init()
   
joystick = pygame.joystick.Joystick(0)
joystick.init()
   
while done==False:
    for event in pygame.event.get(): # User did something
        if event.type == pygame.QUIT: # If user clicked close
			done=True # Flag that we are done so we exit this loop
#
    joystick_count = pygame.joystick.get_count()

    print "%s\t%s" % (round(joystick.get_axis(1), 1), round(joystick.get_axis(2), 1))

    # Limit to 10 updates per second
    #clock.tick(10)
    
# Close the window and quit.
# If you forget this line, the program will 'hang'
# on exit if running from IDLE.
pygame.quit ()