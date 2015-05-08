#!/usr/bin/python2.7
"""Read the json output from the LDR2ethernet"""

#standard modules
import urllib

def simplefetch(ip, port):
  url = 'http://%s:%d' % (ip, port)
  data = urllib.urlopen(url)
  print data.read()

def main():
  """Processes commandline input to read from the ethernet module."""
  import optparse
  parser = optparse.OptionParser()
  parser.add_option('--host', default='192.168.178.210', #10.89.13.241
                    help='IP of the LDR2Ethernet device.')
  parser.add_option('-p', '--port', type='int', default=80,
                    help='Port of the LDR2Ethernet device.')
  options, arguments = parser.parse_args()
  print simplefetch(options.host, options.port)

if __name__ == '__main__':
  main()
