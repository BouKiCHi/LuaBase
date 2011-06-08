#!/usr/bin/python
#
# autogen.py : convert C function to lua register
#


import sys
import re

if (len(sys.argv) < 2):
	print "autogen.py <c file>"
	quit()

print "file:%s" % sys.argv[1]

f = open(sys.argv[1])
lines = f.readlines()
f.close()

reg = re.compile("static.*int (.*)_(.*)\((.*)\)")

dict = {}
list = []
max_chr = 0

for line in lines:
	m = reg.match(line)
	if m:
		func = "%s_%s" % (m.group(1),m.group(2))
		name = m.group(2)
		
		if not dict.has_key( func ):
			dict[ func ] = True
			list.append( ( func , name  ) )
			if max_chr < len(name):
				max_chr = len(name)
			
print "len:%d" % max_chr

for part in list:
	name = part[1]
	func = part[0]
	spacer = " " * (max_chr - len(name))
	
	print "{ \"%s\"%s , %s }," % ( name , spacer , func )
	
