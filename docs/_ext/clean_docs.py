import sys, getopt
import os
import filecmp
import hashlib
from pathlib import Path
import re
import shlex
import shutil
from subprocess import Popen, PIPE, STDOUT
import tempfile
from typing import List, Dict, Optional, Any

def main(argv):
    print("clean...")

    clean_dir1 = "output"
    clean_dir2 = "zh/apps"
    clean_dir3 = "zh/_static"
    clean_dir4 = "en/apps"
    clean_dir5 = "en/_static"
    clean_dir6 = "zh/middleware"
    clean_dir7 = "en/middleware"

    if os.path.isdir(clean_dir1):
        shutil.rmtree(clean_dir1)

    if os.path.isdir(clean_dir2):
        shutil.rmtree(clean_dir2)

    if os.path.isdir(clean_dir3):
        shutil.rmtree(clean_dir3)

    if os.path.isdir(clean_dir4):
        shutil.rmtree(clean_dir4)

    if os.path.isdir(clean_dir5):
        shutil.rmtree(clean_dir5)

    if os.path.isdir(clean_dir6):
        shutil.rmtree(clean_dir6)

    if os.path.isdir(clean_dir7):
        shutil.rmtree(clean_dir7)

    print("clean done!")


if __name__ == '__main__':
    main(sys.argv)
    

    
