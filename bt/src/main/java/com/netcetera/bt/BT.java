package com.netcetera.bt;

import java.util.Vector;

import javax.bluetooth.BluetoothStateException;
import javax.bluetooth.LocalDevice;
import javax.bluetooth.UUID;

public class BT {

  public static final Vector<String> serviceFound = new Vector();

  static final UUID OBEX_FILE_TRANSFER = new UUID(0x1106);

  public static void main(String[] args) throws BluetoothStateException {
    LocalDevice local = LocalDevice.getLocalDevice();
    
    System.out.println(local.getBluetoothAddress());

  }

}
