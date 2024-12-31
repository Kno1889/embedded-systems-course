from pymavlink import mavutil

connection = mavutil.mavlink_connection('/dev/ttyACM0')
print("Hear")
connection.wait_heartbeat()
print("Heartbeat from system (system %u component %u)" % (connection.target_system, connection.target_component))


while 1:
    msg = connection.recv_match(type = "ATTITUDE", blocking = True)
    print(msg)