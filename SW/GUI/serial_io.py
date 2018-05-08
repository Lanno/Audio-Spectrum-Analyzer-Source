from threading import Thread
from Queue import Empty
from json import loads

from serial import Serial as pyserial
import htmlPy

class Queue_Wrapper(htmlPy.Object):
    def __init__(self, message_queue, data_queue, error_queue):
        htmlPy.Object.__init__(self)
        
        self._message_queue = message_queue
        
        self._data_queue = data_queue
        
        self._error_queue = error_queue
        
    @htmlPy.Slot(result=str)    
    def message(self):
        try:
            return self._message_queue.get(False)
            
        except Empty:
            return "empty"

class Serial_IO:
    def __init__(self, port, message_queue, data_queue, error_queue, baud_rate=115200):
        self._connection = pyserial(port, baud_rate, timeout=0)
    
        if not self._connection.is_open:
            raise "Serial connection failed."
            
        self._message_queue = message_queue
        
        self._data_queue = data_queue
        
        self._error_queue = error_queue
            
    def read(self):
        return self._connection.read(300000)
        

class Read_Thread(Thread):
    def __init__(self, message_queue, data_queue, error_queue):
        Thread.__init__(self)
    
        self._serial = Serial_IO("COM9", message_queue, data_queue, error_queue)
        
        self._name = "Read_Thread"
        
        self._message_queue = message_queue
        
        self._data_queue = data_queue
        
        self._error_queue = error_queue
        
    def run(self):
        buffer = ""
    
        while True:
            payload = self._serial.read()

            if len(payload) > 0:
                buffer += payload

                split = buffer.replace("\r", "").replace("\n", "").split("}{")
                
                if len(split) == 1:
                    messages = split
                    
                else:
                    for idx in range(len(split)):
                        if idx != 0 and idx != len(split) - 1:
                            split[idx] = "{" + split[idx] + "}"
                            
                        elif idx == 0:
                            split[idx] += "}"
                            
                        else:
                            split[idx] = "{" + split[idx]
                            
                    messages = split
                
                for message in messages:
                    try:
                        item = loads(message)
                        
                    except:
                        buffer = message
                        
                    else:
                        buffer = ""
                    
                        if item.keys()[0] == "message":
                            self._message_queue.put(item["message"])
                    
                        elif item.keys()[0] == "data":
                            self._data_queue.put(item["data"])
                        
                        elif item.keys()[0] == "error":
                            self._error_queue.put(item["error"])
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        