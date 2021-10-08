import socket
import sys

def login():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('192.168.44.133', 1234))

    except socket.error as msg:
        print(msg)
        print(sys.exit(1))
    # 1. respond with an initial message
    s.recv(1024)
    # 2. client to send a “USER anonymous” command.
    str = 'USER anonymous\0'
    s.send(str.encode())
    # 3. ask for an email address as a password
    s.recv(1024)
    email = input("Please input an email address for logging in: ") + '\0'
    s.send(email.encode())
    # 4. rcv greetings
    s.recv(1024).decode("utf-8", "ignore")

    s.close()

def port():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('192.168.44.133', 1234))

    except socket.error as msg:
        print(msg)
        print(sys.exit(1))
    s.recv(15)
    print("success!")

def pasv():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('192.168.44.133', 1234))
    except socket.error as msg:
        print(msg)
        print(sys.exit(1))
    s.recv(50)
    print("PASV success!")

def syst():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('192.168.44.133', 1234))
    except socket.error as msg:
        print(msg)
        print(sys.exit(1))
    s.recv(50)
    print("SYST success!")

def type():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('192.168.44.133', 1234))
    except socket.error as msg:
        print(msg)
        print(sys.exit(1))
    s.recv(50)
    print("TYPE success!")

if __name__ == '__main__':
    #login()
    #port()
    #pasv()
    #syst()
    type()

