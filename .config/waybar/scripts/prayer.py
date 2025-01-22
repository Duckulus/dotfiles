#!/usr/bin/python3
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.chrome.service import Service as ChromeService
from webdriver_manager.chrome import ChromeDriverManager
import sys

def loaded(driver):
    countdown_xpath="/html/body/div[9]/div/div[2]/div[7]/div[2]/div[2]/div/div"
    return driver.find_element(By.XPATH, countdown_xpath).text!=""

if len(sys.argv)<2:
    print("N/A")
    sys.exit(1)

timeout=5
masjid=sys.argv[1]
next_prayer_xpath="/html/body/div[9]/div/div[2]/div[7]/div[2]/div[2]/div"
not_found_xpath="/html/body/div/div/div/div/p[2]"

options = webdriver.ChromeOptions()
options.add_argument('--headless')
browser = webdriver.Chrome(service=ChromeService(ChromeDriverManager().install()), options=options)
browser.get(f"https://mawaqit.net/en/{masjid}")

try:
   if len(browser.find_elements(By.XPATH, not_found_xpath)):
       raise Exception()
   WebDriverWait(browser, timeout).until(loaded)
   el = browser.find_element(By.XPATH, next_prayer_xpath)
   print(el.text)
except Exception:
   print("N/A")
   sys.exit(1)
