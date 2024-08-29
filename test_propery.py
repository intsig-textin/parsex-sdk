class Document:  
    def __init__(self, app_key=None):
        pass

    @property
    def page_num(self):
        return 100
    

if __name__ == '__main__':
    doc = Document()
    print(doc.page_num)