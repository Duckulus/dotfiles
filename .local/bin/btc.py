#!/usr/bin/python
import requests

r = requests.get("https://api.coinbase.com/v2/exchange-rates?currency=BTC")
print(str(round(float(r.json()["data"]["rates"]["EUR"]), 2)) + "€")
