Answer: How to write a chat program (two clients chat with each other) with UDP?

两个客户端都绑定到同一个端口，然后其中一个客户端先发送消息，然后进入接收消息的状态；另一个客户端先接收消息，然后再回复第一个客户端的消息。

```python
bind(port)
while True:
    send(Msg, port)
    rcv = receive()
```

```python
bind(port)
while True:
    rcv = receive()
    send(Msg, port)
```

