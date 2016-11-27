from WebAdapter import UrlHandler
from ArduinoConnector import ArduinoConnector
import logging
import time
import json

logger = logging.getLogger()

class Unit:
    def __init__(self, id):
        self.id = id
        self.last_mark = time.time()
        self.connector = ArduinoConnector(self.id)

class Metrics:
    def __init__(self, measure_start, measure_end, power_usage):
        self.measure_start = measure_start
        self.measure_end = measure_end
        self.power_usage = power_usage


class Proccessor:

    def __init__(self):
        logger.debug("Starting")

        self.const_timer_action = 1
        self.const_timer_usage = 5

        serverUrl = 'http://homedash.cloudapp.net/api/'
        self.http_client = UrlHandler(serverUrl, "", "")
        self.units = [Unit('lukoUnit')]

    def main(self):
        timer_action = self.const_timer_action
        timer_usage = self.const_timer_usage
        while True:
            timer_action -= 1
            timer_usage -= 1

            if timer_action <= 0:
                self.process_actions()
                timer_action = self.const_timer_action

            if timer_usage <= 0:
                self.process_usage()
                timer_usage = self.const_timer_usage

            time.sleep(1)


    def process_actions(self):
        for unit in self.units:
            response = self.http_client.post('actions/',{"unitIds": [unit.id]})
            json_data = json.loads(response.decode('utf-8'))
            print(json_data)
            for unitsWithActions in json_data['unitActions']:
                for action in unitsWithActions['actions']:
                    logger.debug(action)
                    if action == 'ON':
                        self.connector.switch(True)
                    elif action == 'OFF':
                        self.connector.switch(False)
                    else:
                        raise NotImplementedError

                    time.sleep(0.1)

    def process_usage(self):
        for unit in self.units:
            power_usage = unit.connector.read_power_usage()
            measure_end = time.time()
            self.post_metrics(unit.id, unit.last_mark, measure_end, power_usage)
            unit.last_mark = measure_end

    def post_metrics(self, unit_id, measure_start, measure_end, power_usage):
        self.http_client.post('measurements/',{
            "units": [{
                "unitId": unit_id,
                "measureStart": measure_start,
                "measureEnd": measure_end,
                "power": power_usage
        }]})

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')
    processor = Proccessor()
    processor.main()

