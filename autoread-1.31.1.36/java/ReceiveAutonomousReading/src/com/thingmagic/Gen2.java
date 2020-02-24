/*
 * Copyright (c) 2008 ThingMagic, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
package com.thingmagic;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.Map;

/**
 * This class is a namespace for Gen2-specific subclasses of generic
 * Mercury API classes, Gen2 data structures, constants, and Gen2
 * convenience functions.
 */
public class Gen2
{

  // non-public
  Gen2() { }

/**
 * This class extends {@link TagData} to represent the details of a Gen2 RFID tag.
 */
  public static class TagData extends com.thingmagic.TagData 
  {
    final byte[] pc;

    public TagData(byte[] bEPC)
    {
      super(bEPC);

      pc = new byte[2];
      pc[0] = (byte)((epc.length) << 3);
      pc[1] = 0;
    }

    public TagData(byte[] bEPC, byte[] newPC)
    {
      super(bEPC);

      pc = newPC.clone();
    }

    public TagData(byte[] bEPC, byte[] crc, byte[] newPC)
    {
      super(bEPC, crc);

      pc = newPC.clone();
    }

    public TagData(String sEPC)
    {
      super(sEPC);

      pc = new byte[2];
      pc[0] = (byte)((epc.length) << 3);
      pc[1] = 0;
    }

    public TagData(String sEPC, String sCrc)
    {
      super(sEPC,sCrc);

      pc = new byte[2];
      pc[0] = (byte)((epc.length) << 3);
      pc[1] = 0;
    }

    public TagProtocol getProtocol()
    {
      return TagProtocol.GEN2;
    }

    public byte[] pcBytes()
    {
      return pc.clone();
    }

    @Override
    boolean checkLen(int epcbytes)
    {
      if (epcbytes < 0)
      {
        return false;
      }
      if (epcbytes > 62)
      {
        return false;
      }
      if ((epcbytes & 1) != 0)
      {
        return false;
      }
    return true;
    }

    public String toString()
    {
      return String.format("GEN2:%s", epcString());
    }

  }

    /**
     * Target algorithm options
     */
    public enum Target
    {
        /**
         * Search target A until exhausted, then search target B
         */
        AB(0),
        /**
         * Search target B until exhausted, then search target A
         */
        BA(1),
        /**
         * Search target A
         */
        A(2),
        /**
         * Search target B
         */
        B(3);

        int rep;

        /* Gen2 air protocol representation of the value */
        Target(int rep)
        {
            this.rep = rep;
        }
        private static final Map<Integer, Target> lookup = new HashMap<Integer, Target>();

        static
        {
            for (Target target : EnumSet.allOf(Target.class))
            {
                lookup.put(target.getCode(), target);
            }
        }

        public int getCode()
        {
            return rep;
        }

        public static Target get(int rep)
        {
            return lookup.get(rep);
        }
    }

    /**
     * Algorithm choices for Q - superclass
     */
    public abstract static class Q
    {
    }

    /**
     * Dynamic Q algorithm
     */
    public static class DynamicQ extends Q
    {
        @Override
        public String toString()
        {
            return String.format("DynamicQ");
        }
    }

    /**
     * Static initial Q algorithm
     */
    public static class StaticQ extends Q
    {
        /**
         * The initial Q value to use
         */
        public int initialQ;

        /**
         * Create a static Q algorithm instance with a particular value.
         *
         * @param initialQ the q value
         */
        public StaticQ(int initialQ)
        {
            this.initialQ = initialQ;
        }

        @Override
        public String toString()
        {
            return String.format("%d", initialQ);
        }
    }

    /**
     * Gen2 link frequencies
     */
    public enum LinkFrequency
    {
        LINK250KHZ(250),
        LINK320KHZ(320),
        LINK640KHZ(640);

        int rep;

        LinkFrequency(int rep)
        {
            this.rep = rep;
        }

        public static LinkFrequency getFrequency(int fr)
        {
            LinkFrequency freq;
            switch (fr)
            {
                case 0:
                    freq = Gen2.LinkFrequency.LINK250KHZ;
                    break;
                case 2:
                    freq = Gen2.LinkFrequency.LINK320KHZ;
                    break;
                case 4:
                    freq = Gen2.LinkFrequency.LINK640KHZ;
                    break;
                default:
                    throw new IllegalArgumentException("Invalid Gen2 Link Frequency " + fr);
            }
            return freq;
        }

        public static int get(LinkFrequency lf)
        {
            int freq = -1;
            switch (lf)
            {
                case LINK250KHZ:
                    freq = 0;
                    break;
                case LINK320KHZ:
                    freq = 2;
                    break;
                case LINK640KHZ:
                    freq = 4;
                    break;
                default:
                    throw new IllegalArgumentException("Invalid Gen2 Link Frequency " + lf);
            }
            return freq;
        }
    }

    /**
     * A class to represent the gen2 specific data from tag response containing
     * parameters like q, target and link frequency.
     */
    public static class TagReadData extends ProtocolTagReadData
    {

        int initialQ = 0;
        public Gen2.StaticQ q = new Gen2.StaticQ(initialQ);
        public Gen2.Target target = null;
        public Gen2.LinkFrequency lf = null;

        // Non-public constructor
        TagReadData()
        {
        }

        /**
         * Return the gen2Q of the tag
         *
         * @return the Q
         */
        public Gen2.StaticQ getGen2Q()
        {
            return q;
        }

        /**
         * Return the gen2Link frequency of the tag
         *
         * @return the Gen2 Link frequency
         */
        public Gen2.LinkFrequency getGen2LF()
        {
            return lf;
        }

        /**
         * Return the Target of the tag
         *
         * @return the Target
         */
        public Gen2.Target getGen2Target()
        {
            return target;
        }
    }

}