import socket

size = 8192

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(('127.0.0.1', 9876))
sequence_number = 1
try:
  while True:
    data, address = sock.recvfrom(size)
    data = data.decode('utf-8')
    data = str(sequence_number) + ' ' + data.upper()
    sock.sendto(data.encode(), address)
    sequence_number += 1
finally:
  sock.close()