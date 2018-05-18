from threading import Thread
from Queue import Empty
from struct import unpack
from json import loads, dumps

from serial import Serial as pyserial
from numpy.fft import fft
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
            result = self._message_queue.get(False)

            print type(result), len(result)

            return result

        except Empty:
            return "empty"

    @htmlPy.Slot(result=str)
    def data(self):
        try:
            data = self._data_queue.get(False)

            left_data  = [];

            right_data = [];

            for point in data:
                hex_string = hex(point).strip("0x").strip("L").zfill(8).decode("hex")
                
                left, right = unpack("hh", hex_string)

                left  = left  / 2.0**16

                right = right / 2.0**16

                left_data.append(left)

                right_data.append(right)

            left_fft  = [abs(point) for point in fft(left_data)][0:len(left_data)/2]

            right_fft = [abs(point) for point in fft(right_data)][0:len(right_data)/2]

            result = [left_data, right_data, left_fft, right_fft]

            print type(result), len(result)

            return dumps(result)

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
                with open("payload.txt", "a") as handle:
                    handle.write(payload)

                split = payload.replace("\r", "").replace("\n", "").split("}{")

                with open("split.txt", "a") as handle:
                    handle.writelines(split)

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

                        with open("json.txt", "a") as handle:
                            handle.write(message + "\n")

                    except Exception as error:
                        pass

                        buffer += message

                        try:
                            item = loads(buffer)

                            with open("json.txt", "a") as handle:
                                handle.write(buffer + "\n")

                        except Exception as error:
                            pass

                        else:
                            buffer = ""

                            try:
                                if item.keys()[0] == "message":
                                    self._message_queue.put(item["message"])

                                elif item.keys()[0] == "data":
                                    self._data_queue.put(item["data"])

                                elif item.keys()[0] == "error":
                                    self._error_queue.put(item["error"])

                            except AttributeError:
                                pass

                    else:
                        try:
                            if item.keys()[0] == "message":
                                self._message_queue.put(item["message"])

                            elif item.keys()[0] == "data":
                                self._data_queue.put(item["data"])

                            elif item.keys()[0] == "error":
                                self._error_queue.put(item["error"])

                        except AttributeError:
                            pass


















