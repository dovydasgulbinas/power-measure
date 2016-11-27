import logging
import random

logger = logging.getLogger('./url_jobs')

class ArduinoConnector:
    def __init__(self, unit_id):
        logger.debug("Initiating ArduinoConnector")
        self.unit_id = unit_id

    def switch(self, possition):
        logger.debug("Switching to " + possition)

    def read_power_usage(self):
        logger.debug("Reading usage")
        return 10 * random.random()


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')