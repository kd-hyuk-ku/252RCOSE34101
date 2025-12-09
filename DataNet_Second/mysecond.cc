/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1   n2   n3   n4
//    point-to-point  |    |    |    |
//                    ================
//                      LAN 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SecondScriptExample");

int
main(int argc, char* argv[])
{
    bool verbose = true;
    uint32_t nCsma = 3; 
    uint32_t nPackets = 1; 

    CommandLine cmd(__FILE__);
    cmd.AddValue("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);
    cmd.AddValue("nPackets", "Number of packets to echo", nPackets);

    cmd.Parse(argc, argv);

    if (verbose)
    {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    } // Setting logs

    nCsma = nCsma == 0 ? 1 : nCsma; // Ensure there is at least one CSMA-only node

    // Nodes
    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    NodeContainer csmaNodes;
    csmaNodes.Add(p2pNodes.Get(0)); //**Hint**
    csmaNodes.Create(nCsma);
  
    // Links – p2p
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps")); //**Hint**
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes);
    
    // Links – csma
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps")); 
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));
    
    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install(csmaNodes);

    // Internet stack
    InternetStackHelper stack;
    stack.Install(p2pNodes.Get(1)); //**Hint**
    stack.Install(csmaNodes);

    // Addressing
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign(p2pDevices);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address.Assign(csmaDevices);

    // Applications - Echo server 
    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(csmaNodes.Get(3)); //**Hint**
    serverApps.Start(Seconds(1.0)); // Start after interfaces are up
    serverApps.Stop(Seconds(10.0));

    // Applications - Echo client
    UdpEchoClientHelper echoClient(csmaInterfaces.GetAddress(3), 9); //**Hint**
    
    echoClient.SetAttribute("MaxPackets", UintegerValue(nPackets)); //**Hint**
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(p2pNodes.Get(1)); //**Hint**
    clientApps.Start(Seconds(2.0)); // Ensure server is ready
    clientApps.Stop(Seconds(10.0));

    // Routing must happen after IP assignment and before Run
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    //High-level tracing 
    pointToPoint.EnablePcapAll("second");
    csma.EnablePcap("second", csmaDevices.Get(1), true);

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
