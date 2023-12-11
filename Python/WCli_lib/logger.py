import logging
wlogger = logging.getLogger('WCLI')

wlogger.propagate = False
formatter = logging.Formatter('[%(name)s][%(levelname)s] %(message)s')

wlogger_console_handler = logging.StreamHandler()
wlogger_console_handler.setLevel(logging.DEBUG)
wlogger_console_handler.setFormatter(formatter)
wlogger.addHandler(wlogger_console_handler)
