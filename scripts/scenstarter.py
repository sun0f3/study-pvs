#!/usr/bin/env python
#

import sys, socket, time, os

MAILDROP_DIR = "./maildrop/"
SCENARIO_DIR = "./scenarios/"
def main():
    connected = 0
    last_user = None
    f = open(sys.argv[2], 'r') 
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    while connected < 5:
        try:
            time.sleep(2)
            s.connect(("127.0.0.1", int(sys.argv[1])))
            break
        except:
            pass
        connected += 1
            
    for line in f:
        first_char = line[0]
        str = line[1:len(line)-1]
        if first_char == '#':
            print str
        elif first_char == ':':
            print "\section{"+str+"}"
        elif first_char == '>':
            print 'C:'+str
            s.send(str)
        elif first_char == '*':
            data = s.recv(1024)
            try:
                print unicode('S:'+data[:len(data)-1])
            except:
                print "ERROR: Non-Unicode string arrived."
        elif first_char == '+':
            s.settimeout(0.3)
            try:
                while True:
                    data = s.recv(1024)
                    print 'S:'+data[:len(data)]
            except:
                s.settimeout(None)
        elif first_char == '@':
            os.system('mkdir -p ' + MAILDROP_DIR + str)
            last_user = str
        elif (first_char == '!') and (last_user != None):
            os.system('cp ' + SCENARIO_DIR + str.split(':')[0] + ' ' + MAILDROP_DIR + last_user + '/' + str.split(':')[1])
    s.close()
    f.close()
    print "\n"
    return 0

if __name__ == '__main__': main()
