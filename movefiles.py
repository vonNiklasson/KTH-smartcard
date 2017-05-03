import shutil
import os

source = 'C:\Users\Erik\Documents\GitHub\KTH-smartcard'
destination = 'C:\Users\Erik\Downloads\PK2proj\PK2proj\Work'


files = os.listdir(source)

for f in files:
	if (f.startswith(".") == False and f.startswith("movefiles") == False):
		shutil.copy(f, destination)