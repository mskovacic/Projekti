import urllib2
from lxml import html

url = "https://www.links.hr/hr/discounted-products?pagenumber="
start_index = 1
homepage = "https://www.links.hr"


def crawl_discount_page(url):
	page = urllib2.urlopen(url).read()
	parsed_page = html.fromstring(page)
	elements = parsed_page.xpath('.//div[@class="item-box"]')

	for element in elements:
		link_element = element.xpath('.//a')[0]
		title = link_element.attrib["title"]
		link = homepage + link_element.attrib["href"]
		actual_price_text = element.xpath('.//span[@class="price actual-price"]')[0].text
		old_price_text = element.xpath('.//span[@class="price old-price"]')[0].text
		actual_price = float(actual_price_text.replace(".",""))
		old_price = float(old_price_text.replace(".",""))
		discount = (old_price - actual_price) / old_price * 100
		print "%s;%.2f;%.2f;%.0f;%s" % (title.encode("utf-8"), old_price, actual_price, discount, link)
 
	next_page_element = parsed_page.xpath('.//li[@class="next-page"]/a')
	if next_page_element:
		next_page = homepage + next_page_element[0].attrib["href"]
		crawl_discount_page(next_page)


crawl_discount_page(url+str(start_index))
