/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.thingmagic;

import com.thingmagic.SerialTransport.SimpleTransportListener;

/**
 * Sample programs that pull the autonomous streaming and prints the tags
 * As there is no auto port detection in Java, this sample program expects autonomous
 * streaming port as argument.
 */
public class ReceiveAutonomousReading
{
  static SimpleTransportListener currentListener;
  static void usage()
  {
    System.out.printf("Usage: As there in auto port detection in Java, this sample program expects "
            + "autonomous streaming port as argument. \n Please provide reader URL such as /COM12 or /dev/ttyS0 "
            + "or -v /COM12 or -v /dev/ttyS0 to enable transport listener");
    System.exit(1);
  }

  public static void setTrace(SerialTransport st, String args[])
  {
    if (args[0].toLowerCase().equals("on"))
    {
        st.addTransportListener(SerialTransport.simpleTransportListener);
        currentListener = SerialTransport.simpleTransportListener;
    }
    else if (currentListener != null)
    {
        st.removeTransportListener(SerialTransport.simpleTransportListener);
    }
  }
   
    public static void main(String[] argv)
    {
        int nextarg = 0;
        boolean trace = false;
        
        try
        {
            if(argv.length < 1)
            {
                usage();
            }
            if (argv[nextarg].equals("-v"))
            {
                trace = true;
                nextarg++;
            }
            if(!argv[nextarg].startsWith("/"))
            {
               usage(); 
            }
            String readerURI = argv[nextarg];
            boolean isConnected = false;
            SerialTransport serialTransport = new SerialTransport();
            if (trace)
            {
                setTrace(serialTransport, new String[] {"on"});
            }
            isConnected = serialTransport.connect(readerURI);
            if (isConnected)
            {
                serialTransport.receiveStreaming();
            }
        }
        catch (ReaderException re)
        {
            re.printStackTrace();
        } 
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
    }
}
