using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using omg.org.CosNaming;
using System.Runtime.Remoting;
using Ch.Elca.Iiop.Services;
using System.Net.Sockets;

namespace DistFileServer
{
    public static class InitialRefLineParser
    {
        public static NamingContext getNamingServiceForArgs(string [] args)
        {
            String defaultnamingport = "1105";
            String defaultnaminghost = "localhost";
            String defaultnamingref = null;

            for (int i = 0; i < args.Length; i++ )
            {
                string item = args[i];
                if (item.Equals("-ORBInitRef", StringComparison.InvariantCultureIgnoreCase))
                {
                    defaultnamingref = args[i+1];
                    break;
                }
                else

                    if (item.Equals("-ORBInitialHost", StringComparison.InvariantCultureIgnoreCase))
                    {
                        defaultnaminghost = args[i + 1];
                    }
                    else

                        if (item.Equals("-ORBInitialPort", StringComparison.InvariantCultureIgnoreCase))
                        {
                            defaultnamingport = args[i + 1];
                        }
            }

            if (defaultnamingref != null)
            {
                return (NamingContext)RemotingServices.Connect(typeof(NamingContext), defaultnamingref);
            }
            else if (defaultnaminghost != null && defaultnamingport != null)
            {

                try
                {
                    TcpClient tc = new TcpClient(defaultnaminghost, Int32.Parse(defaultnamingport));
                }
                catch (Exception)
                {                    
                    return null;
                }
                
                string namingcorbloc = String.Format("corbaloc:iiop:1.0@{0}:{1}/NameService", defaultnaminghost, defaultnamingport);
                return (NamingContext)RemotingServices.Connect(typeof(NamingContext), namingcorbloc);
            }
            else
                return null;

        }
    }
}
