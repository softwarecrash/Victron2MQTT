Import("env")
import os
import glob
from pathlib import Path
import sys
import pip

def install(package):
    if hasattr(pip, 'main'):
        pip.main(['install', package])
    else:
        pip._internal.main(['install', package])

try:
    import minify_html
except ImportError:
    install('minify_html')

filePath = 'src/webpages/'

try:
  print("==========================")
  print("Generating webpage")
  print("==========================")
  print("Preparing html.h file from source")
  print("  -insert header") 
  cpp_output = "#pragma once\n\n#include <Arduino.h>  // PROGMEM\n\n"
  print("  -insert html")

  for x in glob.glob(filePath+"*.html"):
   print("prozessing file:" + Path(x).stem)
   print(Path(x).stem)
   cpp_output += "static const char "+Path(x).stem+"[] PROGMEM = R\"rawliteral("
   f = open(x, "r")
   if env.GetProjectOption("build_type") == "debug":
        content = f.read()  
   else:
        content = minify_html.minify(f.read(), minify_js=True)
   cpp_output += content
   f.close()
   cpp_output += ")rawliteral\";\n"
   cpp_output += "#define " +Path(x).stem+ "_LEN " + str(len(content)) +"\n"


   f = open ("./src/html.h", "w")
   f.write(cpp_output)
   f.close()
   print("==========================\n")

except SyntaxError as e:
  print(e)