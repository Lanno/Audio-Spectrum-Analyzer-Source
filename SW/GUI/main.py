import os
from Queue import Queue

import htmlPy

from serial_io import Read_Thread

#######################################################################################

app = htmlPy.AppGUI(title=u"Audio Analyzer", maximized=True, developer_mode=True)

app.template_path = os.path.abspath(".")

app.static_path = os.path.abspath(".")

app.template = ("index.html", {"username" : "User"})

#######################################################################################

message_queue = Queue()

data_queue = Queue()

error_queue = Queue()

reader = Read_Thread(message_queue, data_queue, error_queue)

reader.start()

#######################################################################################

app.bind(message_queue)

app.bind(data_queue)

app.bind(error_queue)

app.start()