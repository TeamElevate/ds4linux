import Queue
import threading
import socket
import uuid

class WifiNetwork(object):
    knownIPs = []
    lock = threading.Lock()
    threads = []
    recved = {}
    queue = Queue.Queue()
    def __init__(self):
        listener = threading.Thread(target=self._listen)
        listener.start()
        listener.Daemon = True

    def send(self, message):
        with open("/opt/scan.txt") as f:
            for line in f:
                ip = line.split(':')[0]
                self.add_known_IP(ip)

        msgid = uuid.uuid4()
        self.recved[msgid] = True
        for ip in self.knownIPs:
            threading.Thread(target=self._send_to, args=(message, msgid, ip)).start()

    def _send_to(self, message, msgid, address):
        try:
            ip_string = ','.join([ip for ip in self.knownIPs if ip != address])
            msg = "#%3d%s$%s#%3d%s" % (len(ip_string), ip_string, msgid, len(message), message)
            s = socket.socket()
            s.connect((address, 24398))
            s.send(msg)
        except:
            pass
        finally:
            s.close()

    def receive(self, block=False):
        try:
            return self.queue.get(block)
        except Queue.Empty:
            return None

    def add_known_IP(self,ip):
        with self.lock:
            if ip not in self.knownIPs:
                self.knownIPs.append(ip)

    def _listen(self):
        s = socket.socket()
        s.bind(('', 24398))
        s.listen(2)
        while 1:
            (client, address) = s.accept()
            self.add_known_IP(address[0])

            receiver = threading.Thread(target=self._receive_conn, args=(client, address[0])).start()

    def _receive_conn(self, client, address):
        try:
            if client.recv(1, socket.MSG_WAITALL) != "#":
                return
            ip_size = int(client.recv(3))
            if ip_size != 0:
                ips = client.recv(ip_size, socket.MSG_WAITALL)
                ips = ips.split(',')

            for ip in ips:
                self.add_known_IP(ip)

            if client.recv(1, socket.MSG_WAITALL) != "$":
                return

            msgid = client.recv(36, socket.MSG_WAITALL)

            if client.recv(1, socket.MSG_WAITALL) != "#":
                return

            size = int(client.recv(3))
            msg = client.recv(size, socket.MSG_WAITALL)

            with self.lock:
                if msgid not in self.recved:
                    self.recved[msgid] = True
                    self.queue.put(msg)
                else:
                    return

            for ip in self.knownIPs:
                if ip not in ips and ip != address:
                    self._send_to(msg, msgid, ip)

        except:
            pass
        finally:
            client.close()


