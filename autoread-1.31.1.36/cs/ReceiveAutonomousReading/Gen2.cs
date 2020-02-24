/*
 * Copyright (c) 2009 ThingMagic, Inc.
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
using System;
using System.Collections.Generic;
using System.Text;

namespace ThingMagic
{
    /// <summary>
    /// Gen2 protocol-specific constructs
    /// </summary>
    public static class Gen2
    {

        #region TagData

        /// <summary>
        /// Gen2-specific version of TagData
        /// </summary>
        public class TagData : ThingMagic.TagData
        {
            #region Fields

            internal byte[] _pc;

            #endregion

            #region Properties

            /// <summary>
            /// Tag's RFID protocol
            /// </summary>
            public override TagProtocol Protocol
            {
                get { return TagProtocol.GEN2; }
            }

            /// <summary>
            /// PC (Protocol Control) bits
            /// </summary>
            public byte[] PcBytes
            {
                get { return (null != _pc) ? (byte[])_pc.Clone() : null; }
            }

            #endregion

            #region Construction

            /// <summary>
            /// Create TagData with blank CRC
            /// </summary>
            /// <param name="epcBytes">EPC value</param>
            public TagData(ICollection<byte> epcBytes) : base(epcBytes) { }

            /// <summary>
            /// Create TagData
            /// </summary>
            /// <param name="epcBytes">EPC value</param>
            /// <param name="crcBytes">CRC value</param>
            public TagData(ICollection<byte> epcBytes, ICollection<byte> crcBytes) : base(epcBytes, crcBytes) { }

            /// <summary>
            /// Create TagData
            /// </summary>
            /// <param name="epcBytes">EPC value</param>
            /// <param name="crcBytes">CRC value</param>
            /// <param name="pcBytes">PC value</param>
            public TagData(ICollection<byte> epcBytes, ICollection<byte> crcBytes, ICollection<byte> pcBytes)
                : base(epcBytes, crcBytes)
            {
                _pc = (null != pcBytes) ? CollUtil.ToArray(pcBytes) : null;
            }

            #endregion
        }

        #endregion

        #region TagReadData

        /// <summary>
        /// Gen2-specific version of TagData
        /// </summary>
        public class TagReadData : ThingMagic.ProtocolTagReadData
        {
            #region Fields

            /// <summary>
            /// Gen2 Q
            /// </summary>
            public Gen2.StaticQ _q = new Gen2.StaticQ(0);

            /// <summary>
            /// Gen2.TagReadData
            /// </summary>
            public Gen2.Target _target = 0;
            /// <summary>
            /// Gen2 LinkFrequency
            /// </summary>
            public Gen2.LinkFrequency _lf = 0;

            #endregion

            #region Properties

            /// <summary>
            /// Gen2 Q when tag was read
            /// </summary>
            public Gen2.StaticQ Q
            {
                get { return _q; }
            }

            /// <summary>
            /// Gen2 Link Frequency when tag was read
            /// </summary>
            public Gen2.LinkFrequency LF
            {
                get { return _lf; }
            }

            /// <summary>
            /// Gen2 Target when tag was read
            /// </summary>
            public Gen2.Target Target
            {
                get { return _target; }
            }

            #endregion

            #region Construction

            /// <summary>
            /// Create TagReadData
            /// </summary>
            public TagReadData()
            {
            }

            #endregion
        }

        #endregion

        #region LinkFrequency
        /// <summary>
        /// Gen2 LinkFrequency
        /// </summary>
        public enum LinkFrequency
        {
            /// <summary>
            ///LinkFrequency=250KHZ
            /// </summary>
            LINK250KHZ = 250,
            /// <summary>
            ///LinkFrequency=320KHZ
            /// </summary>
            LINK320KHZ = 320,
            /// <summary>
            /// LinkFrequency=640KHZ
            /// </summary>
            LINK640KHZ = 640,
        }

        #endregion

        #region Target

        /// <summary>
        /// Gen2 target settings.
        /// Includes standard A(0) and B(1), as well as
        /// ThingMagic reader values of A-then-B, and B-then-A
        /// </summary>
        public enum Target
        {
            /// <summary>
            /// Search for tags in State A
            /// </summary>
            A,
            /// <summary>
            /// Search for tags in State B
            /// </summary>
            B,
            /// <summary>
            /// Search for tags in State A, then switch to B
            /// </summary>
            AB,
            /// <summary>
            /// Search for tags in State B, then switch to A
            /// </summary>
            BA
        }

        #endregion

        #region StaticQ

        /// <summary>
        /// Gen2 Static Q subclass.
        /// </summary>
        public class StaticQ : Q
        {
            #region Fields
            /// <summary>
            /// The Q value to use
            /// </summary>
            public byte InitialQ;

            #endregion

            #region Construction

            /// <summary>
            /// Create a static Q algorithim instance with a particular value.
            /// </summary>
            /// <param name="initQ">Q value</param>
            public StaticQ(byte initQ)
            {
                InitialQ = initQ;
            }

            #endregion

            #region ToString

            /// <summary>
            /// Human-readable representation
            /// </summary>
            /// <returns>Human-readable representation</returns>
            public override string ToString()
            {
                return String.Format("{0:D}", InitialQ);
            }

            #endregion
        }

        #endregion

        #region Q

        /// <summary>
        /// Abstract Gen2 Q class.
        /// </summary>
        public class Q { }

        #endregion

    }
}