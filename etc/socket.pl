#!/usr/bin/env perl
# Kaolin - Exfoliate your face
# Copyright (c) 2008 Devin Torres
# Licensed under the MIT license.

# Used for stress testing sockets.

use Socket;
use IO::Handle;

if ($#ARGV < 2) {
  print "Usage: socket.pl [host] [port] [number of times to connect]\n";
  exit;
}

$host = $ARGV[0];
$port = $ARGV[1];
$num  = $ARGV[2];

($name, $aliases, $proto) = getprotobyname('tcp');
($name, $aliases, $type, $len, $thataddr) = gethostbyname($host);
$sockaddr = 'S n a4 x8';
$that = pack($sockaddr, AF_INET, $port, $thataddr);

for ($i = 1; $i <= $num; $i++) {
  print "Opening connection $i...\n";

  $s[$i] = IO::Handle->new();
	
  socket($s[$i], AF_INET, SOCK_STREAM, $proto) or "socket: die $!";
  connect($s[$i], $that) or die "connect: $!";
}
