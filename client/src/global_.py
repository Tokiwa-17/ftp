
class Global():
    def __init__(self):
        self.recv_msg = ''

    def set_value(self, value):
        self.recv_msg = value

    def get_value(self):
        return self.recv_msg