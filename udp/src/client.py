import socket
 
size = 8192

for i in range(51):
  try:
    msg = str(i)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # 发送数据
    sock.sendto(msg.encode(), ('127.0.0.1', 9876))
    # 接收数据
    print(sock.recv(size).decode('utf-8'))
    sock.close()
 
  except:
    print("cannot reach the server")