/**
Lightweight profiler library for c++
Copyright(C) 2016-2019  Sergey Yagovtsev, Victor Zarubkin

Licensed under either of
    * MIT license (LICENSE.MIT or http://opensource.org/licenses/MIT)
    * Apache License, Version 2.0, (LICENSE.APACHE or http://www.apache.org/licenses/LICENSE-2.0)
at your option.

The MIT License
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is furnished
    to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
    LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
    USE OR OTHER DEALINGS IN THE SOFTWARE.


The Apache License, Version 2.0 (the "License");
    You may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

**/

#include <fstream>

#include <QFile>

#include <easy/writer.h>

#include "common_functions.h"
#include "globals.h"
#include "file_reader.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

FileReader::FileReader()
{

}

FileReader::~FileReader()
{
    interrupt();
}

const bool FileReader::isFile() const
{
    return m_isFile;
}

const bool FileReader::isSaving() const
{
    return m_jobType == JobType::Saving;
}

const bool FileReader::isLoading() const
{
    return m_jobType == JobType::Loading;
}

const bool FileReader::isSnapshot() const
{
    return m_isSnapshot;
}

bool FileReader::done() const
{
    return m_bDone.load(std::memory_order_acquire);
}

int FileReader::progress() const
{
    return m_progress.load(std::memory_order_acquire);
}

unsigned int FileReader::size() const
{
    return m_size.load(std::memory_order_acquire);
}

const QString& FileReader::filename() const
{
    return m_filename;
}

void FileReader::load(const QString& _filename)
{
    interrupt();

    m_jobType = JobType::Loading;
    m_isFile = true;
    m_isSnapshot = false;
    m_filename = _filename;

    m_thread = std::thread([this] (bool _enableStatistics)
    {
        const auto size = fillTreesFromFile(m_progress, m_filename.toStdString().c_str(), m_beginEndTime, m_serializedBlocks,
                                            m_serializedDescriptors, m_descriptors, m_blocks, m_blocksTree,
                                            m_bookmarks, m_descriptorsNumberInFile, m_version, m_pid,
                                            _enableStatistics, m_errorMessage);

        m_size.store(size, std::memory_order_release);
        m_progress.store(100, std::memory_order_release);
        m_bDone.store(true, std::memory_order_release);

    }, EASY_GLOBALS.enable_statistics);
}

void FileReader::load(std::stringstream& _stream)
{
    interrupt();

    m_jobType = JobType::Loading;
    m_isFile = false;
    m_isSnapshot = false;
    m_filename.clear();

#if defined(__GNUC__) && __GNUC__ < 5 && !defined(__llvm__)
// gcc 4 has a known bug which has been solved in gcc 5:
    // std::stringstream has no swap() method :(
    // have to copy all contents... Use gcc 5 or higher!
#pragma message "Warning: in gcc 4 and lower std::stringstream has no swap()! Memory consumption may increase! Better use gcc 5 or higher instead."
    m_stream.str(_stream.str());
#else
    m_stream.swap(_stream);
#endif

    m_thread = std::thread([this] (bool _enableStatistics)
    {
        std::ofstream cache_file(NETWORK_CACHE_FILE, std::fstream::binary);
        if (cache_file.is_open())
        {
            cache_file << m_stream.str();
            cache_file.close();
        }

        const auto size = fillTreesFromStream(m_progress, m_stream, m_beginEndTime, m_serializedBlocks, m_serializedDescriptors,
                                              m_descriptors, m_blocks, m_blocksTree, m_bookmarks, m_descriptorsNumberInFile,
                                              m_version, m_pid, _enableStatistics, m_errorMessage);

        m_size.store(size, std::memory_order_release);
        m_progress.store(100, std::memory_order_release);
        m_bDone.store(true, std::memory_order_release);

    }, EASY_GLOBALS.enable_statistics);
}

void FileReader::save(const QString& _filename, profiler::timestamp_t _beginTime, profiler::timestamp_t _endTime,
                      const profiler::SerializedData& _serializedDescriptors,
                      const profiler::descriptors_list_t& _descriptors, profiler::block_id_t descriptors_count,
                      const profiler::thread_blocks_tree_t& _trees, const profiler::bookmarks_t& bookmarks,
                      profiler::block_getter_fn block_getter, profiler::processid_t _pid, bool snapshotMode)
{
    interrupt();

    m_jobType = JobType::Saving;
    m_isFile = true;
    m_isSnapshot = snapshotMode;
    m_filename = _filename;

    auto serializedDescriptors = std::ref(_serializedDescriptors);
    auto descriptors = std::ref(_descriptors);
    auto trees = std::ref(_trees);
    auto bookmarksRef = std::ref(bookmarks);

    m_thread = std::thread([=] (profiler::block_getter_fn getter)
    {
        const QString tmpFile = m_filename + ".tmp";

        const auto result = writeTreesToFile(m_progress, tmpFile.toStdString().c_str(), serializedDescriptors,
                                             descriptors, descriptors_count, trees, bookmarksRef, getter,
                                             _beginTime, _endTime, _pid, m_errorMessage);

        if (result == 0 || !m_errorMessage.str().empty())
        {
            // Remove temporary file in case of error
            QFile::remove(tmpFile);
        }
        else
        {
            // Remove old file if exists
            {
                QFile out(m_filename);
                if (out.exists())
                out.remove();
            }

            QFile::rename(tmpFile, m_filename);
        }

        m_progress.store(100, std::memory_order_release);
        m_bDone.store(true, std::memory_order_release);

    }, std::move(block_getter));
}

void FileReader::interrupt()
{
    join();

    m_bDone.store(false, std::memory_order_release);
    m_size.store(0, std::memory_order_release);
    m_serializedBlocks.clear();
    m_serializedDescriptors.clear();
    m_descriptors.clear();
    m_blocks.clear();
    m_blocksTree.clear();
    m_bookmarks.clear();
    m_descriptorsNumberInFile = 0;
    m_version = 0;
    m_pid = 0;
    m_jobType = JobType::Idle;
    m_isSnapshot = false;

    profiler_gui::clear_stream(m_stream);
    profiler_gui::clear_stream(m_errorMessage);
}

void FileReader::get(profiler::SerializedData& _serializedBlocks, profiler::SerializedData& _serializedDescriptors,
                     profiler::descriptors_list_t& _descriptors, profiler::blocks_t& _blocks,
                     profiler::thread_blocks_tree_t& _trees, profiler::bookmarks_t& bookmarks,
                     profiler::BeginEndTime& beginEndTime, uint32_t& _descriptorsNumberInFile, uint32_t& _version,
                     profiler::processid_t& _pid, QString& _filename)
{
    if (done())
    {
        m_serializedBlocks.swap(_serializedBlocks);
        m_serializedDescriptors.swap(_serializedDescriptors);
        profiler::descriptors_list_t(std::move(m_descriptors)).swap(_descriptors);
        m_blocks.swap(_blocks);
        m_blocksTree.swap(_trees);
        m_bookmarks.swap(bookmarks);
        m_filename.swap(_filename);
        beginEndTime = m_beginEndTime;
        _descriptorsNumberInFile = m_descriptorsNumberInFile;
        _version = m_version;
        _pid = m_pid;
    }
}

void FileReader::join()
{
    m_progress.store(-100, std::memory_order_release);
    if (m_thread.joinable())
    {
        m_thread.join();
    }
    m_progress.store(0, std::memory_order_release);
}

QString FileReader::getError() const
{
    return QString(m_errorMessage.str().c_str());
}
