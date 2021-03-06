/**
 * Copyright © 2009-2014 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StRawFile_h__
#define __StRawFile_h__

#include <StFile/StFileNode.h>

struct AVIOContext;
struct AVIOInterruptCB;

/**
 * @class class to access to the small binary and text files.
 */
class StRawFile : public StFileNode {

        public:

    typedef enum tagReadWrite {
        READ,
        WRITE,
    } ReadWrite;

        public:

    ST_CPPEXPORT StRawFile(const StCString& theFilePath = stCString(""),
                           StNode*          theParent   = NULL);

    ST_CPPEXPORT virtual ~StRawFile();

    /**
     * Access to the raw buffer.
     */
    const stUByte_t* getBuffer() const {
        return myBuffer;
    }

    /**
     * Access to the raw buffer.
     */
    stUByte_t* changeBuffer() {
        return myBuffer;
    }

    /**
     * @return the buffer size in bytes.
     */
    size_t getSize() const {
        return myBuffSize;
    }

    /**
     * Casts the raw buffer as string.
     */
    StString getAsANSIText() const {
        return StString((const char* )myBuffer);
    }

    /**
     * (Re)initialize the buffer.
     * @param theDataSize (size_t ) - new buffer size in bytes.
     */
    ST_CPPEXPORT void initBuffer(size_t theDataSize);

    /**
     * Free current buffer.
     */
    ST_CPPEXPORT void freeBuffer();

    /**
     * Returns true if file is opened.
     */
    bool isOpen() const {
        return myFileHandle != NULL
            || myContextIO  != NULL;
    }

    /**
     * Open the file handle for read or write operation.
     */
    ST_CPPEXPORT bool openFile(StRawFile::ReadWrite theFlags,
                               const StCString&     theFilePath = stCString(""));

    /**
     * Close file handle
     */
    ST_CPPEXPORT void closeFile();

    /**
     * Write current buffer content to the file.
     * If size not set - whole buffer will be saved.
     */
    ST_CPPEXPORT virtual size_t writeFile(size_t theBytes = 0);

    /**
     * Write buffer to the file.
     */
    ST_LOCAL inline size_t write(const StCString& theString) {
        return write(theString.toCString(), theString.getSize());
    }

    /**
     * Write buffer to the file.
     */
    ST_CPPEXPORT size_t write(const char*  theBuffer,
                              const size_t theBytes);

    /**
     * Fill the buffer with file content.
     * @param theFilePath the file path
     * @return true if file was read
     */
    ST_CPPEXPORT virtual bool readFile(const StCString& theFilePath = stCString(""));

    /**
     * Write the buffer into the file.
     * @param theFilePath the file path
     * @return true if file was stored
     */
    ST_CPPEXPORT bool saveFile(const StCString& theFilePath = stCString(""));

    /**
     * Read the text file and return it as a string.
     */
    ST_CPPEXPORT static StString readTextFile(const StCString& theFilePath);

        private:

    /**
     * Interruption callback.
     */
    ST_LOCAL static int avInterruptCallback(void* thePtr);

    /**
     * Interruption callback.
     * @return 0 to continue, 1 to abort
     */
    ST_LOCAL int onInterrupted() { return 0; }

        protected:

    AVIOContext* myContextIO;  //!< file context
    FILE*        myFileHandle; //!< file handle
    stUByte_t*   myBuffer;     //!< buffer with file content
    size_t       myBuffSize;   //!< buffer size
    size_t       myLength;     //!< data length

};

#endif //__StRawFile_h__
