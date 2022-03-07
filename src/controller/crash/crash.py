import os
from datetime import date

class Crash():
    def __init__(self,exitcode,filename,content):
        self.creationdate = date.today().strftime("%d.%m.%Y_%H:%M:%S")
        self.exitcode = exitcode
        self.filename = filename
        self.content = content
        self.extension = os.path.splitext(self.filename)[-1]

    def safe_to_disk(self,directory):
        i = 0
        while True:
            optional = '' if i == 0 else f'_{i}'
            filename = f'crash_{self.creationdate}{optional}_{self.exitcode}_{self.filename}'
            abspath = os.path.abspath(os.path.join(directory,filename))

            if os.path.exists(abspath):
                i += 1
                continue
        
            open(abspath,'wb').write(self.content)
            break