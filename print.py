#!/usr/bin/python
################################################################
import datetime 
import time
import serial
import os
import binascii
import struct
import crcmod
import sys
#Especificar o local de registro no HD##########################
#dirbkp="/home/pi/dados/"
#"/home/fieldmill/dados/"
################################################################
#Especificar o dispositivo serial###############################
ser=serial.Serial('/dev/ttyUSB0', 921600, timeout=1)
################################################################
crc = crcmod.predefined.Crc('crc-16')

while 1:
#	saida=open("serial_test.dat",'a')
#	line=ser.readline() # Lendo linhas ascii, precisa do \n
	line=ser.read(132) #com CRC

#	print line
#	sys.exit()

	crc= crc.new()
	crc.update(line[0:128])
#	print "Calc: %d\n" % crc.crcValue,
#	print "Hex: "+  crc.hexdigest()+"\n",
#	print "Size: %d\n" % crc.digest_size,

	x=64
	z=struct.unpack("H", line[2*x:2*x+2])[0]

	for x in range(0, 64):
		z=struct.unpack("h", line[2*x:2*x+2])[0]
		print "%4d" % z,
		if ((x+1) % 32)==0:
			print "%2d" % (x+1),
			print ""
ser.close()