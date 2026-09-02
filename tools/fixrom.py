#!/usr/bin/env python

#TODO replace this whole thing with a self-contained C program or srecord command

import struct, sys, os
ROM_BASE = 0x0E000000
with open(sys.argv[1],"r+b") as f:
	# Get checksum range from header
	f.seek(0)
	check_start, check_end = struct.unpack(">LL", f.read(8))
	check_length = check_end+2-check_start
	
	# Sum the data in that range
	f.seek(check_start-ROM_BASE)
	check_data = f.read(check_length)
	s = 0
	for i in range(0, len(check_data), 2):
		s += struct.unpack(">H", check_data[i:i+2])[0]
		s &= 0xFFFFFFFF
	
	# Insert the checksum into the header
	f.seek(8)
	f.write(struct.pack(">L", s))
	print(f"Checksum over {check_length/1024:.1f} KiB: {s:08X}")

