from pymavlink import mavutil
import time

connection = mavutil.mavlink_connection('/dev/ttyACM0')
print("Hear")
connection.wait_heartbeat()
print("Heartbeat from system (system %u component %u)" % (connection.target_system, connection.target_component))

value = 0
while 1:
    msg = mavutil.mavlink.MAVLink_debug_message(0, value, 0.0)
    connection.mav.send(msg)
    time.sleep(1)
    value = (value + 1) % 2
    print("Message sent")
