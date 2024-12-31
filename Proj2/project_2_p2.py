#Libraries
import RPi.GPIO as GPIO
import time

#set GPIO Pins
GPIO_TRIGGER = 23
GPIO_ECHO = 24
#Libraries
import cv2
import numpy as np
import math

from pymavlink import mavutil
import time

connection = mavutil.mavlink_connection('/dev/ttyACM0')
print("Hear")
connection.wait_heartbeat()
print("Heartbeat from system (system %u component %u)" % (connection.target_system, connection.target_component))


def ultrasonic_setup():
    #GPIO Mode (BOARD / BCM)
    GPIO.setmode(GPIO.BCM)
    #set GPIO direction (IN / OUT)
    GPIO.setup(GPIO_TRIGGER, GPIO.OUT)
    GPIO.setup(GPIO_ECHO, GPIO.IN)

def distance():
    # set Trigger to HIGH
    GPIO.output(GPIO_TRIGGER, True)
    # set Trigger after 0.01ms to LOW
    time.sleep(0.00001)
    GPIO.output(GPIO_TRIGGER, False)
    StartTime = time.time()
    StopTime = time.time()
    # save StartTime
    while GPIO.input(GPIO_ECHO) == 0:
        StartTime = time.time()
    # save time of arrival
    while GPIO.input(GPIO_ECHO) == 1:
        StopTime = time.time()

    # time difference between start and arrival
    TimeElapsed = StopTime - StartTime
    # multiply with the sonic speed (34300 cm/s)
    # and divide by 2, because there and back
    distance = (TimeElapsed * 34300) / 2
    
    return distance

cap = cv2.VideoCapture(0)
StepSize = 5
def getChunks(l, n):
    """Yield successive n-sized chunks from l."""
    a = []
    for i in range(0, len(l), n):
        a.append(l[i:i + n])
    return a

def process_camera_frame():
    ret,frame = cap.read()
    img = cv2.flip(frame, 0)
    blur = cv2.bilateralFilter(img,9,40,40)
    edges = cv2.Canny(blur,50,100)
    img_h = img.shape[0] - 1
    img_w = img.shape[1] - 1
    EdgeArray = []
    for j in range(0,img_w,StepSize):
        pixel = (j,0)
        for i in range(img_h-5,0,-1):
            if edges.item(i,j) == 255:
                pixel = (j,i)
                EdgeArray.append(pixel)
                break
    if len(EdgeArray) != 0:
        chunks = getChunks(EdgeArray, math.ceil(len(EdgeArray)/3))
    else:
        return

    #c = []
    distance = []
    for i in range(len(chunks)):
        x_vals = []
        y_vals = []
        for (x,y) in chunks[i]:
            x_vals.append(x)
            y_vals.append(y)
        avg_x = int(np.average(x_vals))
        avg_y = int(np.average(y_vals))
        #c.append([avg_y,avg_x])
        distance.append(math.sqrt((avg_x - 320)**2 + (avg_y - 640)**2))
        cv2.line(img, (320, 640), (avg_x,avg_y), (0,0,255), 2)
    
    cv2.imshow("frame", img)
    cv2.waitKey(5)
    if(distance[0] < distance[1]):
        if(distance[0] < distance[2]):
            return 0
        else:
            return 2
    else:
        if(distance[1] < distance[2]):
            return 1
        else:
            return 2

ultrasonic_setup();
val = 0
while True:
    dist = distance()
    dire = process_camera_frame()
    
    if dist < 15:
        if dire == 0: # left
            val = 0
        elif dire == 1: # fwd
            val = 1
        elif dire == 2: # right
            val = 2
    elif dist > 15 and dist < 50:
        if dire == 0: # left
            val = 3
        elif dire == 1: # fwd
            val = 4
        elif dire == 2: # right
            val = 5
    else:
        if dire == 0: # left
            val = 6
        elif dire == 1: # fwd
            val = 7
        elif dire == 2: # right
            val = 8
            
    print("Value: %d" % val)

    msg = mavutil.mavlink.MAVLink_debug_message(0, val, 0.0)
    connection.mav.send(msg)
    
    time.sleep(0.1)
    