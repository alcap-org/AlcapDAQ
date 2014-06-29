import os
import time
import datetime
import merlin_utils
import DBManager
import RunManager
import subprocess
import make_test_table


merlin_utils.remove_all_files(3200, "alcapana")
merlin_utils.remove_all_files(3201, "alcapana")
merlin_utils.remove_all_files(3202, "alcapana")
merlin_utils.remove_all_files(3203, "alcapana")

import run_production
