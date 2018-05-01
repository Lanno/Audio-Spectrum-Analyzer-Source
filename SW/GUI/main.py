import os

import htmlPy


app = htmlPy.AppGUI(title=u"Audio Analyzer", maximized=True, developer_mode=True)

app.template_path = os.path.abspath(".")

app.static_path = os.path.abspath(".")

app.template = ("index.html", {"username" : "User"})

app.start()