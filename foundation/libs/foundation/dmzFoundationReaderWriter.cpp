/*!

\class dmz::Reader
\ingroup Foundation
\brief Data reading interface.
\sa dmz::ReaderFile \n dmz::ReaderZip

\fn dmz::Reader::Reader ()
\brief Constructor.

\fn dmz::Reader::~Reader ()
\brief Destructor.

\fn dmz::Boolean dmz::Reader::open_file (const String &FileName)
\brief Opens file for reading.
\details Any previously opened file will be closed.
\param[in] FileName String containing the name of the file to open.
\return Returns dmz::True if the file was successfully opened for reading.

\fn dmz::String dmz::Reader::get_file_name () const
\brief Gets name of file being read.
\return Returns a String containing the name of the opened file. Returns an empty
string if no file has been opened.

\fn dmz::UInt64 dmz::Reader::get_file_size () const
\brief Gets the size of the file being read.
\return Returns the size of the currently open file. Returns zero if no file is open.

\fn dmz::Int32 dmz::Reader::read_file (void *buffer, const Int32 Size)
\brief Reads the next block of data from the file.
\param[out] buffer Pointer to the buffer to store the next block of the file.
\param[in] Size Number of bytes in the buffer.
\return Returns the number of bytes stored in the buffer. Returns zero if there is no
more data to read from the file or if there is no open file to read from.

\fn dmz::Boolean dmz::Reader::close_file ()
\brief Closes current file.A
\return Returns dmz::True if the file was closed.

\fn dmz::String dmz::Reader::get_error () const
\brief Gets String containing last error.
\return Returns a String containing the last error.

\class dmz::Writer
\ingroup Foundation
\brief Data writing interface.
\sa dmz::WriterFile \n dmz::WriterZip

\fn dmz::Writer::Writer ()
\brief Constructor.

\fn dmz::Writer::~Writer ()
\brief Destructor.

\fn dmz::Boolean dmz::Writer::open_file (const String &FileName)
\brief Opens file for writing.
\details If the file already exists, it will be over written. Any previously opened
file will be closed.
\param[in] FileName String containing the name of the file to open.
\return Returns dmz::True if the file was successfully opened for writing.

\fn dmz::String dmz::Writer::get_file_name () const
\brief Gets name of file being written.
\return Returns a String containing the name of the opened file. Returns an empty
string if no file has been opened.

\fn dmz::Boolean dmz::Writer::write_file (const void *Buffer, const Int32 Size)
\brief Writes next block of data to the file.
\param[in] Buffer Pointer to the buffer to write the next block of the file.
\param[in] Size Number of bytes in the buffer.
\return Returns dmz::True if the data was successfully written to the file.
Returns dmz::False if the write failed or there was no file open writing.

\fn dmz::Boolean dmz::Writer::close_file ()
\brief Closes current file.
\return Returns dmz::True if the file was closed.

\fn dmz::String dmz::Writer::get_error () const
\brief Gets String containing last error.
\return Returns a String containing the last error.

*/
