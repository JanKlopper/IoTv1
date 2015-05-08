#!/usr/bin/python2.7
# -*- coding: utf8 -*-
""" UDP listening client for the Weatherduino UDP output"""
__author__ = 'Jan KLopper (jan@underdark.nl)'
__version__ = 0.1

PROTOCOLVERSION = 1
MAGIC = 101

import socket
import argparse
import struct
import time
import os

class WeatherDuinoListener(object):
  def __init__(self, options, handler):
    """This function listens for udp packets on all ethernet interfaces on the
    given port and processes them."""

    self.options = options
    self.UDPSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self.UDPSock.bind(('', options.port))
    self.verbose = options.verbose
    print 'Starting WeatherDuino listener'

  def Run(self):
    count = 0
    while True:
      data, addr = self.UDPSock.recvfrom(1024)
      if self.verbose:
        count = count + 1
        print 'Packet %s @ %s' % (count, time.strftime("%H:%M:%S"))
      for measurement in self.ParsePacket(data):
        self.PrintMeasurements(*measurement)

  def ParsePacket(self, data):
    """This processes the actual data packet"""
    magic = ord(data[0])
    version = ord(data[1])
    if magic == MAGIC and version == PROTOCOLVERSION:
      device = (ord(data[2]), ord(data[3]), ord(data[4]))
      probecount = ord(data[5])
      offset = 6
      if self.verbose:
        print '%d probes found on %x:%x:%x' % (
            probecount, device[0], device[1], device[2])
      for sensor in range(0, probecount):
        humidity = ord(data[(sensor*3)+offset+2:(sensor*3)+offset+3])
        temp = (ord(data[(sensor*3)+offset:(sensor*3)+offset+1]),
                ord(data[(sensor*3)+offset+1:(sensor*3)+offset+2]))
        yield (device, sensor, temp, humidity)

  def PrintMeasurements(self, device, sensor, temp, humidity):
    """Prints the data for a sensor if either temperature or humidty is valid"""
    if temp[0] < 129 or humidity < 255:
      print 'Sensor %i:' % sensor
    if temp[0] < 129:
      print '\ttemp: %d.%02d°' % temp
    if humidity < 255:
      print '\thumidity: %d%%' % humidity

  def __del__(self):
    self.UDPSock.close()

def main():
  """This program listenes to the broadcast address on the listening port and
  handles any received measurements

  Measurements are output to stdout"""
  parser = argparse.ArgumentParser()
  parser.add_argument("-p", "--port", dest="port",
                    help="Listen port", default=65001)
  parser.add_argument("-v", "--verbose", dest="verbose",
                    action="store_true",
                    help="Be verbose", default=False)
  options = parser.parse_args()

  wduino = WeatherDuinoListener(options)
  wduino.Run()

if __name__ == '__main__':
  main()
