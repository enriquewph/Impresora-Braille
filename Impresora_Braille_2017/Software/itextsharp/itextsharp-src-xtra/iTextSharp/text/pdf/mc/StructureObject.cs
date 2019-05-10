/*
 * $Id$
 *
 * This file is part of the iText (R) project.
 * Copyright (c) 1998-2016 iText Group NV
 * Authors: Bruno Lowagie, Paulo Soares, et al.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License version 3
 * as published by the Free Software Foundation with the addition of the
 * following permission added to Section 15 as permitted in Section 7(a):
 * FOR ANY PART OF THE COVERED WORK IN WHICH THE COPYRIGHT IS OWNED BY
 * ITEXT GROUP. ITEXT GROUP DISCLAIMS THE WARRANTY OF NON INFRINGEMENT
 * OF THIRD PARTY RIGHTS
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Affero General Public License for more details.
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA, 02110-1301 USA, or download the license from the following URL:
 * http://itextpdf.com/terms-of-use/
 *
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License.
 *
 * In accordance with Section 7(b) of the GNU Affero General Public License,
 * a covered work must retain the producer line in every PDF that is created
 * or manipulated using iText.
 *
 * You can be released from the requirements of the license by purchasing
 * a commercial license. Buying such a license is mandatory as soon as you
 * develop commercial activities involving the iText software without
 * disclosing the source code of your own applications.
 * These activities include: offering paid services to customers as an ASP,
 * serving PDFs on the fly in a web application, shipping iText with a closed
 * source product.
 *
 * For more information, please contact iText Software Corp. at this
 * address: sales@itextpdf.com
 */

using System;

namespace iTextSharp.text.pdf.mc
{
    public class StructureObject : StructureItem
    {
        /** The structure element of which the properties are stored. */
        protected PdfDictionary structElem;
        /** The reference of the structure element of which the properties are stored. */
        protected PdfIndirectReference refa;
        /** An object referred to by an OBJR dictionary. */
        protected PdfObject obj;
        /** The reference to the object. */
        protected PdfIndirectReference objref;
        /** The StructParent value of the object. */
        protected int structParent;

        /**
         * Creates a StructureObject for an OBJR dictionary.
         * @param structElem	the parent structure element
         * @param ref			the reference of the parent structure element
         * @param dict			the object reference dictionary
         */
        public StructureObject(PdfDictionary structElem, PdfIndirectReference refa, PdfDictionary dict) {
            this.structElem = structElem;
            this.refa = refa;
            this.obj = dict.GetDirectObject(PdfName.OBJ);
            this.objref = dict.GetAsIndirectObject(PdfName.OBJ);
            this.structParent = ((PdfDictionary) obj).GetAsNumber(PdfName.STRUCTPARENT).IntValue;
            PdfIndirectReference pg = dict.GetAsIndirectObject(PdfName.PG);
            if (pg == null)
                pg = structElem.GetAsIndirectObject(PdfName.PG);
            this.pageref = pg.Number;
        }

        /**
         * Returns the structure element.
         * @return a StructElem dictionary
         */
        virtual public PdfDictionary GetStructElem() {
            return structElem;
        }

        /**
         * Returns the structure element's reference.
         * @return an indirect reference to a StructElem dictionary
         */
        virtual public PdfIndirectReference GetRef() {
            return refa;
        }

        /**
         * Returns the object referred to by the OBJR dictionary.
         * Note that this method returns a dictionary which means
         * that in case the object is a stream, only the stream
         * dictionary will be returned.
         * @return the object referred to by OBJR as a dictionary
         */
        virtual public PdfDictionary GetObjAsDict() {
            if (obj.IsDictionary())
                return (PdfDictionary) obj;
            return null;
        }

        /**
         * Returns the reference to the object.
         * @return	an object reference
         */
        virtual public PdfIndirectReference GetObjRef() {
            return objref;
        }

        /**
         * Checks if a StructParent corresponds with the StructParent stored in the object.
         * @return  1 in case the StructParent matches,
         *         -1 in case there's no match.
         */
        public override int CheckStructParent(int pg, int sp) {
            if (pg == pageref && sp == structParent)
                return 1;
            return -1;
        }

        /**
         * Creates a String representation of the object.
         * @see java.lang.Object#toString()
         */
        public override String ToString() {
            return "StructParent " + structParent + " of object " + obj + " on page with reference " + pageref;
        }
    }
}
