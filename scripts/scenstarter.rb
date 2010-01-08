#!/usr/bin/env ruby

require 'socket'
MAILDROP_DIR = "./maildir/"
SCENARIO_DIR = "./tests/"



def main
  connected = 0
  last_user = nil

  file = File.open(ARGV[1], 'r')
  socket = TCPSocket('127.0.0.1', ARGV[0])

  lines = file.readlines

  lines.each do |line|
    first_char = line[0]
    str = line[1..-1]
    if first_char == '#'
      puts str
    elsif first_char == ':'
      puts "\section{#{str}}"
    elsif first_char == '>'
      puts "C:#{str}"
      socket.puts str
    elsif first_char == '*'
      data = socket.gets
      puts data
    elsif first_char == '+'

  end



end


main
