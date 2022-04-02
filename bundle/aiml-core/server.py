import json, socket

class Server:
  def __init__(self, port):
    self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    self.server_socket.bind((socket.gethostname(), port))
    self.server_socket.listen(5)
  
  def _waits_for(self):
    (socket, address) = self.server_socket.accept()
    try:
      data = socket.recv(8192)
      return data
    except ConnectionRefusedError:
      print("Connection refused.")
      return b"{}"
  
  def _encode_json(j):
    return json.dumps(j).encode()
  
  def _decode_json(b):
    return json.loads(b.decode())
  
  def listen(self):
    return self._decode_json(self._waits_for())
