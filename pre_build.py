Import("env")
import subprocess

version = "undefined_version"
boardname = "undefined_boardname"

try: 
    defFile = open("src/definitions.h")
    for line in defFile:
        if line.startswith("#define MAJOR_VERSION"):
            version = line.replace("#define MAJOR_VERSION ", "")
            version = version.strip()
    defFile.close()

    defFile = open("src/definitions.h")
    for line in defFile:
        if line.startswith("#define BOARD_NAME"):
            boardname = line.replace("#define BOARD_NAME ", "")
            boardname = boardname.strip()
            boardname = boardname.replace("\"", "")

    defFile.close()
except:
    print ("Board name or version number not found.")

program_file_name = boardname + "_v" + version 

env.Replace(PROGNAME=program_file_name)
