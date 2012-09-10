using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DistFileServer.Server;
using System.Runtime.Remoting;
using omg.org.CosNaming;
using Ch.Elca.Iiop.Services;
using Ch.Elca.Iiop;
using System.Runtime.Remoting.Channels;

namespace DistFileServer.IIOPInterface
{
    public sealed class DFSManager : MarshalByRefObject, DFS.DFSmanager, IDisposable
    {
        DFServer server;
        string id;
        ObjRef objrefWellKnown = null;
        NamingContext nameService = null;

        public DFSManager(DFServer srv, string id)
        {
            this.server = srv;
            this.id = id;

            server.registerInterface(this);

            objrefWellKnown = RemotingServices.Marshal(this, id);
        }

        public DFSManager(DFServer srv, string id, NamingContext nc)
        {
            this.server = srv;
            this.id = id;

            server.registerInterface(this);

            objrefWellKnown = RemotingServices.Marshal(this, id);

            this.nameService = nc;
            if ( nameService != null ) nameService.rebind(new NameComponent[] { new NameComponent(id) }, this);
        }

        public void Dispose()
        {
            RemotingServices.Unmarshal(objrefWellKnown);
            if (nameService != null)
            {
                nameService.unbind(new NameComponent[] { new NameComponent(id) });
            }

        }

        public void addNode(string nodename)
        {
            server.addNode(nodename);
        }

        public string[] listNodes()
        {
            return server.listNodes();
        }

        public void removeNode(string nodename)
        {
            server.removeNode(nodename);
        }

        public void shutdownDFS()
        {
            server.shutdownDFS();
        }

    }
}
