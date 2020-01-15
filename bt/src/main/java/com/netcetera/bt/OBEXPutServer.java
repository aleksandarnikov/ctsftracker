package com.netcetera.bt;

import java.io.IOException;
import java.io.InputStream;

import javax.bluetooth.DiscoveryAgent;
import javax.bluetooth.LocalDevice;
import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;

import com.intel.bluetooth.btspp.Connection;

public class OBEXPutServer {

  static final String serverUUID = "11111111111111111111111111111123";

  public static void main(String[] args) throws IOException {

    LocalDevice.getLocalDevice().setDiscoverable(DiscoveryAgent.GIAC);

    // SessionNotifier serverConnection =
    // (SessionNotifier) Connector.open("btspp://localhost:" + serverUUID +
    // ";name=ObexExample");
    Connection connection = (Connection) Connector.open("btspp://localhost:" + serverUUID + ";name=ObexExample");
    StreamConnection stream = connection.acceptAndOpen();
    System.out.println("connected");
    InputStream is = stream.openDataInputStream();
    stream.openDataOutputStream();
    while (true) {
      StringBuffer buf = new StringBuffer();
      int data;
      while ((data = is.read()) != -1) {
        buf.append((char) data);
      }
      System.out.println(buf);
    }
  }

}
