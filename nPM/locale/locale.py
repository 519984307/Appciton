#!/usr/bin/python
# encoding=utf8

import csv
from xml.dom.minidom import Document
import sys
reload(sys)
sys.setdefaultencoding('utf8')

if __name__ == "__main__":
    try:    
        csvfile = open('locale.csv', 'rb')        
        reader =csv.DictReader(csvfile);        
    except IOError as e:
        print e
        sys.exit(1)
                
    languages = []
    docs = {}
    rootNode = {}
    for field in reader.fieldnames:
        if field.upper() != "ID" and  not field.startswith('_'):
            languages.append(field)                        
            docs[field] = Document()                        
            rootNode[field] = docs[field].createElement("language")            
            docs[field].appendChild(rootNode[field])
                                                
    for row in reader:
        for language in languages:
            element = docs[language].createElement(row["ID"])            
            text = docs[language].createTextNode(row[language])
            element.appendChild(text)
            rootNode[language].appendChild(element)            
            
    for language in languages:
        with open(language+".xml", "w") as f:
            f.write(docs[language].toprettyxml(encoding="UTF-8"))