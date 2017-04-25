#!/usr/bin/env python3

"""
Server for BigClown Sigfox presenter

Usage:
  ./server.py [options]

Options:
  -p PORT --port=PORT          Listen port (default is 8080).
  -D --debug                   Print debug messages.
  --help                       Show this message.
"""

from pynput.keyboard import Key, Controller
from docopt import docopt
from logging import DEBUG, INFO
import logging as log
from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib

LOG_FORMAT = '%(asctime)s %(levelname)s: %(message)s'

keyboard = Controller()

arguments = docopt(__doc__)
opts = {k.lstrip('-').replace('-', '_'): v
        for k, v in arguments.items() if v}

log.basicConfig(level=DEBUG if opts.get('debug') else INFO, format=LOG_FORMAT)

translate = {Key.right: "Right", Key.left: "Left"}


class HTTPServer_RequestHandler(BaseHTTPRequestHandler):

    def do_GET(self):

        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

        message = "hi"
        key = None

        if self.path.startswith('/next'):
            key = Key.right
        elif self.path.startswith('/prev'):
            key = Key.left
        else:
            params = urllib.parse.parse_qs(self.path[self.path.find('?')+1:])
            print('params', params)
            if 'data' in params:
                if params['data'] == ['00']:
                    key = Key.left
                elif params['data'] == ['01']:
                    key = Key.right

        if key:
            keyboard.press(key)
            keyboard.release(key)
            message = "key " + translate[key]

        self.wfile.write(bytes(message, "utf8"))
        return


log.info('starting server...')
port = opts.get('port', 8080)
httpd = HTTPServer(('0.0.0.0', port), HTTPServer_RequestHandler)
log.info('running server http://localhost:%i' % port)

try:
    httpd.serve_forever()
except KeyboardInterrupt:
    pass
httpd.server_close()
