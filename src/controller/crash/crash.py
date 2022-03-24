import os
import datetime

class Crash():
    def __init__(self,exitcode,filename,content):
        self.creationdate = datetime.datetime.now()
        self.exitcode = exitcode
        self.filename = filename
        self.content = content
        self.extension = os.path.splitext(self.filename)[-1]

    def safe_to_disk(self,directory):
        i = 0
        while True:
            optional = '' if i == 0 else f'_{i}'
            exit_info = self.exitcode if self.exitcode != None else 'hang'
            date = self.creationdate.strftime("%d.%m.%y_%H:%M")
            filename = f'crash_{date}{optional}_{exit_info}_{self.filename}'
            abspath = os.path.abspath(os.path.join(directory,filename))

            if os.path.exists(abspath):
                i += 1
                continue
        
            open(abspath,'wb').write(self.content)
            break