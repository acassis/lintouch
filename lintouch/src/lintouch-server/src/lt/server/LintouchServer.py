# $Id: LintouchServer.py 362 2005-02-25 16:06:12Z mman $
#
#   FILE: LintouchServer.py --
# AUTHOR: Jiri Barton <jbar@swac.cz>
#   DATE: 01 February 2004
#
# Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
# Copyright (c) 2001-2005 S.W.A.C. Bohemia s.r.o., Czech Republic.
#
# This application is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published
# by the Free Software Foundation; either version 2 of the License, or (at
# your option) any later version.
#
# This application is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public
# License along with this application; if not, write to the
# Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.

import getopt
import sys
import os.path
import threading
import urlparse
import traceback
import time

from apr.general import apr_initialize
from lt.cp import lt_cp_initialize
from lt.vars import *
from xml.dom.minidom import parseString
from zipfile import ZipFile
from zipfile import BadZipfile

import LintouchCommServer
import LintouchHTTPServer
import ServerPlugin
import Localizator
import config

# prepend path where we install plugins
sys.path.insert(0,
        os.path.join(config.prefix, "lib", "lintouch", "server-plugins" ) )
sys.path.insert(0,
        os.path.join(config.appdir, "..", "lib", "lintouch", "server-plugins" ) )

def banner():
    print "Lintouch Server (%s)" % config.version

def usage():
    print "usage is:", \
            sys.argv[0], "[--debug level] [--port port] project.ltp"

def parse_commandline():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "h:dp:",
            ["help","port=", "debug="])
    except getopt.GetoptError:
        usage()
        print "\nInvalid options specified..., quitting"
        sys.exit(2)

    loglevel = 0

    options = {}
    options['project'] = ""

    if len(args) > 0:
        options['project'] = args[0]
    else:
        usage()
        print "\nNo project.ltp specified..., quitting"
        sys.exit(3)

    #the location of the project can be remote in the future
    isremote = bool(urlparse.urlparse(options['project'])[0])

    options['port'] = 5555
    for o, a in opts:
        if o in ["-d", "--debug"]:
            loglevel = int(a)
        if o in ["-p", "--port"]:
            options['port'] = int(a)
        if o == "-h":
            usage()
            sys.exit(1)

    return options

def ParseConnection(content):
    """Parse an open XML, expecting a connection.

    content - the XML file in a string.
    """

    def ParseProperties(pd):
        result = {}
        for property_ in pd.getElementsByTagName('property'):
            result[property_.getAttribute('name').encode('utf-8')] = \
                property_.getAttribute('value').encode('utf-8')
        return result

    pd = parseString(content)

    type_ = pd.documentElement.getAttribute('type').encode('utf-8')

    properties = {}
    pdProps = pd.documentElement.getElementsByTagName('properties')
    if len(pdProps):
        properties = ParseProperties(pdProps[0])

    variabletypes, variableproperties = {}, {}
    pdVars = pd.documentElement.getElementsByTagName('variables')
    if len(pdVars):
        for variable in pdVars[0].getElementsByTagName('variable'):
            varname = variable.getAttribute('name').encode('utf-8');
            variabletypes[varname] = variable.getAttribute('type')\
                .encode('utf-8')
            variableproperties[varname] = ParseProperties(variable)

    return {
        'type': type_,
        'properties': properties,
        'vartypes': variabletypes,
        'varproperties': variableproperties }

def readproject(options):
    """Parse the project and return all the connections.

    options - dictionary of command line options.
    """

    # open the project
    try:
        f = ZipFile(options['project'], 'r')
    except (IOError, BadZipfile), e:
        print '%s: %s' % (options['project'], e)
        sys.exit(3)
    except Exception, e:
        print 'Error while open the file %s: %s' % (options['project'], e)
        sys.exit(3)

    # read and parse the project descriptor
    try:
        pd = parseString(f.read("project-descriptor.xml"))
    except KeyError:
        print 'The project descriptor missing'
        sys.exit(4)
    except Exception, e:
        print 'Error in the project descriptor: %s' % e
        sys.exit(4)

    # extract the project info
    info = dict(zip(['author', 'version', 'date', 'shortdesc'], ['<not specified>']*4))
    pi = pd.documentElement.getElementsByTagName('project-info')
    if pi:
        for name in info.keys():
            pp = pi[0].getElementsByTagName(name)
            if not pp:
                print 'Warning: the project descriptor info is missing %s' % name
                continue
            try:
                info[name] = pp[0].firstChild.nodeValue
            except Exception, e:
                pass
    else:
        print 'Warning: no project-info found in the descriptor'

    # extract the project connections
    try:
        projectconnections = pd.documentElement.getElementsByTagName(
                'project-connections')
        connections = {}
        if len(projectconnections):
            for elem in projectconnections[0].getElementsByTagName(
                    'connection'):
                try:
                    connections[elem.getAttribute('name').encode('utf-8')]\
                        = ParseConnection(f.read(elem.getAttribute('src')))
                except Exception, e:
                    print 'Error in the connection definition %s: %s' % (
                        elem.getAttribute('src'), e)

                    #FIXME: the following will be caught in the outer
                    #exception handler, with e equals to 5
                    #Error in the python exception handling?
                    sys.exit(5)
        else:
            print 'Warning: no connections defined'
    except Exception, e:
        print 'Error in the project descriptor: %s' % e
        sys.exit(4)

    f.close()
    return info, connections

def CreatePluginPool(connections, loc):
    """Load the plugin and create connections.

    connections - parsed connections
    Return a tuple
        ({connection: varset} dict, (plugin instance, thread) list)
    """

    #load the plugins
    pluginclasses = {}
    ancestor = ServerPlugin.ServerPlugin
    classtype = type(ancestor)
    for name, connection in connections.items():
        connectiontype = connection['type']
        if connectiontype in pluginclasses:
            connection['plugin'] = pluginclasses[connectiontype]
            continue

        try:
            module = __import__(connectiontype)
        except ImportError, e:
            print 'Error while loading the server plugin %s: %s' % (
                connectiontype, e)
            sys.exit(1)

        for klassname, klass in module.__dict__.items():
            if isinstance(klass, classtype) and issubclass(klass, ancestor) \
                and klass != ancestor:
                pluginclasses[connectiontype] = klass
                connection['plugin'] = klass
                break
        else:
            print 'No plugin class %s found in the file' % connectiontype
            sys.exit(2)

    result = {}
    pluginpool = []
    #create the plugin pool
    print 'Loading the plugins:',
    for name, connection in connections.items():
        varset = VarSet()
        result[name] = varset
        for varname, vartype in connection['vartypes'].items():
            tp = LT_IO_TYPE_NULL
            if vartype == "bit": tp = LT_IO_TYPE_BIT
            elif vartype == "number": tp = LT_IO_TYPE_NUMBER
            elif vartype == "string": tp = LT_IO_TYPE_STRING
            elif vartype == "sequence": tp = LT_IO_TYPE_SEQUENCE
            varset[varname] = Variable(tp)

        try:
            plugin = connection['plugin'](
                    varset,
                    connection['properties'],
                    connection['varproperties'],
                    loc)
        except Exception, e:
            ei = sys.exc_info()
            print '%s: %s\n%s' % (ei[0], ei[1], '\n'.join(
                traceback.format_tb (ei[2])))
            print '\nThere was an error while loading the plugin', \
                name, ' - please consult the errors above.'
            sys.exit(5)

        t = threading.Thread(target=plugin.run, args=(), name=name)
        t.setDaemon(True)
        print '%s (%s)  ' % (name, connection['type']),
        pluginpool.append((plugin, t))
        t.start()
    print

    return result, pluginpool

def ShutdownPlugins(pluginpool):
    """Ask the plugins to terminate.

    Wait for 4 secs, then kill 'em all.
    """

    print 'Stopping plugins.',
    for plugin, t in pluginpool:
        if plugin.__class__.__dict__.has_key('stop'):
            plugin.stop()

    for i in range(20):
        for plugin, t in pluginpool:
            if t.isAlive():
                break
        else:
            print 'All plugins stopped.'
            break
        sys.stdout.write('.')
        sys.stdout.flush()
        time.sleep(0.2)
    else:
        print 'Some plugins still running. Killing:',
        for plugin, t in pluginpool:
            if t.isAlive():
                print t.getName(),
        print

def main():
    banner()
    apr_initialize()
    lt_cp_initialize()
    options = parse_commandline()
    info, connections = readproject(options)

    try:
        loc = Localizator.Localizator(options['project'])
    except Exception, e:
        print 'Error while creating the localizator: %s' % e
        sys.exit(11)

    #print the project info
    print '''
Loaded the project %(shortdesc)s
  Author: %(author)s
  Version: %(version)s
  Date: %(date)s
''' % info

    #start plugins
    pluginvarsets, pluginpool = CreatePluginPool(connections, loc)

    #start the HTTP server
    http = LintouchHTTPServer.LintouchHTTPServer(options['project'])
    validurl = 'http://%%s:%d/%s' % (
            http.server_port,
            os.path.basename( options['project'] ) )
    print 'Lintouch HTTP Server ready for connections at', http.server_port

    #start the communication server
    try:
        comm = LintouchCommServer.LintouchCommServer(pluginvarsets,\
            validurl, options['port'])
    except Exception, e:
        try:
            if e[0] == 98:
                print 'Cannot start the Lintouch Communication Server: '\
                    'another Lintouch Server is running?'
            else:
                raise
        except:
            print 'Cannot start the Lintouch Communication Server: %s' % e
        http.server_close()
        sys.exit(7)

    print 'Lintouch Communication Server ready for connections at',\
            options['port']

    print 'Press Ctrl+C to exit'

    #the main loop
    try:
        while sys.stdin.read(1):
            pass
    except KeyboardInterrupt:
        pass 

    #finalize
    try:
      print 'Initiating shutdown sequence...'
      comm.server_close()
      http.server_close()
      ShutdownPlugins(pluginpool)
      print 'Server shut down.'
    except KeyboardInterrupt:
      print
      print 'Server killed. Unclean shutdown.'

if __name__ == "__main__":
    main()

# vim: set et ts=4 sw=4 tw=76:
# $Id: LintouchServer.py 362 2005-02-25 16:06:12Z mman $
