import os
from Queue import Queue

import htmlPy

from serial_io import Read_Thread, Queue_Wrapper

#######################################################################################

app = htmlPy.AppGUI(title=u"Audio Analyzer", maximized=True, developer_mode=True)

app.template_path = os.path.abspath(".")

app.static_path = os.path.abspath(".")

app.template = ("index.html", {"username" : "User"})

#######################################################################################

message_queue = Queue()

data_queue = Queue()

error_queue = Queue()

try:
	reader = Read_Thread(message_queue, data_queue, error_queue)

except Exception as error:
	print error
	
	input()
	
	exit()
	
else:
	reader.start()

#######################################################################################
	
app.bind(Queue_Wrapper(message_queue, data_queue, error_queue))

app.start()
