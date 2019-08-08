﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using iTextSharp.text;
using iTextSharp.text.error_messages;
using iTextSharp.text.log;
using iTextSharp.text.pdf;

namespace itextsharp.pdfa.iTextSharp.text.pdf {

    /**
     * PdfASmartCopy has the same functionality as PdfACopy,
     * but when resources (such as fonts, images,...) are
     * encountered, a reference to these resources is saved
     * in a cache, so that they can be reused.
     * This requires more memory, but reduces the file size
     * of the resulting PDF document.
     */

    public class PdfASmartCopy : PdfACopy {

        private static readonly ILogger LOGGER = LoggerFactory.GetLogger(typeof(PdfSmartCopy));

        /** the cache with the streams and references. */
        private Dictionary<PdfSmartCopy.ByteStore, PdfIndirectReference> streamMap = null;
        private Dictionary<RefKey, int> serialized = new Dictionary<RefKey, int>();

        /**
         * Constructor
         *
         * @param document         document
         * @param os               outputstream
         * @param conformanceLevel
         */
        public PdfASmartCopy(Document document, Stream os, PdfAConformanceLevel conformanceLevel) : base(document, os, conformanceLevel) {
            this.streamMap = new Dictionary<PdfSmartCopy.ByteStore, PdfIndirectReference>();
        }

        protected override PdfIndirectReference CopyIndirect(PRIndirectReference @in) {
            PdfObject srcObj = PdfReader.GetPdfObjectRelease(@in);
            PdfSmartCopy.ByteStore streamKey = null;
            bool validStream = false;
            if (srcObj.IsStream()) {
                streamKey = new PdfSmartCopy.ByteStore((PRStream) srcObj, serialized);
                validStream = true;
                PdfIndirectReference streamRef;
                if (streamMap.TryGetValue(streamKey, out streamRef)) {
                    return streamRef;
                }
            } else if (srcObj.IsDictionary()) {
                streamKey = new PdfSmartCopy.ByteStore((PdfDictionary) srcObj, serialized);
                validStream = true;
                PdfIndirectReference streamRef;
                if (streamMap.TryGetValue(streamKey, out streamRef)) {
                    return streamRef;
                }
            }

            PdfIndirectReference theRef;
            RefKey key = new RefKey(@in);
            IndirectReferences iRef;
            if (indirects.TryGetValue(key, out iRef)) {
                theRef = iRef.Ref;
                if (iRef.Copied) {
                    return theRef;
                }
            } else {
                theRef = body.PdfIndirectReference;
                iRef = new IndirectReferences(theRef);
                indirects[key] = iRef;
            }
            if (srcObj.IsDictionary()) {
                PdfObject type = PdfReader.GetPdfObjectRelease(((PdfDictionary)srcObj).Get(PdfName.TYPE));
                if (type != null) {
                    if ((PdfName.PAGE.Equals(type))) {
                        return theRef;
                    }
                    if ((PdfName.CATALOG.Equals(type))) {
                        LOGGER.Warn(MessageLocalization.GetComposedMessage("make.copy.of.catalog.dictionary.is.forbidden"));
                        return null;
                    }
                }
            }

            iRef.SetCopied();

            if (validStream) {
                streamMap[streamKey] = theRef;
            }

            PdfObject obj = CopyObject(srcObj);
            AddToBody(obj, theRef);
            return theRef;
        }

        public override void FreeReader(PdfReader reader) {
            serialized.Clear();
            base.FreeReader(reader);
        }

        public override void AddPage(PdfImportedPage iPage) {
            if (currentPdfReaderInstance.Reader != reader)
                serialized.Clear();
            base.AddPage(iPage);
        }
    }
}
