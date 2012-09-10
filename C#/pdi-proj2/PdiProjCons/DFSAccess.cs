using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.Remoting;
using System.Runtime.Remoting.Services;
using System.Runtime.Remoting.Channels;
using Ch.Elca.Iiop;
using Ch.Elca.Iiop.Services;
using DistFileServer.Server;
using System.Collections;
using System.Runtime.InteropServices;
using omg.org.CosNaming;

namespace DistFileServer.IIOPInterface
{

    public sealed class DFSAccess : System.MarshalByRefObject, DFS.DFSaccess, IDisposable
    {

        DFServer server;
        string id;
        ObjRef objrefWellKnown = null;
        NamingContext nameService = null;

        public DFSAccess(DFServer srv, string id)
        {
            this.server = srv;
            this.id = id;

            server.registerInterface(this);

            objrefWellKnown = RemotingServices.Marshal(this, id);

        }

        public DFSAccess(DFServer srv, string id, NamingContext nc)
        {
            this.server = srv;
            this.id = id;

            server.registerInterface(this);
            
            objrefWellKnown = RemotingServices.Marshal(this, id);

            this.nameService = nc;
            if (nameService != null) nameService.rebind(new NameComponent[] { new NameComponent(id) }, this);
        }

        public void Dispose()
        {
            RemotingServices.Unmarshal(objrefWellKnown);
            if (nameService != null)
            {
                nameService.unbind(new NameComponent[] { new NameComponent(id) });
            }

        }

        public void addFile(string filename, byte[] content)
        {
            server.addFile(filename, content);
        }

        public byte[] getFile(string filename)
        {
            return server.getFile(filename);
        }

        public string[] getFileList()
        {
            return server.getFileList();
        }

        public void removeFile(string filename)
        {
            server.removeFile(filename);
        }
    }
}
