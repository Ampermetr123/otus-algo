import time

class Log_profile():
    def __init__(self, message):
        self.message = message
    def __enter__(self):
        self.start = time.perf_counter()
        return None  # could return anything, to be used like this: with Timer("Message") as value:
    def __exit__(self, type, value, traceback):
        run_time = time.perf_counter() - self.start
        print(" %s %.3f s  " % (self.message, run_time), flush=True)
