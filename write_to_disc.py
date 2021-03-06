#!/usr/bin/python
################################################################
#Aquisicao de dados do sensor de campo eletrico (FieldMill)
#BOLTECK EFM-100. Registra o valor do campo e o tempo.
################################################################
#data:2011-02-04
#Autor: Evandro M. Anselmo
#e-mail: evandro@model.iag.usp.br
#dependencias: python versao:2.5.2-3; python-serial versao:2.3-1
################################################################
import datetime 
import time
import serial
import os
import binascii
import struct
import crcmod
import sys
import signal

ser=serial.Serial('/dev/ttyUSB0', 921600, timeout=None)
ser.flush()
ser.flushInput()
ser.flushOutput()

################################################################
file=open("teste.bin","wb")
crc = crcmod.predefined.Crc('crc-16')
flag_0=1

index=0
n_blocos=5*11314.25/64
while 1:
	line=ser.read(132) #com CRC e indice
#	index=struct.unpack("H", line[130:132])[0]
	if (index==0 or flag_0 ==0):
		crc= crc.new()
		crc.update(line[0:129])
		z=struct.unpack("H", line[130:131])[0]
	
		if ( crc.crcValue == z):
			print "OK! %4X %4X %d\n" % (z,crc.crcValue,index),
			file.write(line[2:129])
		else:
			print "CRC fail! %4X %4X\n" % (z,crc.crcValue),

		if(index<n_blocos):
			flag_0=0
		else:
			flag_0=2
	else:	
		print index
		continue
	index=index+1
		
	if (flag_0==2):
		file.close()
		ser.close()
		sys.exit(1)

