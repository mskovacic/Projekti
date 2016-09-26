from selenium import webdriver
#from lxml import html

#existing tag: J-347-3553-E
URL = "https://3ds.pokemon-gl.com/user/%s/secretbase"

#tag: slovo, 3 broja, 4 broja, slovo

def get_picture_url(tr_elements):
	for tr_element in tr_elements:
		for th_element in tr_element.find_elements_by_xpath('.//th'):
			print th_element.text
		if tr_element.find_elements_by_xpath('.//th')[0].text == 'QR Code':
			return tr_element.find_elements_by_xpath('.//img')[0].get_attribute('src')

def get_tag(i, j, k, l):
	return '%s-%s-%s-%s' % (i, j, k, l)

def get_location(tr_elements):
	for tr_element in tr_elements:
		for th_element in tr_element.find_elements_by_xpath('.//th'):
			print th_element.text
		if tr_element.find_elements_by_xpath('.//th')[0].text == 'Location':
			return tr_element.find_elements_by_xpath('.//p')[0].text
			
def write_to_file(file, tag, location, url):
	if tag:
		if location:
			if url:
				file.write('%s,%s,%s\n' % (tag, location, url))
				

#from pyvirtualdisplay import Display
#display = Display(visible=0, size=(800, 600))
#display.start()

profile = webdriver.FirefoxProfile()
profile.native_events_enabled = False
driver = webdriver.Firefox(profile)
out_file = open("secret_bases.csv", "a")

for i in xrange(ord('A'), ord('Z')+1):
	for j in xrange (0, 1000):
		for k in xrange (0, 10000):
			for l in xrange(ord('A'), ord('Z')+1):
				tag = "%s-%3d-%4d-%s" % (i, j, k, l)
				url = URL % (tag)
				print "Working on url: %s" % (url)
				driver.get(url)
				tr_elements = driver.find_elements_by_xpath('//tr')

				location = get_location(tr_elements)
				picture_url = get_picture_url(tr_elements)
				write_to_file(out_file, tag, location, picture_url)

out_file.close()
driver.quit()
#display.stop()
