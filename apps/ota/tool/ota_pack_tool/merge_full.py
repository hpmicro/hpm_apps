#!/usr/bin/python
# -*- coding: utf-8 -*-


import sys, getopt
import os
import struct
import ctypes


# DNASYS_PATH = os.getenv('DNASYS_PATH')
# TARGET_APP  = os.getenv('TARGET_APP')

HEAD_BACKUP_OFFSET	= 0x400


common = {
	"BOOT_NAME": 		"boot_all.bin",
	"BOOTUSER_NAME": 	"boot_user.bin",
	"FIRMWARE1_NAME":   "user_app1.bin",
	"FIRMWARE2_NAME":   "user_app2.bin",
	"TARGET_SIZE":		0x00120000,
	"BOOT_ADDR": 		0x00000000,
	"BOOTUSER_ADDR":	0x00003000,
	"FIRMWARE1_ADDR":	0x00023000,
	"FIRMWARE2_ADDR":   0x000A3000,
}


def makeup_full(target, boot, fw1, fw2, tar_dic):

	target_fo = open(target, 'wb+')

	# clear target file to 0xff
	cnt = 0
	print("TARGET_SIZE:", tar_dic['TARGET_SIZE'])
	while (cnt < tar_dic['TARGET_SIZE']):
		target_fo.write(b'\xff')
		cnt += 1

	# 1. merge boot file
	boot_fo = open(boot, "rb")

	target_fo.seek(tar_dic['BOOT_ADDR'])
	target_fo.write(boot_fo.read())

	boot_fo.close()

	# 2. merge boot_user file
	# bootuser_fo = open(BOOTUSER_PATH, "rb")

	# target_fo.seek(tar_dic['BOOTUSER_ADDR'])
	# target_fo.write(bootuser_fo.read())

	# bootuser_fo.close()

	# 3. merge firmware1 file
	fw1_fo = open(fw1, 'rb')

	fw1_binary = fw1_fo.read()
	target_fo.seek(tar_dic['FIRMWARE1_ADDR']-HEAD_BACKUP_OFFSET)
	target_fo.write(fw1_binary)
	fw1_fo.close()

	# 4. merge firmware2 file
	fw2_fo = open(fw2, 'rb')

	fw2_binary = fw2_fo.read()
	target_fo.seek(tar_dic['FIRMWARE2_ADDR']-HEAD_BACKUP_OFFSET)
	target_fo.write(fw2_binary)
	fw2_fo.close()

	target_fo.close()


def main(argv):
	print ("0:", argv[0], ",1:", argv[1], ",2:", argv[2], ",3:", argv[3], ",4:", argv[4])

	# target = common
	print("BOOT_NAME:", common['BOOT_NAME'])

	makeup_full(argv[1], argv[2], argv[3], argv[4], common)
	print ("%s Merge Success!!!" %(argv[0]))


if __name__ == '__main__':
	main(sys.argv)
