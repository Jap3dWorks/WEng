import logging
import sys
wlogger = logging.getLogger('WCLI')
wlogger.setLevel(logging.DEBUG)

wlogger.propagate = False
wlogger_console_handler = logging.StreamHandler(sys.stdout)
wlogger_console_handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('[%(name)s][%(levelname)s] %(message)s')
wlogger_console_handler.setFormatter(formatter)

wlogger.addHandler(wlogger_console_handler)
