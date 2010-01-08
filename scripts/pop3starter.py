#!/usr/bin/env python
#
import os, time, sys
EXEC_NAME = "pop3s"
CMDS = {'kill':'9', 'stop':'USR1', 'drop':'USR2'}

def main():

    pids = os.popen(" ps -eo pid,args |grep \"" + EXEC_NAME + "\"").read().split()
    try:
        pid = int(pids[0])
        cmd = 'kill -' + CMDS[sys.argv[1]] + ' ' + str(pid)
        print cmd
        os.system(cmd)
    except:
        pass    
    return 0

if __name__ == '__main__': main()
