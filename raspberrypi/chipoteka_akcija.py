from lxml import html
import urllib2
import re

url="http://www.chipoteka.hr/mega-akcija/?page="

for i in range(1,11):
	page = urllib2.urlopen(url+str(i)).read()
	parsed_page=html.fromstring(page)
	elements = parsed_page.xpath('.//div[@class="p-g"]')

	for element in elements:
		cijena1_text = element.xpath('.//div[@class="p1"]')[0].text
		cijena2_text = element.xpath('.//div[@class="p2 akcija"]')[0].text
		cijena1 = float(re.search('[\d.,]+', cijena1_text).group(0).replace('.', '').replace(',', '.'))
		cijena2 = float(re.search('[\d.,]+', cijena2_text).group(0).replace('.', '').replace(',', '.'))
		naziv = element.xpath('.//a[@class="t"]')[0].text
		print ("%s;%.2f;%.2f;%.0f" % (naziv.encode('utf-8'), cijena1, cijena2, (cijena1-cijena2)/cijena1*100))