/*
* Name: Gaurav Jain
* Entry Number: 2019CS10349
* Assignment 3 Q3
*/

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Part3");
int drop_count = 0;

// ===========================================================================
// Block Diagram:
// 
//         node 1                node 3               node 2
//       TCP Source             TCP Sink            TCP Source
//    Connection: 1,2      Connection: 1,2,3       Connection: 3
//   +----------------+    +----------------+    +----------------+    
//   |    ns-3 TCP    |    |    ns-3 TCP    |    |    ns-3 TCP    |
//   +----------------+    +----------------+    +----------------+
//   | point-to-point |    | point-to-point |    | point-to-point |
//   +----------------+    +----------------+    +----------------+
//           |                 |       |                 |
//           +-----------------+       +-----------------+
//              10 Mbps, 3 ms             9 Mbps, 3 ms

// ===========================================================================

class MyApp : public Application{
public:
  MyApp ();
  virtual ~MyApp ();

  /* Register this type, return The TypeId.*/

  static TypeId GetTypeId (void);
  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp (){
  m_socket = 0;
}

/* static */
TypeId MyApp::GetTypeId (void){
  static TypeId tid = TypeId ("MyApp")
    .SetParent<Application> ()
    .SetGroupName ("Tutorial")
    .AddConstructor<MyApp> ();
  return tid;
}

void MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate){
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void MyApp::StartApplication (void){
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void MyApp::StopApplication (void){
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void MyApp::SendPacket (void){
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void MyApp::ScheduleTx (void){
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

static void CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd){
  double time = Simulator::Now().GetSeconds();
  //NS_LOG_UNCOND (time<< "\t" << newCwnd);
  *stream->GetStream () << time << "," << newCwnd << std::endl;
}

static void RxDrop (Ptr<PcapFileWrapper> file, Ptr<const Packet> p){
  drop_count++;
}

void run(int mode){
    drop_count = 0;

    // for config3, all senders are TcpNewRenoCSE
    if (mode == 2){
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewRenoCSE"));
    }

    NodeContainer nodes;
    nodes.Create (3);

    NodeContainer nodes_13 = NodeContainer (nodes.Get (0), nodes.Get (2));
    NodeContainer nodes_23 = NodeContainer (nodes.Get (1), nodes.Get (2));
  
    PointToPointHelper pointToPoint;

    std::string dr_13 = "10Mbps";
    std::string dr_23 = "9Mbps";

    std::string delay_13 = "3ms";
    std::string delay_23 = "3ms";

    pointToPoint.SetDeviceAttribute ("DataRate", StringValue (dr_13));
    pointToPoint.SetChannelAttribute ("Delay", StringValue (delay_13));
    NetDeviceContainer devices_13 = pointToPoint.Install (nodes_13);

    pointToPoint.SetDeviceAttribute ("DataRate", StringValue (dr_23));
    pointToPoint.SetChannelAttribute ("Delay", StringValue (delay_23));
    NetDeviceContainer devices_23 = pointToPoint.Install (nodes_23);

    Ptr<RateErrorModel> em1 = CreateObject<RateErrorModel> ();
    em1->SetAttribute ("ErrorRate", DoubleValue (0.00001));
    devices_13.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em1));
    devices_23.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em1));

    InternetStackHelper stack;
    stack.Install (nodes);

    Ipv4AddressHelper address;

    address.SetBase ("10.1.1.0", "255.255.255.252");
    Ipv4InterfaceContainer interfaces_13 = address.Assign (devices_13);

    address.SetBase ("10.1.2.0", "255.255.255.252");
    Ipv4InterfaceContainer interfaces_23 = address.Assign (devices_23);

    double start = 1.0;
    double end = 30.0;
    
    // two tcp-sockets for connection 1 and 2
    // one tcp-socket for connection 3
    uint16_t sinkPort1 = 5010;
    Address sinkAddress1 (InetSocketAddress (interfaces_13.GetAddress (1), sinkPort1));
    PacketSinkHelper packetSinkHelper1 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort1));
    ApplicationContainer sinkApps1 = packetSinkHelper1.Install (nodes.Get (2));
    sinkApps1.Start (Seconds (start));
    sinkApps1.Stop (Seconds (end));

    uint16_t sinkPort2 = 5008;
    Address sinkAddress2 (InetSocketAddress (interfaces_13.GetAddress (1), sinkPort2));
    PacketSinkHelper packetSinkHelper2 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort2));
    ApplicationContainer sinkApps2 = packetSinkHelper2.Install (nodes.Get (2));
    sinkApps2.Start (Seconds (start));
    sinkApps2.Stop (Seconds (end));

    uint16_t sinkPort3 = 5006;
    Address sinkAddress3 (InetSocketAddress (interfaces_23.GetAddress (1), sinkPort3));
    PacketSinkHelper packetSinkHelper3 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort3));
    ApplicationContainer sinkApps3 = packetSinkHelper3.Install (nodes.Get (2));
    sinkApps3.Start (Seconds (start));
    sinkApps3.Stop (Seconds (end));

    AsciiTraceHelper asciiTraceHelper;
    
    std::string con1 = "plots/part3/config_" + std::to_string(mode+1) + "_con1.csv";
    std::string con2 = "plots/part3/config_" + std::to_string(mode+1) + "_con2.csv";
    std::string con3 = "plots/part3/config_" + std::to_string(mode+1) + "_con3.csv";

    Ptr<OutputStreamWrapper> stream1 = asciiTraceHelper.CreateFileStream (con1);
    Ptr<Socket> ns3TcpSocket0 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
    ns3TcpSocket0->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream1));
  
    Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper.CreateFileStream (con2);
    Ptr<Socket> ns3TcpSocket1 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
    ns3TcpSocket1->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream2));

    // for config2, sender of connection 1 use TcpNewRenoCSE
    if (mode == 1){
      TypeId tid = TypeId::LookupByName ("ns3::TcpNewRenoCSE");
      std::stringstream nodeId;
      nodeId << nodes.Get (1)->GetId ();
      
      Config::Set ("/NodeList/" + nodeId.str () + "/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tid));
    }
    
    Ptr<OutputStreamWrapper> stream3 = asciiTraceHelper.CreateFileStream (con3);
    Ptr<Socket> ns3TcpSocket2 = Socket::CreateSocket (nodes.Get (1), TcpSocketFactory::GetTypeId ());
    ns3TcpSocket2->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream3));

    Ptr<MyApp> app1 = CreateObject<MyApp> ();
    app1->Setup (ns3TcpSocket0, sinkAddress1, 3000, 100000, DataRate ("1.5Mbps"));
    nodes.Get (0)->AddApplication (app1);
    app1->SetStartTime (Seconds (1.));
    app1->SetStopTime (Seconds (20.));

    Ptr<MyApp> app2 = CreateObject<MyApp> ();
    app2->Setup (ns3TcpSocket1, sinkAddress2, 3000, 100000, DataRate ("1.5Mbps"));
    nodes.Get (0)->AddApplication (app2);
    app2->SetStartTime (Seconds (5.));
    app2->SetStopTime (Seconds (25.));

    Ptr<MyApp> app3 = CreateObject<MyApp> ();
    app3->Setup (ns3TcpSocket2, sinkAddress3, 3000, 100000, DataRate ("1.5Mbps"));
    nodes.Get (1)->AddApplication (app3);
    app3->SetStartTime (Seconds (15.));
    app3->SetStopTime (Seconds (30.));

    Ptr<PcapFileWrapper> file;
    devices_13.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback(&RxDrop,file));
    devices_23.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback(&RxDrop,file));

    Simulator::Stop (Seconds (30));
    Simulator::Run ();
    Simulator::Destroy ();

    std::cout<<"Configuration: "<<mode+1<<"\n";

    std::cout<<"Output saved in ../plots/part3/config_" << std::to_string(mode+1) << "_con1.csv (Connection 1)\n";
    std::cout<<"Output saved in ../plots/part3/config_" << std::to_string(mode+1) << "_con2.csv (Connection 2)\n";
    std::cout<<"Output saved in ../plots/part3/config_" << std::to_string(mode+1) << "_con3.csv (Connection 3)\n";

    std::cout<<"Number of packets dropped: "<<drop_count<<"\n";  
    std::cout<<"------------------------------------------------------------------\n";
}

int main (int argc, char *argv[]){
  CommandLine cmd;
  cmd.Parse (argc, argv);

    //run(0);
    run(2);
    //run(2);

  return 0;
}