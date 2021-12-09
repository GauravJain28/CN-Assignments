/*
* Name: Gaurav Jain
* Entry Number: 2019CS10349
* Assignment 3 Q1
*/

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Part1");
int drop_count = 0;

// ===========================================================================
//
//         node 0                 node 1
//   +----------------+    +----------------+
//   |    ns-3 TCP    |    |    ns-3 TCP    |
//   +----------------+    +----------------+
//   |    10.1.1.1    |    |    10.1.1.2    |
//   +----------------+    +----------------+
//   | point-to-point |    | point-to-point |
//   +----------------+    +----------------+
//           |                     |
//           +---------------------+
//                8 Mbps, 3 ms

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

int main (int argc, char *argv[]){
  CommandLine cmd;
  cmd.Parse (argc, argv);

  // protocols to be checked-
    // * newreno
    // * highspeed
    // * veno
    // * vegas

  std::string prot = "HighSpeed";

    drop_count = 0;

    NodeContainer nodes;
    nodes.Create (2);                                                         // Node 1 and Node 2

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("8Mbps"));      // Data rate of link b/w node 1 and 2
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("3ms"));          // Propagation Delay

    NetDeviceContainer devices;
    devices = pointToPoint.Install (nodes);

    Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
    em->SetAttribute ("ErrorRate", DoubleValue (0.00001));                    // Error rate of Rate Error model
    devices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

    InternetStackHelper stack;
    stack.Install (nodes);

    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.252");
    Ipv4InterfaceContainer interfaces = address.Assign (devices);

    uint16_t sinkPort = 8080;
    Address sinkAddress (InetSocketAddress (interfaces.GetAddress (1), sinkPort));
    PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
    ApplicationContainer sinkApps = packetSinkHelper.Install (nodes.Get (1));
    sinkApps.Start (Seconds (1.));                                            // Start time
    sinkApps.Stop (Seconds (30.));                                            // End time

    std::string pr = "ns3::Tcp"+prot;
    std::cout<<"TCP Protocol: "<<pr<<"\n";
    TypeId tid = TypeId::LookupByName(pr);                    // Set tcp protocol

    Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tid));
    Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());

    Ptr<MyApp> app = CreateObject<MyApp> ();
    app->Setup (ns3TcpSocket, sinkAddress, 3000, 100000, DataRate ("1Mbps"));   // Application data rate, packet size, number of packets
    nodes.Get (0)->AddApplication (app);
    app->SetStartTime (Seconds (1.));                                         // Start time
    app->SetStopTime (Seconds (30.));                                         // End time

    AsciiTraceHelper asciiTraceHelper;
    prot = "tcp"+prot;
    std::string fil = "plots/part1/"+prot+".csv";
    Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (fil);
    ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));

    Ptr<PcapFileWrapper> file;
    devices.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop, file));

    Simulator::Stop (Seconds (30.));
    Simulator::Run ();
    Simulator::Destroy ();
    
    std::cout<<"Output saved in ../plots/part1/"<<prot<<".csv\n";
    std::cout<<"Number of packets dropped: "<<drop_count<<"\n";  
    std::cout<<"------------------------------------------------------------------\n";
  

  return 0;
}

