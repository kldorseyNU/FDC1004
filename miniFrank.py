#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial
import time
from platform import system
if system() == 'Windows':
    from msvcrt import getch
else:
    from getch import getch

from math import isnan
from array import *
import binascii
import numpy as np
import serial.tools.list_ports
import rospy
import sys
from datetime import datetime
from std_msgs.msg import Float64
from sensors.msg import *
from sensors.msg import sensorMultiChannel
import typing

def main():
    pub = rospy.Publisher('capacitance', sensorMultiChannel, queue_size=10)
    rospy.init_node('capacitance_node', anonymous=True)
    port_var = rospy.get_param('~port', '/dev/ttyACM0')

    msg = sensorMultiChannel()
    
    serial_port = serial.Serial(port = port_var, baudrate = 115200)
    
    # This sleep is necessary to finish flushing the data
    time.sleep(0.5)
    
    serial_port.close()

    
    try:
        serial_port.open()

        rospy.loginfo("device_id=FDC1004")

        while not rospy.is_shutdown():
            # Initializing array for capacitance channels

            line = serial_port.readline()
            line = line.decode("utf-8")
            capvals = line.split(',')            

            msg.Header.stamp= rospy.Time.now()
            msg.Header.frame_id = 'Capacitance'
            if len(capvals) == 5:
                msg.channel0 = float(capvals[1])
                msg.channel1 = float(capvals[2])
                msg.channel2 = float(capvals[3])
                msg.channel3 = float(capvals[4])
            else: 
                rospy.logwarn(capvals[0])

            pub.publish(msg)
            rospy.loginfo(msg)

    except KeyboardInterrupt:
        exit()


if __name__ == "__main__":
    main()
