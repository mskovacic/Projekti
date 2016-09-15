import urllib2
from lxml import html

URL = "http://zetcode.com/gui/pyqt5/"
page = urllib2.urlopen(URL).read()
parsed_page = html.fromstring(page)
content_elements = parsed_page.xpath("//nav/ul//a")
contents = []
for c in content_elements:
    contents.append(c.attrib["href"])
urls = []
for c in contents:
    urls.append(URL+c)

for url in urls:
    page = urllib2.urlopen(url).read()
    parsed_page = html.fromstring(page)
    h2_elements = parsed_page.xpath("//h2")
    code_elements = parsed_page.xpath("//pre[@class='code']")
    while len(h2_elements) > len(code_elements):
        h2_elements.pop(0)    

    for i in range(len(h2_elements)):
        filename = h2_elements[i].text.replace(" ", "_") + ".py"
        file = open(filename, "w")
        file.write(code_elements[i].text)
        file.close()
    

