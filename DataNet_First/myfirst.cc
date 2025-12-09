/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
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

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");
// Define a log component name for ns-3 logging system

int
main(int argc, char* argv[])
{                                 
                                     // Assignment 2: create new variable 'nPackets' with data type 'uint32_t'
    uint32_t nPackets = 1; // DH edit - Create a new variable ‘nPackets’

    
    CommandLine cmd;
                                    // Assignment 2: add function that maps command parameter to 'nPackets' variable hint : cmd.AddValue()
                                    
    cmd.AddValue("nPackets", "Number of echo packets to send", nPackets); // DH edit - Create a new function 'cmd.AddValue'
    
    cmd.Parse(argc, argv);
    
    // CommandLine cmd
    // - Creates a command line parser to handle arguement
    //
    // cmd.AddValue("name" , "help" , value)
    // - Links command line arguement with program variable
    // - name : option name used in command line
    // - help : description string for option name
    // - value : actual variable in your code
    //
    // cmd.Parse(argc , argv)
    // - Reads the arguements passed at program execution 
    //   and updates linked variable

    Time::SetResolution(Time::NS);
    // use nanoseconds as simulation time unit
    
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    // Enable certain level logs for EchoClient / Server app
    // LogComponentEnable(component , level)
    // component : string name of ns-3 log component
    // level : level of informations to log
    

    NodeContainer nodes;
    nodes.Create(2);
    // NodeContainer nodes
    // - Holds multiple ns-3 node objects
    // - currently has 2 nodes with index 0 , 1
    // ex) nodes = [n0 , n1]
    
    // nodes.Create(n)
    // - n : number of nodes to create
   

    PointToPointHelper pointToPoint;
    // pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    // pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
    
    // PointToPointHelper pointToPoint
    // - Helper to set up P2P links between nodes
    

    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);
    
    // NetDeviceContainer
    // - Contains netDevices (one for each node)
    // pointToPoint.Install(nodes)
    // - Installs netDevice for each node and connects them with P2P channel
    // ex) devices = [netDevice0 (for n0) , netDevice1 (for n1)]
    
    InternetStackHelper stack;
    stack.Install(nodes);
    // stack.Install(nodes)
    // - Installs Internet stack (TCP/IP , UDP ...) on nodes
   
    
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    // address.SetBase(network , mask)
    // Sets the base network address and subnet mask
    // - network : base address string
    // - mask : subnet mask

    Ipv4InterfaceContainer interfaces = address.Assign(devices);
    // Ipv4InterfaceContainer
    // - stores assigned IP address
    // address.Assign(devices)
    // - Assign IP addresses to each NetDevice
    // interfaces = [IPv4 (for netDevice0) , IPv4 (for netDevice1)]

    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));
    // echoserver(9)
    // - creates UDP echoServer, server listens to port 9 
    // echoServer.Install(nodes.Get(1))
    // - Install server application on n1 node
    // serverApp starts at 1s , stops at 10s
   
    

    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9); // set the destination as node1
    echoClient.SetAttribute("MaxPackets", UintegerValue(nPackets));     // Assignment2 : use variable 'nPackets' to modify 'maxPackets' by command line DH edit - Turn the value '1' to 'nPackets'
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));
    // echoClient(interfaces.GetAddress(1) , 9)
    // client sends packet to server's IP & port 9
    // - interfaces.GetAddress(1) : get server's address
    // - 9 : destination port number
    

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));
    //echoClient.Install(nodes.Get(0))
    // - Installs client app on n0 node
    // clientApp starts at 2s , stops at 10s

    Simulator::Run();
    // Runs the simulation
    
    Simulator::Destroy();
    // Clean up resources after simulation
    return 0;
}
