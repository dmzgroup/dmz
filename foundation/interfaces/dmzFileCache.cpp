/*

This file is NOT compiled and is strictly for documentation purposes.

*/

/*!

\enum FileCacheResultEnum
\ingroup Foundation
\brief Enumeration specifying cache file request.

\class dmz::FileCacheAction
\ingroup Foundation
\brief Provides interface for processing file cache requests.
\details The results from a dmz::FileCach::add_file_request are returned to
the requester via the dmz::FileCacheAction::process_file pure virtual function.

\fn void dmz::FileCacheAction::process_file (
const FileCacheResultEnum RequestResult,
const String &LocalFilePath,
const String &RequestedFileLocation,
const String &RequestedFileName)
\brief Pure virtual function invoked to process requested file.
\details Derived classes should implement this function to receive notification
when the cache has a result from the file request.
\param[in] RequestResult Result of the file request.
\param[in] LocalFilePath String containing the path on the local file system pointing
to the requested file.
\param[in] RequestedFileLocation String containing original path used in the file request.
The value should match the \a FileLocation parameter from the
dmz::FileCache::add_file_request call.
\param[in] RequestedFileName String containing original file name used in the file
request.  The value should match the \a FileName parameter from the
dmz::FileCache::add_file_request call.
\sa dmz::FileCache::add_file_request

\class dmz::FileCache
\ingroup Foundation
\brief Asynchronous file finder.
\details The FileCache provides an interface for asynchronous file loading.

\fn dmz::Boolean dmz::FileCache::process_all_requests (const Float64 TimeOut = -1.0)
\brief Forces the FileCache to process all requests.
\details This function will block until all file requests have been processed.
\param[in] TimeOut Amount of time in seconds to wait for all requested files before
timing out the request.

\fn void dmz::FileCache::add_file_request (
const String &FileLocation,
const String &FileName,
FileCacheAction &action)
\brief Requests a file from the FileCache.
\param[in] FileLocation String containing the location of the file.
\param[in] FileName String containing the name of the file. The file name should \b not
contain any path information.
\param[in] action FileCacheAction to send the file request result.

*/
