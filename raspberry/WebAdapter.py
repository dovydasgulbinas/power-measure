import logging
from datetime import datetime
import urllib
import urllib.parse
import urllib.request
from urllib.error import URLError, HTTPError
import json


logger = logging.getLogger()

class UrlHandler:
    # FIXME: Add a post object from dispatch sms
    def __init__(self, url, header_field_dict, api_dic):
        logger.debug("Initiating UrlHandler")
        self.url = url
        self.header_field_dict = header_field_dict
        self.api_dic = api_dic

    def get(self, relative_path):
        """Calls a url using a GET method and print the response."""
        full_path = self.url + relative_path
        logger.debug("Calling get" + full_path)
        #with urllib.request.urlopen(full_path) as f:
        #    logger.debug(f.read(300))
        response = urllib.request.urlopen(full_path)
        json_obj = json.loads(response.read().decode('utf-8'))
        return json_obj

    def post(self, relative_path, request_body):
        """Calls a url using POST statement and print the response.
        1. Add url to request object
        2. Add data to request object
        3. Try to post .urlopen()
        """

        full_path = self.url + relative_path
        #logger.debug("Calling push" + full_path + " Content: " + request_body)

        data = json.dumps(request_body).encode('utf8')
        logger.debug("Calling push {0} Content: {1}.".format(full_path, data))

        headers = {}
        headers['Accept'] = "*/*"
        headers['Content-type'] = "application/json"

        logger.debug("Data: {0}.".format(data))
        request = urllib.request.Request(full_path, data, headers)
        #request.type = "post"

        #for key, value in self.header_field_dict.iteritems():
            # add static header fields to the request
        #    request.add_header(key, value)

        #request.add_data(json.dumps(request_body))

        try:
            response = urllib.request.urlopen(request)
        except HTTPError as e:
            print('The server couldn\'t fulfill the request.')
            print('Error code: ', e.code)
            print('Error message: ', e.msg)
        except URLError as e:
            print('We failed to reach a server.')
            print('Reason: ', e.reason)
        else:
            body = response.read()
            logger.info('POST_RESPONSE_OK: "{}"'.format(body))
            return body




if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')
