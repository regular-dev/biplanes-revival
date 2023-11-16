/*
  Simple Network Library from "Networking for Game Programmers"
  http://www.gaffer.org/networking-for-game-programmers
  Author: Glenn Fiedler <gaffer@gaffer.org>
*/

#ifndef NET_H
#define NET_H

#define PLATFORM_WINDOWS  1
#define PLATFORM_UNIX     2

#if defined(_WIN32)

  #define PLATFORM PLATFORM_WINDOWS
  #include <winsock2.h>

#elif defined(__linux__) || defined(__APPLE__) || defined(__MACH__)

  #define PLATFORM PLATFORM_UNIX

  #include <fcntl.h>
  #include <unistd.h>
  #include <arpa/inet.h>

#else
  static_assert(false, "Net.h is incompatible with your system");

#endif

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <list>
#include <vector>

#include <include/utility.hpp>


namespace net
{
  // internet address

  class Address
  {
  public:

    Address()
    {
      address = 0;
      port = 0;
    }

    Address( unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port )
    {
      this->address = ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d;
      this->port = port;
    }

    Address( unsigned int address, unsigned short port )
    {
      this->address = address;
      this->port = port;
    }

    unsigned int GetAddress() const
    {
      return address;
    }

    std::string GetA() const
    {
      return std::to_string( (uint8_t) ( address >> 24 ) );
    }

    std::string GetB() const
    {
      return std::to_string( (uint8_t) ( address >> 16 ) );
    }

    std::string GetC() const
    {
      return std::to_string( (uint8_t) ( address >> 8 ) );
    }

    std::string GetD() const
    {
      return std::to_string( (uint8_t) ( address ) );
    }

    unsigned short GetPort() const
    {
      return port;
    }

    bool operator == ( const Address & other ) const
    {
      return address == other.address && port == other.port;
    }

    bool operator != ( const Address & other ) const
    {
      return ! ( *this == other );
    }

    bool operator < ( const Address & other ) const
    {
      // note: this is so we can use address as a key in std::map
      if ( address < other.address )
        return true;
      if ( address > other.address )
        return false;
      else
        return port < other.port;
    }

  private:

    unsigned int address;
    unsigned short port;
  };

  // sockets

  inline bool InitializeSockets()
  {
    #if PLATFORM == PLATFORM_WINDOWS
      WSADATA WsaData;
      return WSAStartup( MAKEWORD( 2,2 ), &WsaData ) != 0;

    #else
      return false;

    #endif
  }


  inline void ShutdownSockets()
  {
    #if PLATFORM == PLATFORM_WINDOWS
      WSACleanup();
    #endif
  }


  class Socket
  {
  public:

    Socket()
    {
      socketHandle = 0;
    }

    ~Socket()
    {
      Close();
    }

    bool Open( unsigned short port )
    {
      assert( !IsOpen() );

      // create socket

      socketHandle = ::socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );


      if ( socketHandle <= 0 )
      {
        log_message( "NETWORK: Failed to create socket!\n" );
        socketHandle = 0;
        return false;
      }

      // bind to port

      sockaddr_in address;
      address.sin_family = AF_INET;
      address.sin_addr.s_addr = INADDR_ANY;
      address.sin_port = htons( (unsigned short) port );

      if ( bind( socketHandle, (const sockaddr*) &address, sizeof(sockaddr_in) ) < 0 )
      {
        log_message( "NETWORK: Failed to bind a socket!\n" );
        Close();
        return false;
      }

      // set non-blocking io

      #if PLATFORM == PLATFORM_WINDOWS

        DWORD nonBlocking = 1;

        if ( ioctlsocket( socketHandle, FIONBIO, &nonBlocking ) != 0 )
      #else

        int nonBlocking = 1;

        if ( fcntl( socketHandle, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
      #endif
        {
          log_message( "NETWORK: Failed to set Non-Blocking mode for a socket!\n" );
          Close();
          return false;
        }

      return true;
    }

    void Close()
    {
      if ( socketHandle != 0 )
      {
        #if PLATFORM == PLATFORM_WINDOWS
          closesocket( socketHandle );

        #else
          close( socketHandle );

        #endif

        socketHandle = 0;
      }
    }

    bool IsOpen() const
    {
      return socketHandle != 0;
    }

    bool Send( const Address & destination, const void * data, int size )
    {
      assert( data );
      assert( size > 0 );

      if ( socketHandle == 0 )
        return false;

      assert( destination.GetAddress() != 0 );
      assert( destination.GetPort() != 0 );

      sockaddr_in address;
      address.sin_family = AF_INET;
      address.sin_addr.s_addr = htonl( destination.GetAddress() );
      address.sin_port = htons( (unsigned short) destination.GetPort() );

      int sent_bytes = sendto( socketHandle, (const char*)data, size, 0, (sockaddr*)&address, sizeof(sockaddr_in) );

      return sent_bytes == size;
    }

    int Receive( Address & sender, void * data, int size )
    {
      assert( data );
      assert( size > 0 );

      if ( socketHandle == 0 )
        return false;

      #if PLATFORM == PLATFORM_WINDOWS
      typedef int socklen_t;
      #endif

      sockaddr_in from;
      socklen_t fromLength = sizeof( from );

      int received_bytes = recvfrom( socketHandle, (char*)data, size, 0, (sockaddr*)&from, &fromLength );

      if ( received_bytes <= 0 )
        return 0;

      unsigned int address = ntohl( from.sin_addr.s_addr );
      unsigned short port = ntohs( from.sin_port );

      sender = Address( address, port );

      return received_bytes;
    }


  private:

    int socketHandle;
  };

  // connection

  class Connection
  {
  public:

    Socket socket {};

    enum Mode
    {
      None,
      Client,
      Server
    };

    Connection( unsigned int protocolId, float timeout )
    {
      this->protocolId = protocolId;
      this->timeout = timeout;
      mode = None;
      running = false;
      ClearData();
    }

    virtual ~Connection()
    {
      if ( IsRunning() )
        Stop();
    }

    bool Start( int port )
    {
      if ( !running )
      {
        log_message( "NETWORK: Opening connection on port ", std::to_string(port), "...\n" );
        if ( !socket.Open( port ) )
        {
          log_message( "NETWORK: Could not start connection on port ", std::to_string(port), "\n" );
          return false;
        }
        running = true;
        OnStart();
        return true;
      }
      else
        return true;
    }

    void Stop()
    {
      if ( running )
      {
        log_message( "NETWORK: Ceasing connection\n" );

        bool connected = IsConnected();
        ClearData();
        socket.Close();
        running = false;
        if ( connected )
          OnDisconnect();
        OnStop();
      }
    }

    bool IsRunning() const
    {
      return running;
    }

    void Listen()
    {
      log_message( "NETWORK: Server is listening for connections...\n" );
      bool connected = IsConnected();
      ClearData();
      if ( connected )
        OnDisconnect();
      mode = Server;
      state = Listening;
    }

    void Connect( const Address & address )
    {
      std::string address_buf;
      address_buf =   address.GetA() + "." +
                      address.GetB() + "." +
                      address.GetC() + "." +
                      address.GetD() + ":";
      address_buf +=  std::to_string( address.GetPort() );
      log_message( "NETWORK: Client is connecting to ", address_buf, "...\n" );

      bool connected = IsConnected();
      ClearData();
      if ( connected )
        OnDisconnect();
      mode = Client;
      state = Connecting;
      this->address = address;
    }

    bool IsConnecting() const
    {
      return state == Connecting;
    }

    bool ConnectFailed() const
    {
      return state == ConnectFail;
    }

    bool ConnectTimedOut() const
    {
      return state == ConnectTimeout;
    }

    bool ConnectHasErrors() const
    {
      return state == ConnectFail || state == ConnectTimeout;
    }

    bool IsConnected() const
    {
      return state == Connected;
    }

    bool IsListening() const
    {
      return state == Listening;
    }

    Mode GetMode() const
    {
      return mode;
    }

    virtual void Update( const double deltaTime )
    {
      assert( running );
      timeoutAccumulator += deltaTime;
      if ( timeoutAccumulator > timeout )
      {
        if ( state == Connecting )
        {
          log_message( "NETWORK: Connection failed!\n" );
          ClearData();
          state = ConnectFail;
          OnDisconnect();
        }
        else if ( state == Connected )
        {
          log_message( "NETWORK: Connection timed out!\n" );
          ClearData();
          if ( state == Connecting )
            state = ConnectTimeout;
          OnDisconnect();
        }
      }
    }

    virtual bool SendPacket( const unsigned char data[], int size )
    {
      assert( running );
      if ( address.GetAddress() == 0 )
        return false;
      unsigned char packet[size+4];
      packet[0] = (unsigned char) ( protocolId >> 24 );
      packet[1] = (unsigned char) ( ( protocolId >> 16 ) & 0xFF );
      packet[2] = (unsigned char) ( ( protocolId >> 8 ) & 0xFF );
      packet[3] = (unsigned char) ( ( protocolId ) & 0xFF );
      memcpy( &packet[4], data, size );
      return socket.Send( address, packet, size + 4 );
    }

    virtual int ReceivePacket( unsigned char data[], int size )
    {
      assert( running );
      unsigned char packet[size+4];
      Address sender;
      int bytes_read = socket.Receive( sender, packet, size + 4 );
      if ( bytes_read == 0 )
        return 0;
      if ( bytes_read <= 4 )
        return 0;
      if ( packet[0] != (unsigned char) ( protocolId >> 24 ) ||
         packet[1] != (unsigned char) ( ( protocolId >> 16 ) & 0xFF ) ||
         packet[2] != (unsigned char) ( ( protocolId >> 8 ) & 0xFF ) ||
         packet[3] != (unsigned char) ( protocolId & 0xFF ) )
        return 0;
      if ( mode == Server && !IsConnected() )
      {
        state = Connected;
        address = sender;
        std::string address_buf;
        address_buf =   address.GetA() + ".";
        address_buf +=  address.GetB() + ".";
        address_buf +=  address.GetC() + ".";
        address_buf +=  address.GetD() + ":";
        address_buf +=  std::to_string(address.GetPort());
        log_message( "NETWORK: New client connected from ", address_buf, "\n" );
        OnConnect();
      }
      if ( sender == address )
      {
        if ( mode == Client && state == Connecting )
        {
          log_message( "NETWORK: Successfully connected to server!\n" );
          state = Connected;
          OnConnect();
        }
        timeoutAccumulator = 0.0f;
        memcpy( data, &packet[4], bytes_read - 4 );
        return bytes_read - 4;
      }
      return 0;
    }

    int GetHeaderSize() const
    {
      return 4;
    }

  protected:

    virtual void OnStart()		{}
    virtual void OnStop()		{}
    virtual void OnConnect()    {}
    virtual void OnDisconnect() {}

  private:

    void ClearData()
    {
      state = Disconnected;
      timeoutAccumulator = 0.0f;
      address = Address();
    }

    enum State
    {
      Disconnected,
      Listening,
      Connecting,
      ConnectFail,
      ConnectTimeout,
      Connected
    };

    unsigned int protocolId;
    float timeout;

    bool running;
    Mode mode;
    State state;
    float timeoutAccumulator;
    Address address;
  };

  // packet queue to store information about sent and received packets sorted in sequence order
  //  + we define ordering using the "sequence_more_recent" function, this works provided there is a large gap when sequence wrap occurs

  struct PacketData
  {
    unsigned int sequence;			// packet sequence number
    float time;					    // time offset since packet was sent or received (depending on context)
    int size;						// packet size in bytes
  };

  inline bool sequence_more_recent( unsigned int s1, unsigned int s2, unsigned int max_sequence )
  {
    return ( s1 > s2 ) && ( s1 - s2 <= max_sequence/2 ) || ( s2 > s1 ) && ( s2 - s1 > max_sequence/2 );
  }

  class PacketQueue : public std::list<PacketData>
  {
  public:

    bool exists( unsigned int sequence )
    {
      for ( iterator itor = begin(); itor != end(); ++itor )
        if ( itor->sequence == sequence )
          return true;
      return false;
    }

    void insert_sorted( const PacketData & p, unsigned int max_sequence )
    {
      if ( empty() )
      {
        push_back( p );
      }
      else
      {
        if ( !sequence_more_recent( p.sequence, front().sequence, max_sequence ) )
        {
          push_front( p );
        }
        else if ( sequence_more_recent( p.sequence, back().sequence, max_sequence ) )
        {
          push_back( p );
        }
        else
        {
          for ( PacketQueue::iterator itor = begin(); itor != end(); itor++ )
          {
            assert( itor->sequence != p.sequence );
            if ( sequence_more_recent( itor->sequence, p.sequence, max_sequence ) )
            {
              insert( itor, p );
              break;
            }
          }
        }
      }
    }

    void verify_sorted( unsigned int max_sequence )
    {
      PacketQueue::iterator prev = end();
      for ( PacketQueue::iterator itor = begin(); itor != end(); itor++ )
      {
        assert( itor->sequence <= max_sequence );
        if ( prev != end() )
        {
          assert( sequence_more_recent( itor->sequence, prev->sequence, max_sequence ) );
          prev = itor;
        }
      }
    }
  };

  // reliability system to support reliable connection
  //  + manages sent, received, pending ack and acked packet queues
  //  + separated out from reliable connection because it is quite complex

  class ReliabilitySystem
  {
  public:

    ReliabilitySystem( unsigned int max_sequence = 0xFFFFFFFF )
    {
      this->rtt_maximum = rtt_maximum;
      this->max_sequence = max_sequence;
      Reset();
    }

    void Reset()
    {
      local_sequence = 0;
      remote_sequence = 0;
      sentQueue.clear();
      receivedQueue.clear();
      pendingAckQueue.clear();
      ackedQueue.clear();
      sent_packets = 0;
      recv_packets = 0;
      lost_packets = 0;
      acked_packets = 0;
      sent_bandwidth = 0.0f;
      acked_bandwidth = 0.0f;
      rtt = 0.0f;
      rtt_maximum = 1.0f;
    }

    void PacketSent( int size )
    {
      if ( sentQueue.exists( local_sequence ) )
      {
          log_message( "NETWORK: Local sequence ", (const char*) local_sequence, " exists\n" );
        for ( PacketQueue::iterator itor = sentQueue.begin(); itor != sentQueue.end(); ++itor )
            log_message( " + ", (const char*) itor->sequence, "\n" );
      }
      assert( !sentQueue.exists( local_sequence ) );
      assert( !pendingAckQueue.exists( local_sequence ) );
      PacketData data;
      data.sequence = local_sequence;
      data.time = 0.0f;
      data.size = size;
      sentQueue.push_back( data );
      pendingAckQueue.push_back( data );
      sent_packets++;
      local_sequence++;
      if ( local_sequence > max_sequence )
        local_sequence = 0;
    }

    void PacketReceived( unsigned int sequence, int size )
    {
      recv_packets++;
      if ( receivedQueue.exists( sequence ) )
        return;
      PacketData data;
      data.sequence = sequence;
      data.time = 0.0f;
      data.size = size;
      receivedQueue.push_back( data );
      if ( sequence_more_recent( sequence, remote_sequence, max_sequence ) )
        remote_sequence = sequence;
    }

    unsigned int GenerateAckBits()
    {
      return generate_ack_bits( GetRemoteSequence(), receivedQueue, max_sequence );
    }

    void ProcessAck( unsigned int ack, unsigned int ack_bits )
    {
      process_ack( ack, ack_bits, pendingAckQueue, ackedQueue, acks, acked_packets, rtt, max_sequence );
    }

    void Update( const double deltaTime )
    {
      acks.clear();
      AdvanceQueueTime( deltaTime );
      UpdateQueues();
      UpdateStats();
    }

    void Validate()
    {
      sentQueue.verify_sorted( max_sequence );
      receivedQueue.verify_sorted( max_sequence );
      pendingAckQueue.verify_sorted( max_sequence );
      ackedQueue.verify_sorted( max_sequence );
    }

    // utility functions

    static bool sequence_more_recent( unsigned int s1, unsigned int s2, unsigned int max_sequence )
    {
      return ( s1 > s2 ) && ( s1 - s2 <= max_sequence/2 ) || ( s2 > s1 ) && ( s2 - s1 > max_sequence/2 );
    }

    static int bit_index_for_sequence( unsigned int sequence, unsigned int ack, unsigned int max_sequence )
    {
      assert( sequence != ack );
      assert( !sequence_more_recent( sequence, ack, max_sequence ) );
      if ( sequence > ack )
      {
        assert( ack < 33 );
        assert( max_sequence >= sequence );
        return ack + ( max_sequence - sequence );
      }
      else
      {
        assert( ack >= 1 );
        assert( sequence <= ack - 1 );
        return ack - 1 - sequence;
      }
    }

    static unsigned int generate_ack_bits( unsigned int ack, const PacketQueue & received_queue, unsigned int max_sequence )
    {
      unsigned int ack_bits = 0;
      for ( PacketQueue::const_iterator itor = received_queue.begin(); itor != received_queue.end(); itor++ )
      {
        if ( itor->sequence == ack || sequence_more_recent( itor->sequence, ack, max_sequence ) )
          break;
        int bit_index = bit_index_for_sequence( itor->sequence, ack, max_sequence );
        if ( bit_index <= 31 )
          ack_bits |= 1 << bit_index;
      }
      return ack_bits;
    }

    static void process_ack( unsigned int ack, unsigned int ack_bits,
                 PacketQueue & pending_ack_queue, PacketQueue & acked_queue,
                 std::vector<unsigned int> & acks, unsigned int & acked_packets,
                 float & rtt, unsigned int max_sequence )
    {
      if ( pending_ack_queue.empty() )
        return;

      PacketQueue::iterator itor = pending_ack_queue.begin();
      while ( itor != pending_ack_queue.end() )
      {
        bool acked = false;

        if ( itor->sequence == ack )
        {
          acked = true;
        }
        else if ( !sequence_more_recent( itor->sequence, ack, max_sequence ) )
        {
          int bit_index = bit_index_for_sequence( itor->sequence, ack, max_sequence );
          if ( bit_index <= 31 )
            acked = ( ack_bits >> bit_index ) & 1;
        }

        if ( acked )
        {
          rtt += ( itor->time - rtt ) * 0.1f;

          acked_queue.insert_sorted( *itor, max_sequence );
          acks.push_back( itor->sequence );
          acked_packets++;
          itor = pending_ack_queue.erase( itor );
        }
        else
          ++itor;
      }
    }

    // data accessors

    unsigned int GetLocalSequence() const
    {
      return local_sequence;
    }

    unsigned int GetRemoteSequence() const
    {
      return remote_sequence;
    }

    unsigned int GetMaxSequence() const
    {
      return max_sequence;
    }

    void GetAcks( unsigned int ** acks, int & count )
    {
      *acks = &this->acks[0];
      count = (int) this->acks.size();
    }

    unsigned int GetSentPackets() const
    {
      return sent_packets;
    }

    unsigned int GetReceivedPackets() const
    {
      return recv_packets;
    }

    unsigned int GetLostPackets() const
    {
      return lost_packets;
    }

    unsigned int GetAckedPackets() const
    {
      return acked_packets;
    }

    float GetSentBandwidth() const
    {
      return sent_bandwidth;
    }

    float GetAckedBandwidth() const
    {
      return acked_bandwidth;
    }

    float GetRoundTripTime() const
    {
      return rtt;
    }

    int GetHeaderSize() const
    {
      return 12;
    }

  protected:

    void AdvanceQueueTime( float deltaTime )
    {
      for ( PacketQueue::iterator itor = sentQueue.begin(); itor != sentQueue.end(); itor++ )
        itor->time += deltaTime;

      for ( PacketQueue::iterator itor = receivedQueue.begin(); itor != receivedQueue.end(); itor++ )
        itor->time += deltaTime;

      for ( PacketQueue::iterator itor = pendingAckQueue.begin(); itor != pendingAckQueue.end(); itor++ )
        itor->time += deltaTime;

      for ( PacketQueue::iterator itor = ackedQueue.begin(); itor != ackedQueue.end(); itor++ )
        itor->time += deltaTime;
    }

    void UpdateQueues()
    {
      const float epsilon = 0.001f;

      while ( sentQueue.size() && sentQueue.front().time > rtt_maximum + epsilon )
        sentQueue.pop_front();

      if ( receivedQueue.size() )
      {
        const unsigned int latest_sequence = receivedQueue.back().sequence;
        const unsigned int minimum_sequence = latest_sequence >= 34 ? ( latest_sequence - 34 ) : max_sequence - ( 34 - latest_sequence );
        while ( receivedQueue.size() && !sequence_more_recent( receivedQueue.front().sequence, minimum_sequence, max_sequence ) )
          receivedQueue.pop_front();
      }

      while ( ackedQueue.size() && ackedQueue.front().time > rtt_maximum * 2 - epsilon )
        ackedQueue.pop_front();

      while ( pendingAckQueue.size() && pendingAckQueue.front().time > rtt_maximum + epsilon )
      {
        pendingAckQueue.pop_front();
        lost_packets++;
      }
    }

    void UpdateStats()
    {
      int sent_bytes_per_second = 0;
      for ( PacketQueue::iterator itor = sentQueue.begin(); itor != sentQueue.end(); ++itor )
        sent_bytes_per_second += itor->size;
      int acked_packets_per_second = 0;
      int acked_bytes_per_second = 0;
      for ( PacketQueue::iterator itor = ackedQueue.begin(); itor != ackedQueue.end(); ++itor )
      {
        if ( itor->time >= rtt_maximum )
        {
          acked_packets_per_second++;
          acked_bytes_per_second += itor->size;
        }
      }
      sent_bytes_per_second /= rtt_maximum;
      acked_bytes_per_second /= rtt_maximum;
      sent_bandwidth = sent_bytes_per_second * ( 8 / 1000.0f );
      acked_bandwidth = acked_bytes_per_second * ( 8 / 1000.0f );
    }

  private:

    unsigned int max_sequence;			// maximum sequence value before wrap around (used to test sequence wrap at low # values)
    unsigned int local_sequence;		// local sequence number for most recently sent packet
    unsigned int remote_sequence;		// remote sequence number for most recently received packet

    unsigned int sent_packets;			// total number of packets sent
    unsigned int recv_packets;			// total number of packets received
    unsigned int lost_packets;			// total number of packets lost
    unsigned int acked_packets;			// total number of packets acked

    float sent_bandwidth;				// approximate sent bandwidth over the last second
    float acked_bandwidth;				// approximate acked bandwidth over the last second
    float rtt;							// estimated round trip time
    float rtt_maximum;					// maximum expected round trip time (hard coded to one second for the moment)

    std::vector<unsigned int> acks;		// acked packets from last set of packet receives. cleared each update!

    PacketQueue sentQueue;				// sent packets used to calculate sent bandwidth (kept until rtt_maximum)
    PacketQueue pendingAckQueue;		// sent packets which have not been acked yet (kept until rtt_maximum * 2 )
    PacketQueue receivedQueue;			// received packets for determining acks to send (kept up to most recent recv sequence - 32)
    PacketQueue ackedQueue;				// acked packets (kept until rtt_maximum * 2)
  };

  // connection with reliability (seq/ack)

  class ReliableConnection : public Connection
  {
  public:

    ReliableConnection( unsigned int protocolId, float timeout, unsigned int max_sequence = 0xFFFFFFFF )
      : Connection( protocolId, timeout ), reliabilitySystem( max_sequence )
    {
      ClearData();
    }

    ~ReliableConnection()
    {
      if ( IsRunning() )
        Stop();
    }

    // overriden functions from "Connection"

    bool SendPacket( const unsigned char data[], int size )
    {
      const int header = 12;
      unsigned char packet[header+size];
      unsigned int seq = reliabilitySystem.GetLocalSequence();
      unsigned int ack = reliabilitySystem.GetRemoteSequence();
      unsigned int ack_bits = reliabilitySystem.GenerateAckBits();
      WriteHeader( packet, seq, ack, ack_bits );
      memcpy( packet + header, data, size );
      if ( !Connection::SendPacket( packet, size + header ) )
        return false;
      reliabilitySystem.PacketSent( size );
      return true;
    }

    int ReceivePacket( unsigned char data[], int size )
    {
      const int header = 12;
      unsigned char packet[header+size];

      const bool wasConnected = IsConnected();

      int received_bytes = Connection::ReceivePacket( packet, size + header );

      if ( IsConnected() != wasConnected )
        ClearData();

      if ( received_bytes == 0 )
        return false;

      if ( received_bytes <= header )
        return false;

      unsigned int packet_sequence = 0;
      unsigned int packet_ack = 0;
      unsigned int packet_ack_bits = 0;

      ReadHeader( packet, packet_sequence, packet_ack, packet_ack_bits );
      reliabilitySystem.PacketReceived( packet_sequence, received_bytes - header );
      reliabilitySystem.ProcessAck( packet_ack, packet_ack_bits );

      memcpy( data, packet + header, received_bytes - header );
      return received_bytes - header;
    }

    void Update( const double deltaTime )
    {
      Connection::Update(deltaTime);
      reliabilitySystem.Update(deltaTime);
    }

    int GetHeaderSize() const
    {
      return Connection::GetHeaderSize() + reliabilitySystem.GetHeaderSize();
    }

    ReliabilitySystem & GetReliabilitySystem()
    {
      return reliabilitySystem;
    }


  protected:

    void WriteInteger( unsigned char * data, unsigned int value )
    {
      data[0] = (unsigned char) ( value >> 24 );
      data[1] = (unsigned char) ( ( value >> 16 ) & 0xFF );
      data[2] = (unsigned char) ( ( value >> 8 ) & 0xFF );
      data[3] = (unsigned char) ( value & 0xFF );
    }

    void WriteHeader( unsigned char * header, unsigned int sequence, unsigned int ack, unsigned int ack_bits )
    {
      WriteInteger( header, sequence );
      WriteInteger( header + 4, ack );
      WriteInteger( header + 8, ack_bits );
    }

    void ReadInteger( const unsigned char * data, unsigned int & value )
    {
      value = ( ( (unsigned int)data[0] << 24 ) | ( (unsigned int)data[1] << 16 ) |
              ( (unsigned int)data[2] << 8 )  | ( (unsigned int)data[3] ) );
    }

    void ReadHeader( const unsigned char * header, unsigned int & sequence, unsigned int & ack, unsigned int & ack_bits )
    {
      ReadInteger( header, sequence );
      ReadInteger( header + 4, ack );
      ReadInteger( header + 8, ack_bits );
    }

    virtual void OnStop()
    {
      ClearData();
    }

    virtual void OnDisconnect()
    {
      ClearData();
    }

  private:

    void ClearData()
    {
      reliabilitySystem.Reset();
    }

    ReliabilitySystem reliabilitySystem;	// reliability system: manages sequence numbers and acks, tracks network stats etc.
  };


    class FlowControl
    {
    public:

        FlowControl()
        {
            log_message( "NETWORK: Flow control initialized!\n" );
            Reset();
        }

        void Reset()
        {
            mode = Bad;
            penalty_time = 4.0f;
            good_conditions_time = 0.0f;
            penalty_reduction_accumulator = 0.0f;
        }

        void Update( const float rtt, const double deltaTime )
        {
            const float RTT_Threshold = 250.0f;

            if ( mode == Good )
            {
                if ( rtt > RTT_Threshold )
                {
                    log_message( "NETWORK: Dropping to bad mode!\n" );
                    mode = Bad;
                    if ( good_conditions_time < 10.0f && penalty_time < 60.0f )
                    {
                        penalty_time *= 2.0f;
                        if ( penalty_time > 60.0f )
                          penalty_time = 60.0f;

                        std::stringstream penalty_buf;
                        penalty_buf << std::fixed << std::setprecision(1) << penalty_time;
                        log_message( "NETWORK: Penalty time increased to ", penalty_buf.str(), " seconds\n" );
                    }
                    good_conditions_time = 0.0f;
                    penalty_reduction_accumulator = 0.0f;
                    return;
                }

                good_conditions_time += deltaTime;
                penalty_reduction_accumulator += deltaTime;

                if ( penalty_reduction_accumulator > 10.0f && penalty_time > 1.0f )
                {
                    penalty_time /= 2.0f;
                    if ( penalty_time < 1.0f )
                      penalty_time = 1.0f;

                    std::stringstream penalty_buf;
                    penalty_buf << std::fixed << std::setprecision(1) << penalty_time;
                    log_message( "NETWORK: Penalty time reduced to ", penalty_buf.str(), " seconds\n" );
                    penalty_reduction_accumulator = 0.0f;
                }
            }

            if ( mode == Bad )
            {
                if ( rtt <= RTT_Threshold )
                    good_conditions_time += deltaTime;
                else
                    good_conditions_time = 0.0f;

                if ( good_conditions_time > penalty_time )
                {
                    log_message( "NETWORK: Upgrading to good mode\n" );
                    good_conditions_time = 0.0f;
                    penalty_reduction_accumulator = 0.0f;
                    mode = Good;
                    return;
                }
            }
        }

        bool IsConnectionStable() const
        {
            return mode == Good;
        }

    private:

        enum Mode
        {
            Good,
            Bad
        };

        Mode mode;
        float penalty_time;
        float good_conditions_time;
        float penalty_reduction_accumulator;
    };
}

#endif
