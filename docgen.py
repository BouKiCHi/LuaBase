#!/usr/bin/python
#
# docgen.py : an automatic document generator
#

import sys
import re


# Read function from file
def read_func( file ):
	d_func = {}
	l_func = []
	
	f = open( file )
	if f is None:
		return None
	l_lines = f.readlines()
	
	f.close()
	
	re_func    = re.compile("static.*int (.*)_(.*)\((.*)\)")

	for lineno in range(len(l_lines)):
		l = l_lines [ lineno ]
		m = re_func.match(l)
		if m:
			func     = "%s_%s" % (m.group(1),m.group(2))
			lua_func = "%s.%s" % (m.group(1),m.group(2))
			pref  = m.group(1)
			name  = m.group(2)
			
			if not d_func.has_key( func ):
				d_func[ func ] = True
				l_func.append( ( func , name , lua_func , lineno , pref ) )
	
	return l_func

# Read document of functions from file
def read_doc( file ):
	d_doc  = {}
	d_funcdoc = {}
	s_func = None
	
	f = open( file )
	if f is None:
		return None
	l_lines = f.readlines()
	
	f.close()
	
	re_comment = re.compile("^[/-]{2}\s@(\S*)\s*(.*)")
	
	for l in l_lines:
		m = re_comment.match(l)
		if m:
			key = m.group(1)
			value = m.group(2)
			
			# If new function is detected ...
			if key == "function":
				if s_func:
					d_funcdoc[ s_func ] = d_doc
			
				# Make new dictionary 	
				d_doc = {}
				s_func = value
	
			d_doc [ key ] = value
			

	# store infomation of last function
		
	if s_func:
		d_funcdoc[ s_func ] = d_doc
	
	
	return d_funcdoc


# Make new document dictionary
def new_doc( func ):
	version = "1.0"
	format = "%s()" % func

	return { "function" : func,
			 "format"   : format,
			 "in"  : "",
			 "out" : "",  
	         "describe" : "Created by docgen.py",
			 "version"  : version }


# Get Max length of keyword in dictionary
def get_maxlen( dict ):
	i_maxlen = 0

	for k,v in dict.iteritems():
		if i_maxlen < len(k):
			i_maxlen = len(k)

	return i_maxlen
	

# Make string list from dictionary 
def make_doc( d_doc , space  ):
	l_doc = []
	i_maxlen = space
	
	for k,v in d_doc.iteritems():
		spacer = " " * (i_maxlen - len(k))
		l_doc.append( "// @%s%s %s\n" % ( k , spacer , v ) )

	l_doc.append( "// @end\n" )
	
	return l_doc

# Display list
def disp_list ( list ):
	for l in list:
		print l
		
# Write list to file
def write_list ( file , list ):
	f = open( file , "w" )
	
	f.writelines( list )

	# for l in list:
	#	f.write( "%s\n" % l )
		
	f.close()	

# Read list from file
def read_list ( file ):
	l_lines = []

	f = open( file )
	if f is None:
		return None
	l_lines = f.readlines()
	
	f.close()
	
	return l_lines

# Make wiki text
def make_wikitext ( file , d_funcdoc , l_func ):
	l_text = []
	
	l_text.append("= %s =\n" % l_func[0][4])
	l_text.append("\n")
	
	for part in l_func:
		func     = part[0] # aaa_bbb
		name     = part[1] # bbb
		lua_func = part[2] # aaa.bbb
		lineno   = part[3]
		pref     = part[4] # aaa
		
		if not d_funcdoc.has_key ( lua_func ):
			d_doc = new_doc ( lua_func )
		else:
			d_doc = d_funcdoc [ lua_func ]
		
		l_text.append( "== %s ==\n\n" % lua_func )
		l_text.append( "''' format '''\n\n")
		l_text.append( " %s\n" % d_doc[ "format" ] )

		if d_doc.has_key("in") and d_doc["in"] != "":
			l_text.append( "''' in '''\n\n")
			l_text.append( " %s\n" % d_doc[ "in" ] )

		if d_doc.has_key("out") and d_doc["out"] != "":
			l_text.append( "''' out '''\n\n")
			l_text.append( " %s\n" % d_doc[ "out" ] )
			
		
		l_text.append( "''' describe '''\n\n")
		l_text.append( " %s\n" % d_doc[ "describe" ] )

		
		l_text.append( "''' version %s'''\n" % d_doc [ "version" ] )
		l_text.append( "\n" )
		

	
	outfile = "wiki." + file + ".text" 
	print "wikifile : %s" % outfile
	write_list ( outfile , l_text )

	


# Insert document part to file
def insert_docpart ( file , d_funcdoc , l_func ):

	i_space = get_maxlen ( new_doc("") )

	l_doc = []
	list = read_list ( file )
	i_line = 0
	add_newdoc = False

	for part in l_func:
		func     = part[0] 
		name     = part[1]
		lua_func = part[2]
		lineno   = part[3]
		
		l_doc.extend( list[i_line : lineno] )
		i_line = lineno
		
		if not d_funcdoc.has_key ( lua_func ):
			add_newdoc = True
			d_doc = new_doc ( lua_func )
			l_doc.extend( make_doc ( d_doc , i_space ) )
			
		

	l_doc.extend( list[i_line:] )

	if add_newdoc:
		outfile = "docs." + file 
		print "outfile : %s" % outfile
	
		write_list ( outfile , l_doc )

	# disp_list ( l_doc )


#
# main
#


if (len(sys.argv) < 2):
	print "docgen.py <c file> [c file ...]"
	quit()


for i in range(len(sys.argv) - 1):

	file = sys.argv[i + 1]

	print "infile:%s" % file

	l_func    = read_func( file )
	d_funcdoc = read_doc( file )
	
	insert_docpart ( file , d_funcdoc , l_func )
	make_wikitext ( file , d_funcdoc , l_func )
	
